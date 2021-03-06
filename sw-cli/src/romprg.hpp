#ifndef _ROMPRG_ROMPRG_HPP_
#define _ROMPRG_ROMPRG_HPP_

#include <string>
#include <cstdint>
#include <memory>
#include <serial/serial.h>

class tRomPrg {
public:
	tRomPrg(std::string szPort, uint32_t ulBaud);
	bool setChip(const std::string szChipName);
	bool readBytes(
		uint8_t ubDepth, uint8_t *pDest, uint32_t ulOffs, uint32_t ulSize
	);
	bool isOpened(void) {return isOpen;};
	bool erase(void);
	bool write(uint8_t ubDepth, uint32_t ulAddr, uint8_t *pData);
	int32_t getBufferSize(void);
	bool writeBuffered(
		uint8_t ubDepth, uint32_t ulAddr, uint8_t *pData, uint32_t ulWordCount
	);

private:
	std::unique_ptr<serial::Serial> m_pSerial = nullptr;
	bool isOpen = false;
};

#endif // _ROMPRG_ROMPRG_HPP_
