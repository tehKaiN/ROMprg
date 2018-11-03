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

static std::vector<std::string> s_vChips = {"megadrive"};

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
	fmt::print("\t-on\t\tOutput name, if omitted will use default\n");
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
	if(szChip != s_vChips[0]) {
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
				fmt::print("Deduced ROM name: {}\n", sHeader.getDomesticName());
				s_szOutName = fmt::format("{}.bin", sHeader.getDomesticName());
			}

			if(s_isAutoSize) {

			}
			else if (lSize <= 0 || (lSize % 1024) != 0) {
				fmt::print("ERR: Size unspecified or not divisible by 1024\n");
				printUsageDump();
				return 1;
			}
			uint32_t ulKilos = lSize / 1024;

			FILE *pFile = fopen(s_szOutName.c_str(), "wb");
			auto TimeStart = std::chrono::system_clock::now();
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
			auto TimeEnd = std::chrono::system_clock::now();
			fclose(pFile);
			float fTotalTime = duration_cast<milliseconds>(TimeEnd - TimeStart).count() / 1000.0;
			fmt::print("\nAll done! Total time: {:.2f}s\n", fTotalTime);
		}
		else {
			fmt::print("ERR: Unknown cmd\n");
			printUsage(pArgs[0]);
			return 0;
		}
	}
	catch(std::exception Exc) {
		fmt::print("Exception: {}", Exc.what());
	}

	return 0;
}
