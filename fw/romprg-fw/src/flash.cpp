#include "flash.hpp"
#include <Arduino.h>
#include "flash/am29f040.hpp"
#include "flash/at24c02.hpp"
#include "flash/megadrive.hpp"
#include "data_buffer.hpp"

static uint8_t onesCount(uint32_t ulData) {
	uint8_t ubCnt = 0;
	while(ulData) {
		if(ulData & 1) {
			++ubCnt;
		}
		ulData >>= 1;
	}
	return ubCnt;
}

bool tFlash::processCommand(const char *szCmd) {
	// Read message type
	char szCmdType[10];
	int32_t lCntRead = sscanf(szCmd, "%s", szCmdType);
	if(!lCntRead) {
		return false;
	}
	szCmd += strlen(szCmdType);
	bool isOk = false;
	uint32_t pArgs[4];
	uint8_t ubArgCnt = sscanf( // Hackty hack
		szCmd, "%lu %lu %lu %lu", &pArgs[0], &pArgs[1], &pArgs[2], &pArgs[3]
	);

	Serial.print("START "); Serial.println(szCmdType);
  if(!strcmp(szCmdType, "read") && ubArgCnt == 3) {
		isOk = cmdRead(pArgs[0], pArgs[1], pArgs[2]);
  }
  else if(!strcmp(szCmdType, "mask") && ubArgCnt == 3) {
		isOk = cmdMask(pArgs[0], pArgs[1], pArgs[2]);
  }
  else if(!strcmp(szCmdType, "chkerase") && ubArgCnt == 3) {
		isOk = cmdChkErase(pArgs[0], pArgs[1], pArgs[2]);
  }
	else if(!strcmp(szCmdType, "write")) {
		ubArgCnt = sscanf( // Hackty hack
			szCmd, "%lu %lu %lx %lu", &pArgs[0], &pArgs[1], &pArgs[2], &pArgs[3]
		);
		if(ubArgCnt == 3) {
			isOk = cmdWrite(pArgs[0], pArgs[1], pArgs[2]);
			if(!isOk) {
				Serial.println("ERR: write");
			}
		}
	}
	else if(!strcmp(szCmdType, "erase_all") && ubArgCnt == 255) {
		isOk = cmdErase();
	}
	else if(!strcmp(szCmdType, "buffer_size") && ubArgCnt == 255) {
		Serial.print("Size: ");
		Serial.println(DATA_BUFFER_SIZE);
		isOk = true;
	}
	else if(!strcmp(szCmdType, "write_buffered") && ubArgCnt == 3) {
		// depth, start addr, words in buffer (bytes / depth)
		// TODO: write_buffered error messages
		uint32_t &ulDepth = pArgs[0];
		uint32_t &ulAddr = pArgs[1];
		uint32_t &ulCount = pArgs[2];
		Serial.println("Waiting for read");
		uint32_t ulReadBytes = 0;
		do {
			// Serial.readBytes() has timeout so we need a loop
			ulReadBytes += Serial.readBytes(
				g_pDataBuffer, ulDepth * ulCount - ulReadBytes
			);
			// TODO: timeout for buffer read loop
		} while(ulReadBytes < ulDepth * ulCount);
		Serial.println("Writing...");
		isOk = true;
		if(pArgs[0] == 2) {
			for(uint16_t i = 0; i < pArgs[2]; ++i) {
				uint16_t uwData = (g_pDataBuffer[i * 2] << 8) | g_pDataBuffer[i * 2 + 1];
				if(!cmdWrite(pArgs[0], pArgs[1] + i, uwData)) {
					isOk = false;
					break;
				}
			}
		}
		else {
			// TODO: implement write_buffered depth 1
			// TODO: implement write_buffered depth 4
			isOk = false;
		}
	}
	else {
		Serial.println("ERR: Unknown command: '");
		Serial.print(szCmdType);
		Serial.print("', arg cnt: ");
		Serial.println(ubArgCnt);
	}
	relax();
	return isOk;
}

tFlash *tFlash::fromString(const char *szChip) {
	if(!strcmp(szChip, "am29f040")) {
		return new tAm29f040();
	}
	if(!strcmp(szChip, "megadrive")) {
		return new tMegadrive();
	}
	if(!strcmp(szChip, "at24c02")) {
		return new tAt24c02();
	}
	return nullptr;
}

bool tFlash::cmdRead(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulLength) {
	// Read mode
	if(ubDepth == 1 || ubDepth == 2 || ubDepth == 4) {
		for(uint32_t i = ulAddr; i < ulAddr + ulLength; ++i) {
			uint32_t ulRead;
			bool isOk = readData(ubDepth, i, ulRead);
			if(isOk) {
				for(uint8_t j = ubDepth; j--;) {
					uint8_t c = (ulRead >> (j * 8)) & 0xFF;
					Serial.write(c);
				}
			}
			else {
				Serial.println();
				Serial.print("ERR: Problem while reading addr:");
				Serial.println(ulAddr+i);
				return false;
			}
		}
		Serial.println();
	}
	else {
		Serial.print("ERR: Unsupported depth: ");
		Serial.println(ubDepth);
		return false;
	}
	return true;
}

bool tFlash::cmdWrite(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulData) {
	return writeData(ubDepth, ulAddr, ulData);
}

bool tFlash::cmdMask(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulLength) {
	if(ubDepth == 1) {
		uint8_t ubMask = 0;
		for(uint32_t i = ulAddr; i < ulAddr + ulLength; ++i) {
			uint32_t ulRead;
			bool isOk = readData(1, i, ulRead);
			ubMask |= ulRead;
		}
		char szBfr[3];
		sprintf(szBfr, "%02x", ubMask);
		Serial.print("Mask: ");
		Serial.println(szBfr);
	}
	else {
		// TODO: add mask depth 2
		// TODO: add mask depth 4
		Serial.print("ERR: Unsupported depth: ");
		Serial.println(ubDepth);
		return false;
	}
	return true;
}

bool tFlash::cmdChkErase(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulLength) {
	// Erase check mode
	uint32_t ulOnesCnt = 0;
	for(uint32_t i = ulAddr; i < ulAddr + ulLength; ++i) {
		uint32_t ulRead;
		bool isOk = readData(1, i, ulRead);
		ulOnesCnt += onesCount(ulRead);
	}
	uint32_t ulBitCnt = (ulLength*8);
	uint32_t ulZerosCnt = ulBitCnt - ulOnesCnt;
	double percentage = (ulZerosCnt *100.0)/ulBitCnt;
	char szDbl[10];
	dtostrf(percentage, 0, 2, szDbl);
	char bfr[255];
	sprintf(bfr, "Zeros: %lu / %lu (%s%%)", ulZerosCnt, ulBitCnt, szDbl);
	Serial.println(bfr);
	return true;
}

bool tFlash::cmdErase(void) {
	return eraseAll();
}

