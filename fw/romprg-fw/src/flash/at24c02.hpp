#ifndef _ROMPRG_FW_FLASH_AT24C02_H_
#define _ROMPRG_FW_FLASH_AT24C02_H_

#include "../flash.hpp"

/**
 * @brief Class for programming AT24C02 chip.
 * @todo Rewrite it so that it may be used for any 24C chip.
 *       Needs returning size for that.
 */
class tAt24c02: public tFlash {
public:
	tAt24c02(void);

	// Implementation of tFlash
	virtual bool writeData(uint8_t ubDepth,  uint32_t ulAddr, uint32_t ulValue);
	virtual bool readData(uint8_t ubDepth, uint32_t ulAddr, uint32_t &ulResult);
	virtual bool eraseAll(void);
	virtual void relax(void);

private:

};

#endif // _ROMPRG_FW_FLASH_AT24C02_H_
