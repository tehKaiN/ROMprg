#include <thread>
#include <vector>
#include <serial/serial.h>
#include <fmt/format.h>
#include "options.hpp"

#define RECV_BUF_LEN 1024


static std::string s_szOutName = "";

static std::vector<std::string> s_vChips = {"megadrive"};

static void printUsage(const char *szAppName) {
	// romprg -c megadrive -rr -s 512k -n dupa
	fmt::print("Usage:\n\t{} port chip op args\n", szAppName);
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
		fmt::print("Connected to {}@{}\n", pArgs[1], lBaud);
		std::this_thread::sleep_for(std::chrono::seconds(1));

		if(!strcmp(pArgs[2], "read")) {
			if(lArgCnt != 6) {
				printUsage(pArgs[0]);
				return 0;
			}
			uint32_t ulOffs = strtoul(pArgs[3], nullptr, 0);
			uint32_t ulCnt = strtoul(pArgs[4], nullptr, 0);
			if(ulOffs == ULONG_MAX || ulCnt == ULONG_MAX) {
				fmt::print("ERR: Invalid offs/cnt\n");
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
					fmt::print("ERR: Read error at offset {}+{}\n", ulOffs, i);
					return 0;
				}
				fwrite(pBfr, sizeof(uint16_t), 1, pFile);
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
