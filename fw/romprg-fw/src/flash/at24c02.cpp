#include "at24c02.hpp"
#include <Arduino.h>
#include <Wire.h>

// Status
#define PORT_STATUS PORTB
#define DDR_STATUS DDRB
#define P_STATUS PB7

// ROM Addr lines
#define PORT_ADDR PORTB
#define DDR_ADDR DDRB
#define P_A0 PB6
#define P_A1 PB5
#define P_A2 PB4

// Write protect pin
#define PORT_WP PORTH
#define DDR_WP DDRH
#define P_WP PH6

tAt24c02::tAt24c02(void)
{
	DDR_STATUS |= _BV(P_STATUS);
	DDR_WP |= _BV(P_WP);
	DDR_ADDR |= _BV(P_A0) | _BV(P_A1) | _BV(P_A2);

	// Clear write protect
	PORT_WP &= ~_BV(P_WP);
	// LED lo on idle
	PORT_STATUS &= ~_BV(P_STATUS);
	// Reset address lines
	PORT_ADDR &= ~(_BV(P_A0) | _BV(P_A1) | _BV(P_A2));

	Wire.begin();
}

bool tAt24c02::writeData(
	uint8_t ubDepth, uint32_t ulAddr, uint32_t ulValue
) {
	if(ubDepth != 1) {
		return false;
	}

	return false;
}

bool tAt24c02::readData(
	uint8_t ubDepth, uint32_t ulAddr, uint32_t &ulResult
) {
	if(ubDepth != 1) {
		return false;
	}

	// Set write protect
	PORT_WP |= _BV(P_WP);

	// LED hi on busy
	PORT_STATUS |= _BV(P_STATUS);

	uint8_t ubAddr = 0b1010000;

	Wire.beginTransmission(ubAddr);
	Wire.write(ulAddr);
	Wire.endTransmission(false);
	Wire.requestFrom(ubAddr, static_cast<uint8_t>(true));
	if(Wire.available()) {
		ulResult = Wire.read();
		return true;
	}

	return false;
}

void tAt24c02::relax(void) {
	// Clear write protect
	PORT_WP &= ~_BV(P_WP);
	// LED lo on idle
	PORT_STATUS &= ~_BV(P_STATUS);
}

bool tAt24c02::eraseAll(void) {
	return false;
}
