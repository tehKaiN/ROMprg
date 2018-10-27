#include "am29f040.hpp"
#include <Arduino.h>

// Chip enable
#define PIN_NE PG0
#define  PORT_NE PORTG
#define DDR_NE DDRG
// Write enable
#define PIN_NWE PG2
#define PORT_NWE PORTG
#define DDR_NWE DDRG
// Output enable
#define PIN_NOE PG1
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

tAm29f040::tAm29f040(void) {
  // Data bus
  DDR_DATA_LO = 0; // Lower
	PORT_DATA_LO = 0;
  DDR_DATA_HI = 0; // Upper
	PORT_DATA_HI = 0;

  // Address bus
  DDR_ADDR_LO = 0xFF; // Lower
  DDR_ADDR_MID = 0xFF; // Higher
  DDR_ADDR_HI = _BV(0) | _BV(1) | _BV(3); // Highest

  // Control bits
	DDR_NE |= _BV(PIN_NE);
	DDR_NWE |= _BV(PIN_NWE);
	DDR_NOE |= _BV(PIN_NOE);
}

bool tAm29f040::writeData(uint8_t ubDepth, uint32_t ulAddr, uint32_t ulValue) {
	if(ubDepth != 1) {
		return false;
	}
	writeCycle(0x555, 0xAA);
	writeCycle(0x2AA, 0x55);
	writeCycle(0x555, 0xA0);
	writeCycle(ulAddr, ulValue);

	// Verify
	uint32_t ulVerify;
	do {
		readData(1, ulAddr, ulVerify);
		Serial.print("Byte: ");
		Serial.println(ulVerify);
		if((ulVerify & _BV(7)) == (ulValue & _BV(7))) {
			return true;
		}
	} while(ulVerify & _BV(7));

	readData(1, ulAddr, ulVerify);
	if((ulVerify & _BV(7)) == (ulValue & _BV(7))) {
		return true;
	}
	return false;
}

bool tAm29f040::readData(uint8_t ubDepth, uint32_t ulAddr, uint32_t &ulResult) {
	if(ubDepth != 1) {
		return false;
	}
  // Set address lines
  PORT_ADDR_LO = ulAddr & 0xFF;
  PORT_ADDR_MID = (ulAddr >> 8) & 0xFF;
  PORT_ADDR_HI = (ulAddr >> 16) & 0b111;

	// Set to input without pullup
	PORT_DATA_LO = 0;
  PORT_DATA_HI = 0;
	DDR_DATA_HI = 0;
	DDR_DATA_LO = 0;

  // Set control lines lo
	PORT_NWE |= _BV(PIN_NWE);
	PORT_NE &= ~ _BV(PIN_NE);
	PORT_NOE &= ~ _BV(PIN_NOE);
  // delayMicroseconds(1);

  // Read from data lines
  ulResult = (PIN_DATA_HI << 8) | PIN_DATA_LO;

  // Set control lines hi
	PORT_NE |= _BV(PIN_NE);
	PORT_NOE |= _BV(PIN_NOE);

	return true;
}

void tAm29f040::relax(void) {
	// Set address lines low
	PORT_ADDR_LO = 0;
	PORT_ADDR_MID = 0;
	PORT_ADDR_HI = 0;

	// Set data  lines to input without pullup
	PORT_DATA_LO = 0;
  PORT_DATA_HI = 0;
	DDR_DATA_HI = 0;
	DDR_DATA_LO = 0;

  // Set control lines hi
	PORT_NE |= _BV(PIN_NE);
	PORT_NOE |= _BV(PIN_NOE);
	PORT_NWE |= _BV(PIN_NOE);
}

void tAm29f040::writeCycle(uint32_t ulAddr, uint8_t ubData) {
	PORT_ADDR_LO = ulAddr & 0xFF;
  PORT_ADDR_MID = (ulAddr >> 8) & 0xFF;
  PORT_ADDR_HI |= (ulAddr >> 16) & 0b111;

	// Output
	DDR_DATA_HI = 0xFF;
	DDR_DATA_LO = 0xFF;

	PORT_DATA_LO = ubData & 0xFF;
  PORT_DATA_HI = (ubData >> 8) & 0xFF;

  // Set control lines lo - address latches on falling edge
	PORT_NOE |= _BV(PIN_NOE);
	PORT_NE &= ~ _BV(PIN_NE);
	PORT_NWE &= ~ _BV(PIN_NWE);
	// delayMicroseconds(1);

	// Set control lines hi - data latches on rising edge
	PORT_NE |= _BV(PIN_NE);
	PORT_NWE |= _BV(PIN_NWE);

	// delayMicroseconds(1);
	PORT_ADDR_HI &= ~0b111;
}

bool tAm29f040::waitToggle(void) {
	constexpr uint8_t ubToggleMask = _BV(2) | _BV(6);
	uint32_t ulStatusOld, ulStatus;
	readData(1, 0, ulStatusOld);
	do {
		readData(1, 0, ulStatus);
		if((ulStatusOld & ubToggleMask) == (ulStatus & ubToggleMask)) {
			return true;
		}
		ulStatusOld = ulStatus;
	} while(!(ulStatus & _BV(5)));
	readData(1, 0, ulStatusOld);
	readData(1, 0, ulStatus);
	if((ulStatusOld & ubToggleMask) == (ulStatus & ubToggleMask)) {
		return true;
	}
	return false;
}

bool tAm29f040::eraseAll(void) {
	writeCycle(0x555, 0xAA);
	writeCycle(0x2AA, 0x55);
	writeCycle(0x555, 0x80);
	writeCycle(0x555, 0xAA);
	writeCycle(0x2AA, 0x55);
	writeCycle(0x555, 0x10);

	return waitToggle();
}
