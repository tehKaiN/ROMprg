#include "romprg.hpp"
#include <thread> // std::this_thread::sleep_for
#include <fmt/format.h>
#include "trim.hpp"

tRomPrg::tRomPrg(std::string szPort, uint32_t ulBaud) {
	try {
		m_pSerial = std::make_unique<serial::Serial>(
			szPort, ulBaud, serial::Timeout::simpleTimeout(1000)
		);
		std::this_thread::sleep_for(std::chrono::seconds(2));
		if(m_pSerial->readline(100, "\n").substr(0, 6) == "romprg") {
			isOpen = true;
		}
		else {
			isOpen = false;
		}
	} catch(...) {
		isOpen = false;
	}
}

bool tRomPrg::setChip(std::string szChipName) {
	m_pSerial->write(fmt::format("chip {}\n", szChipName));
	if(rtrim(m_pSerial->readline(100, "\n")) != "OK") {
		return false;
	}
	return true;
}

bool tRomPrg::readBytes(uint8_t ubDepth, uint8_t *pDest, uint32_t ulOffs, uint32_t ulSize) {
	// Send read cmd
	m_pSerial->write(fmt::format("read 2 {} {}\n", ulOffs, ulSize));

	// Read init response
	std::string szResponse = rtrim(m_pSerial->readline(100, "\n"));
	if(szResponse != "START read") {
		fmt::print(
			"ERR: Wrong initialization text received: '{}'\n", szResponse
		);
		return false;
	}

	// Read actual data + CRLF
	int32_t lBytesRead = 0;
	uint8_t ubBurst = 4;
	while(lBytesRead < ulSize * ubDepth) {
		int32_t lRead = m_pSerial->read(&pDest[lBytesRead], ulSize * ubDepth - lBytesRead);
		lBytesRead += lRead;
		if(lRead == 0) {
			fmt::print("ERR: Device stopped responding\n");
			return 1;
		}
	}
	m_pSerial->readline(100, "\n");

	// Read end response
	szResponse = rtrim(m_pSerial->readline(100, "\n"));
	if(szResponse != "SUCC") {
		fmt::print("ERR: invalid end response: '{}'\n", szResponse);
		return false;
	}
	return true;
}

bool tRomPrg::erase(void) {
	// Send cmd
	m_pSerial->write("erase_all\n");

	// Read initial response
	std::string szResponse = rtrim(m_pSerial->readline(100, "\n"));
	if(szResponse != "START erase_all") {
		fmt::print(
			"ERR: Wrong initialization text received: '{}'\n", szResponse
		);
		return false;
	}

	// Read final response
	do {
		szResponse = m_pSerial->readline(100, "\n");
	} while(szResponse == "");
	rtrimRef(szResponse);
	if(szResponse == "SUCC") {
		return true;
	}
	fmt::print("ERR: wrong final response: {}\n", szResponse);
	return false;
}

bool tRomPrg::write(uint8_t ubDepth, uint32_t ulAddr, uint8_t *pData) {
	// Send cmd
	std::string szCmd = fmt::format("write {} {} 0x", ubDepth, ulAddr);
	for(auto i = 0; i < ubDepth; ++i) {
		szCmd += fmt::format("{:02X}", pData[i]);
	}
	szCmd += "\n";
	m_pSerial->write(szCmd);

	// Read initial response
	std::string szResponse = rtrim(m_pSerial->readline(100, "\n"));
	if(szResponse != "START write") {
		fmt::print(
			"ERR: Wrong initialization text received: '{}'\n", szResponse
		);
		return false;
	}

	// Read final response
	do {
		szResponse = m_pSerial->readline(100, "\n");
	} while(szResponse == "");
	rtrimRef(szResponse);
	if(szResponse == "SUCC") {
		return true;
	}
	fmt::print("ERR: wrong final response: {}\n", szResponse);
	return false;
}

