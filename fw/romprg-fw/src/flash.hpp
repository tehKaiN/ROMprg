#ifndef _ROMPRG_FW_FLASH_H_
#define _ROMPRG_FW_FLASH_H_

#include <stdint.h>

class tFlash {
public:
	/**
	 * @brief Processes string command.
	 *
	 * Supported commands:
	 * 	- `read depth start length`
	 *  - `write depth start hex1 [hex2 [hex3 [hex4]]]`
	 *  - `mask depth start length`
	 *  - `chkerase depth start length`
	 *  - `erase_all`
	 * Args:
	 * - depth: 1|2|4 - bytes per native word.
	 * - start: address corresponding to data depth.
	 * - length: number of reads of given depth.
	 * - hex: hexadecimal byte input without prefix. Multibytes are big endian.
	 *   To send 0x11223344 write hexes: 11 22 33 44.
	 *
	 *
	 * @param szCmd String containing command to execute.
	 * @return True on success, otherwise false.
	 */
	bool processCommand(const char *szCmd);

//----------------------------------------------------------------- STATIC STUFF

	static tFlash *fromString(const char *szChip);

//---------------------------------------------------------------- VIRTUAL STUFF

  virtual ~tFlash() = default;
	/**
	 * @brief Writes byte into Flash at given address.
	 *
	 * @param ulAddr Destination address.
	 * @param ubValue Value to be written.
	 * @return True on success, otherwise false.
	 */
	virtual bool writeData(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulValue) = 0;

	/**
	 * @brief Reads byte from specified address.
	 *
	 * @param ubDepth Bytes per native word.
	 * @param ulAddr Byte address from which read should occur.
	 * @return Byte read from memory.
	 */
	virtual bool readData(uint8_t ubDepth, uint32_t ulAddr, uint32_t &ulResult) = 0;

	/**
	 * @brief Erases whole chip.
	 * @return True on success, otherwise false.
	 */
	virtual bool eraseAll(void) = 0;

	/**
	 * @brief Sets chip in idle state.
	 */
	virtual void relax(void) = 0;

private:
	bool cmdRead(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulLength);
	bool cmdWrite(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulData);
	bool cmdMask(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulLength);
	bool cmdChkErase(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulLength);
	bool cmdErase(void);
	bool cmdWriteBuffered(uint32_t ulDepth, uint32_t ulAddr, uint32_t ulCount);
};

#endif // _ROMPRG_FW_FLASH_H_
