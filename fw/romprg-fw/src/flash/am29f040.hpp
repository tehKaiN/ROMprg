#ifndef _ROMPRG_FW_FLASH_AM29F040_H_
#define _ROMPRG_FW_FLASH_AM29F040_H_

#include "../flash.hpp"

/**
 * @brief Class for programming AM29F040 chip.
 * @todo Rewrite it so that it may be used for any 8-bit AM29F chip.
 *       Needs returning size for that.
 */
class tAm29f040: public tFlash {
public:
	tAm29f040(void);

	// Implementation of tFlash
	virtual bool writeData(uint8_t ubDepth,  uint32_t ulAddr, uint32_t ulValue);
	virtual bool readData(uint8_t ubDepth, uint32_t ulAddr, uint32_t &ulResult);
	virtual bool eraseAll(void);
	virtual void relax(void);

private:
	/**
	 * @brief Issues single byte write to chip at given address.
	 */
	void writeCycle(uint32_t ulAddr, uint8_t ubData);

	/**
	 * @brief Checks for erase operation to end.
	 *
	 * @return True on success, otherwise false.
	 */
	bool waitToggle(void);
};

#endif // _ROMPRG_FW_FLASH_AM29F040_H_
