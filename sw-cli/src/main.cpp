#include <thread>
#include <vector>
#include <serial/serial.h>
#include <fmt/format.h>
#include "options.hpp"
#include "trim.hpp"

#define RECV_BUF_LEN 1024

static std::string s_szOutName = "";
static std::string s_szPort = "";

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
		int32_t lBaud = 76800;
		// Connect do romprg
		serial::Serial Serial(s_szPort, lBaud, serial::Timeout::simpleTimeout(200));
		std::this_thread::sleep_for(std::chrono::seconds(1));
		if(Serial.readline(100, "\n").substr(0, 6) == "romprg") {
			fmt::print("Connected to {}@{}\n", s_szPort, lBaud);
		}
		else {
			fmt::print("ERR: Unknown device on {}@{} failed\n", s_szPort, lBaud);
			return 1;
		}

		// Set chip
		Serial.write(fmt::format("chip {}\n", szChip));
		if(rtrim(Serial.readline(100, "\n")) != "OK") {
			fmt::print("ERR: Unsupported chip/device: {}", szChip);
			return 1;
		}

		// Process command
		std::string szResponse;
		if(eOp == tOp::DEV_DUMP) {
			if(lSize <= 0 || (lSize % 1024) != 0) {
				fmt::print("ERR: Size unspecified or not divisible by 1024\n");
				printUsageDump();
				return 1;
			}
			if(s_szOutName == "") {
				s_szOutName = fmt::format("{}.bin", szChip);
			}
			uint32_t ulKilos = lSize / 1024;

			Serial.flushInput();
			FILE *pFile = fopen(s_szOutName.c_str(), "wb");
			for(auto k = 0; k < ulKilos; ++k) {
				// Send cmd
				fmt::print("\r{}/{}", k, ulKilos);
				Serial.write(fmt::format("read 2 {} 512\n", k * 512));

				// Read init response
				szResponse = rtrim(Serial.readline(100, "\n"));
				if(szResponse != "START read") {
					fmt::print(
						"ERR: Wrong initialization text received: '{}'\n", szResponse
					);
					return 1;
				}

				// Read actual data + CRLF
				uint8_t pData[1024+2];
				int32_t lBytesRead = 0;
				for(auto i = 0; i < 1024+2; ++i) {
					int32_t lRead = Serial.read(&pData[i], 1);
					if(lRead) {
						++lBytesRead;
					}
				}

				// Read end response
				szResponse = rtrim(Serial.readline(100, "\n"));
				if(szResponse != "SUCC") {
					fmt::print("ERR: invalid end response: '{}'\n", szResponse);
					return 1;
				}

				// Put them into file
				fwrite(pData, 1, 512, pFile);
			}
			fclose(pFile);
			fmt::print("All done!\n");
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
