#include <thread>
#include <vector>
#include <chrono>
#include <serial/serial.h>
#include <fmt/format.h>
#include "options.hpp"
#include "trim.hpp"
#include "romprg.hpp"
#include "megadrive_header.hpp"

#define RECV_BUF_LEN 1024

static std::string s_szOutName = "";
static std::string s_szPort = "";
static bool s_isAutoSize = false;

static std::vector<std::string> s_vChips = {"megadrive", "at24c02"};

static void printUsage(const char *szAppName) {
	// romprg -c megadrive -rr -s 512k -n dupa
	fmt::print("Usage:\n\t{} port -c chip -op args\n", szAppName);
	auto vPorts = serial::list_ports();
	fmt::print("\nAvailable ports: {}\n", vPorts.size());
	for(auto &Port: vPorts) {
		fmt::print("\t{}\t\t{}\n", Port.port, Port.description);
	}
	fmt::print("\nAvailable chips:\n");
	for(auto &Chip: s_vChips) {
		fmt::print("\t{}\n", Chip);
	}
	fmt::print("\nAvailable ops:\n");
	for(auto &Op: g_mOpFromString) {
		fmt::print("\t{}\t\t{}\n", Op.first, g_mOpDescription.at(Op.second));
	}
	fmt::print("\nTo get op-specific help, write port, chip & op without further args\n");
}

static void printUsageDump(void) {
	fmt::print("Dump (-dd) options:\n");
	fmt::print("\t-n\t\tOutput name, if omitted will use default\n");
	fmt::print(
		"\t-s\t\tSize in bytes divisible by 1024.\n"
		"\t\t\tYou can use case-insensitive suffixes: 512k, 1m\n"
	);
	fmt::print("\t-as\t\tAutosize - will try to determine total size\n");
}

int32_t parseSize(std::string szSize) {
	int32_t lSize = 0;
	for(auto &c: szSize) {
		if(c >= '0' && c <= '9') {
			lSize = lSize*10 + (c - '0');
		}
		else if(c == 'k' || c == 'K') {
			lSize *= 1024;
		}
		else if(c == 'm' || c == 'M') {
			lSize *= 1024*1024;
		}
		else {
			return -1;
		}
	}
	return lSize;
}

int main(int32_t lArgCnt, char *pArgs[]) {
	char pRecvBuf[RECV_BUF_LEN];
	int lReadByteCount, lResult;
	tOp eOp = tOp::INVALID;
	std::string szChip;
	int32_t lSize = -1;

	if(lArgCnt < 3) {
		printUsage(pArgs[0]);
		return 0;
	}

	// First arg is always serial port
	s_szPort = pArgs[1];

	for(uint8_t i = 2; i < lArgCnt; ++i) {
		if(g_mOpFromString.find(pArgs[i]) != g_mOpFromString.end()) {
			eOp = g_mOpFromString.at(pArgs[i]);
		}
		else if(pArgs[i] == std::string("-c") && i+1 < lArgCnt) {
			// Chip
			szChip = pArgs[i+1];
			++i;
		}
		else if(pArgs[i] == std::string("-s") && i+1 < lArgCnt) {
			// Size
			lSize = parseSize(pArgs[i+1]);
			if(lSize == -1) {
				fmt::print("Invalid size: '{}'", lSize);
			}
		}
		else if(pArgs[i] == std::string("-as")) {
			s_isAutoSize = true;
		}
		else if(pArgs[i] == std::string("-n") && i+1 < lArgCnt) {
			// Name
			s_szOutName = pArgs[i+1];
			++i;
		}
	}

	// Check parameters if they look as remotely valid
	if(std::find(s_vChips.begin(), s_vChips.end(), szChip) == s_vChips.end()) {
		fmt::print("ERR: incorrect chip/device name: '{}'\n", szChip);
		printUsage(pArgs[0]);
		return 1;
	}

	try {
		const uint32_t ulBaud = 250000;
		tRomPrg RomPrg(s_szPort, ulBaud);
		if(RomPrg.isOpened()) {
			fmt::print("Connected to {}@{}\n", s_szPort, ulBaud);
		}
		else {
			fmt::print("ERR: Unknown device on {}@{}\n", s_szPort, ulBaud);
			return 1;
		}

		// Set chip
		if(!RomPrg.setChip(szChip)) {
			fmt::print("ERR: Unsupported chip/device: {}", szChip);
			return 1;
		}

		// Process command
		std::string szResponse;
		using std::chrono::duration_cast;
		using std::chrono::milliseconds;
		auto TimeStart = std::chrono::system_clock::now();
		if(eOp == tOp::DEV_DUMP) {
			if(s_szOutName == "") {
				// Get header
				tMegadriveHeader sHeader;
				bool isReadOk = RomPrg.readBytes(
					2, reinterpret_cast<uint8_t*>(&sHeader),
					0x100/2, sizeof(tMegadriveHeader)/2
				);
				if(!isReadOk) {
					fmt::print("ERR: Couldn't read ROM header\n");
					return 1;
				}
				fmt::print("Deduced ROM name: {}\n", sHeader.getOverseasName());
				s_szOutName = fmt::format("{}.bin", sHeader.getOverseasName());
			}

			if(s_isAutoSize) {
				uint8_t pStart[1024], pTest[1024];
				RomPrg.readBytes(2, pStart, 0, 512);
				// Try on 64k, 128k, 256k, 512k, 1m, 2m, 4m, 8m, 16m
				bool isFound = false;
				for(lSize = 64*1024; lSize <= 16*1024*1024; lSize <<= 1) {
					RomPrg.readBytes(2, pTest, lSize/2, 512);
					if(!memcmp(pStart, pTest, 1024)) {
						isFound = true;
						break;
					}
				}
				if(!isFound) {
					fmt::print("Couldn't determine ROM size\n");
					return 1;
				}
				else {
					fmt::print("Determined ROM size: {}k\n", lSize/1024);
				}
			}
			else if (lSize <= 0 || (lSize % 1024) != 0) {
				fmt::print("ERR: Size unspecified or not divisible by 1024\n");
				printUsageDump();
				return 1;
			}
			uint32_t ulKilos = lSize / 1024;

			FILE *pFile = fopen(s_szOutName.c_str(), "wb");
			for(auto k = 0; k < ulKilos; ++k) {
				auto PartStart = std::chrono::system_clock::now();

				uint8_t pData[1024];
				if(!RomPrg.readBytes(2, pData, k*512, 512)) {
					fmt::print("ERR: Read @ offset {}k\n", 2*512, k);
					return 1;
				}

				// Put them into file
				fwrite(pData, 1, 1024, pFile);
				auto PartEnd = std::chrono::system_clock::now();

				float fSpeed = 1000.0 / duration_cast<milliseconds>(PartEnd - PartStart).count();
				fmt::print(
					"\r{}: {}/{} ({:.2f}%, {:.2f}KB/s)", s_szOutName,
					k+1, ulKilos, (k+1)*100.0 / ulKilos, fSpeed
				);
			}
			fmt::print("\n");
			fclose(pFile);
		}
		else if(eOp == tOp::DEV_CLEAR) {
			fmt::print("Erasing...\n");
			if(!RomPrg.erase()) {
				fmt::print("ERR: erase failed\n");
				return 1;
			}
		}
		else if(eOp == tOp::DEV_BURN) {
			if(s_szOutName == "") {
				fmt::print("ERR: input not specified\n");
				return 1;
			}
			FILE *pIn = fopen(s_szOutName.c_str(), "rb");
			if(!pIn) {
				fmt::print("ERR: Couldn't open '{}'\n", s_szOutName);
				return 1;
			}

			// Check file size
			fseek(pIn, 0, SEEK_END);
			uint32_t ulSize = ftell(pIn);
			fseek(pIn, 0, SEEK_SET);
			if(!ulSize) {
				fmt::print("ERR: empty input file!\n");
				return 1;
			}

			// Read romprg's buffer size
			int32_t lBufferSize = RomPrg.getBufferSize();
			fmt::print("ROMprg write buffer size: {}\n", lBufferSize);

			fmt::print("Writing {}k into device...\n", ulSize / 1024);
			uint8_t pData[ulSize];
			fread(pData, 1, ulSize, pIn);
			fclose(pIn);

			// Get name
			tMegadriveHeader sHeader;
			memcpy(&sHeader, &pData[0x100], sizeof(tMegadriveHeader));
			std::string szRomName = sHeader.getOverseasName();

			// Write data into cart
			for(auto i = 0; i < ulSize; i += lBufferSize) {
				auto PartStart = std::chrono::system_clock::now();
				if(!RomPrg.writeBuffered(2, i/2, &pData[i], lBufferSize/2)) {
					fmt::print("ERR: Programming failed!\n");
					return 1;
				}
				auto PartEnd = std::chrono::system_clock::now();
				float fSpeed = 1000.0 / duration_cast<milliseconds>(PartEnd - PartStart).count();
				fmt::print(
					"\r{}: {}/{} ({:.2f}%, {}KB/s)",
					szRomName, i, ulSize, i*100.0 / ulSize, 0
				);
			}
			fmt::print("\n");
		}
		else if(eOp == tOp::READ || eOp == tOp::WRITE) {
			fmt::print("ERR: Not implemented, sorry!\n");
			return 1;
		}
		else {
			fmt::print("ERR: Unknown cmd\n");
			printUsage(pArgs[0]);
			return 1;
		}
		auto TimeEnd = std::chrono::system_clock::now();
		float fTotalTime = duration_cast<milliseconds>(TimeEnd - TimeStart).count() / 1000.0;
		fmt::print("All done! Total time: {:.2f}s\n", fTotalTime);
	}
	catch(std::exception Exc) {
		fmt::print("Exception: {}", Exc.what());
		return 1;
	}

	return 0;
}
