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

