#include "megadrive.hpp"
#include <Arduino.h>

// A20 controls read/write
// CE is plugged directly to flash
// OE is transformed to OE or WE depending on A20 state

// Chip enable
#define P_NE PG0
#define PORT_NE PORTG
#define DDR_NE DDRG
// Output enable
#define P_NOE PG1
#define PORT_NOE PORTG
#define DDR_NOE DDRG

#define PORT_ADDR_LO PORTL
#define DDR_ADDR_LO DDRL
#define PORT_ADDR_MID PORTK
#define DDR_ADDR_MID DDRK
#define PORT_ADDR_HI PORTB
#define DDR_ADDR_HI DDRB

#define PORT_DATA_LO PORTA
#define DDR_DATA_LO DDRA
#define PIN_DATA_LO PINA
#define PORT_DATA_HI PORTC
#define DDR_DATA_HI DDRC
#define PIN_DATA_HI PINC

#define ADDR_PRG_LINE 20

tMegadrive::tMegadrive(void) {
  // Data bus
  DDR_DATA_LO = 0; // Lower
	PORT_DATA_LO = 0;
  DDR_DATA_HI = 0; // Upper
	PORT_DATA_HI = 0;

  // Address bus
  DDR_ADDR_LO = 0xFF; // Lower - A7..A0
  DDR_ADDR_MID = 0xFF; // Higher - A15..A8
  DDR_ADDR_HI = 0b01111111; // Highest - up to A22..A15, without A23

	PORT_ADDR_LO = 0;
	PORT_ADDR_MID = 0;
	PORT_ADDR_HI = 0;

  // Control bits
	DDR_NE |= _BV(P_NE);
	DDR_NOE |= _BV(P_NOE);
	PORT_NE |= _BV(P_NE);
	DDR_NOE |= _BV(P_NOE);
}

bool tMegadrive::writeData(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulValue) {
	if(ubDepth != 2) {
		return false;
	}
	ulValue &= 0xFFFF;

	writeCycle(0x555, 0xAA);
	writeCycle(0x2AA, 0x55);
	writeCycle(0x555, 0xA0);
	writeCycle(ulAddr, ulValue);

	// Verify
	uint32_t ulVerify;
	do {
		if(!readData(2, ulAddr, ulVerify)) {
			return false;
		}
		if((ulVerify & _BV(7)) == (ulValue & _BV(7))) {
			return true;
		}
	} while(ulVerify & _BV(7));

	readData(2, ulAddr, ulVerify);
	if((ulVerify & _BV(7)) == (ulValue & _BV(7))) {
		return true;
	}
	return false;
}

bool tMegadrive::readData(uint8_t ubDepth, uint32_t ulAddr, uint32_t &ulResult) {
	const uint16_t ubTime = 2;
	if(ubDepth != 2) {
		return false;
	}
  // Set address lines
  PORT_ADDR_LO = ulAddr & 0xFF;
  PORT_ADDR_MID = (ulAddr >> 8) & 0xFF;
  PORT_ADDR_HI = (ulAddr >> 16) & 0b01111111;

	// Set to input without pullup
	PORT_DATA_LO = 0;
  PORT_DATA_HI = 0;
	DDR_DATA_HI = 0;
	DDR_DATA_LO = 0;

  // Set control lines lo
  delayMicroseconds(ubTime);
	PORT_NE &= ~ _BV(P_NE);
	PORT_NOE &= ~ _BV(P_NOE);
  delayMicroseconds(ubTime);

  // Read from data lines
  ulResult = (PIN_DATA_HI << 8) | PIN_DATA_LO;

  // Set control lines hi
	PORT_NOE |= _BV(P_NOE);
	PORT_NE |= _BV(P_NE);
  delayMicroseconds(ubTime);

  return true;
}

void tMegadrive::relax(void) {
	const uint8_t ubTime = 2;
  // Set control lines hi
	PORT_NOE |= _BV(P_NOE);
	PORT_NE |= _BV(P_NE);

	// Set address lines low
	delayMicroseconds(ubTime);
	PORT_ADDR_LO = 0;
	PORT_ADDR_MID = 0;
	PORT_ADDR_HI = 0;

	// Set data  lines to input without pullup
	PORT_DATA_LO = 0;
  PORT_DATA_HI = 0;
	DDR_DATA_HI = 0;
	DDR_DATA_LO = 0;
}

void tMegadrive::writeCycle(uint32_t ulAddr, uint16_t uwData) {
	const uint8_t ubTime = 2;

	// Set address lines
	PORT_ADDR_LO = ulAddr & 0xFF;
  PORT_ADDR_MID = (ulAddr >> 8) & 0xFF;
  PORT_ADDR_HI |= (ulAddr >> 16) & 0b01111111;

	// Set data to output
	DDR_DATA_LO = 0xFF;
	DDR_DATA_HI = 0xFF;

	PORT_DATA_LO = uwData & 0xFF;
  PORT_DATA_HI = (uwData >> 8) & 0xFF;

  // Set control lines lo - address latches on falling edge
  delayMicroseconds(ubTime);
	PORT_ADDR_HI |= _BV(ADDR_PRG_LINE - 16);
	PORT_NE &= ~ _BV(P_NE);
	PORT_NOE &= ~ _BV(P_NOE);
	delayMicroseconds(ubTime);

	// Set control lines hi - data latches on rising edge
	PORT_NOE |= _BV(P_NOE);
	PORT_NE |= _BV(P_NE);
	PORT_ADDR_HI &= ~ _BV(ADDR_PRG_LINE - 16);
}

bool tMegadrive::waitToggle(void) {
	constexpr uint8_t ubToggleMask = _BV(2) | _BV(6);
	uint32_t ulStatusOld, ulStatus;
	readData(2, 0, ulStatusOld);
	do {
		readData(2, 0, ulStatus);
		if((ulStatusOld & ubToggleMask) == (ulStatus & ubToggleMask)) {
			return true;
		}
		ulStatusOld = ulStatus;
	} while(!(ulStatus & _BV(5)));
	readData(2, 0, ulStatusOld);
	readData(2, 0, ulStatus);
	if((ulStatusOld & ubToggleMask) == (ulStatus & ubToggleMask)) {
		return true;
	}
	return false;
}

bool tMegadrive::eraseAll(void) {
	writeCycle(0x555, 0xAA);
	writeCycle(0x2AA, 0x55);
	writeCycle(0x555, 0x80);
	writeCycle(0x555, 0xAA);
	writeCycle(0x2AA, 0x55);
	writeCycle(0x555, 0x10);

	return waitToggle();
}
