#ifndef _ROMPRG_FW_FLASH_MEGADRIVE_H_
#define _ROMPRG_FW_FLASH_MEGADRIVE_H_

#include "../flash.hpp"

/**
 * @brief Class for programming megadrive cart.
 */
class tMegadrive: public tFlash {
public:
	tMegadrive(void);

	// Implementation of tFlash
	virtual bool writeData(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulValue);
	virtual bool readData(uint8_t ubDepth, uint32_t ulAddr, uint32_t &ulResult);
	virtual bool eraseAll(void);
	virtual void relax(void);

private:
	/**
	 * @brief Issues single byte write to chip at given address.
	 */
	void writeCycle(uint32_t ulAddr, uint16_t uwData);

	/**
	 * @brief Checks for erase operation to end.
	 *
	 * @return True on success, otherwise false.
	 */
	bool waitToggle(void);
};

#endif // _ROMPRG_FW_FLASH_MEGADRIVE_H_
