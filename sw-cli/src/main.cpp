#include <cstdio>
#include <thread>
#include <serial/serial.h>

#define RECV_BUF_LEN 1024

static void printUsage(const char *szAppName) {
		printf("Usage: %s port op args\n", szAppName);
		auto vPorts = serial::list_ports();
		printf("Available ports: %d\n", vPorts.size());
		for(auto &Port: vPorts) {
			printf(
				"\t%s\t\t%s\n",
				Port.port.c_str(), Port.description.c_str()
			);
		}
		printf("Available ops:\n");
		printf("\tread start count path - read to file given count of words from ROM, beginning from offs");
}

int main(int32_t lArgCnt, char *pArgs[]) {
	char pRecvBuf[RECV_BUF_LEN];
	int lReadByteCount, lResult;

	if(lArgCnt < 3) {
		printUsage(pArgs[0]);
		return 0;
	}

	try {
		int32_t lBaud = 76800;
		std::string szPort(pArgs[1]);
		serial::Serial Serial(szPort, lBaud, serial::Timeout::simpleTimeout(200));
		printf("Connected to %s@%d\n", pArgs[1], lBaud);
		std::this_thread::sleep_for(std::chrono::seconds(1));

		if(!strcmp(pArgs[2], "read")) {
			if(lArgCnt != 6) {
				printUsage(pArgs[0]);
				return 0;
			}
			uint32_t ulOffs = strtoul(pArgs[3], nullptr, 0);
			uint32_t ulCnt = strtoul(pArgs[4], nullptr, 0);
			if(ulOffs == ULONG_MAX || ulCnt == ULONG_MAX) {
				printf("ERR: Invalid offs/cnt\n");
				printUsage(pArgs[0]);
				return 0;
			}
			Serial.flushInput();
			FILE *pFile = fopen(pArgs[5], "wb");
			std::string szCmd = "read "+ std::to_string(ulOffs) + " " + std::to_string(ulCnt) + "\n";
			Serial.write(szCmd);
			Serial.readline(100, "\n");
			for(uint32_t i = 0; i < ulCnt; ++i) {
				uint8_t pBfr[2];
				if(!Serial.read(pBfr, 2)) {
					printf("ERR: Read error at offset %d+%u\n", ulOffs, i);
					return 0;
				}
				fwrite(pBfr, sizeof(uint16_t), 1, pFile);
			}
			fclose(pFile);
			printf("All done!\n");
		}
		else {
			printf("ERR: Unknown cmd\n");
			printUsage(pArgs[0]);
			return 0;
		}
	}
	catch(std::exception Exc) {
		printf("Exception: %s", Exc.what());
	}

	return 0;
}
