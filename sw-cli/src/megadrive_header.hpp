#ifndef _ROMPRG_MEGADRIVE_HEADER_HPP_
#define _ROMPRG_MEGADRIVE_HEADER_HPP_

#include <stdint.h>
#include <string>

struct tMegadriveHeader {
	char pConsoleName[16];
	char pCopyright[16];
	char pNameDomestic[48];
	char pNameOverseas[48];
	char pSerial[14];
	uint16_t uwChecksum;
	char pIoSupport[16];
	uint32_t ulRomStart;
	uint32_t ulRomEnd;
	uint32_t ulRamStart;
	uint32_t ulRamEnd;
	char pModem[16];
	char pMemo[40];
	char pCountry[16];

	std::string getDomesticName(void);
	std::string getOverseasName(void);
} __attribute__((packed));

#endif // _ROMPRG_MEGADRIVE_HEADER_HPP_
