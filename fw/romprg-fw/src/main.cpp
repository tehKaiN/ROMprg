#include <Arduino.h>

const uint8_t s_ubSerialBfrMaxLength = 150;
bool s_bSerialRxReady = false;
char s_szSerialBfr[s_ubSerialBfrMaxLength];
uint8_t s_ubSerialBfrLength = 0;

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

// http://osoyoo.com/wp-content/uploads/2017/08/arduino_mega_2560_pinout.png

void setup() {
  // put your setup code here, to run once:
  Serial.begin(76800);
  pinMode(13, OUTPUT);
  // Usable ports:
  // PORTB - scattered on DIO & PWM region
  // PORTF, PORTK - analog in region
  // PORTA, PORTC - DIO region

  // Data bus
  DDR_DATA_LO = 0; // Lower
  DDR_DATA_HI = 0; // Upper

  // Address bus
  DDR_ADDR_LO = 0xFF; // Lower
  DDR_ADDR_MID = 0xFF; // Higher
  DDR_ADDR_HI = _BV(0) | _BV(1) | _BV(3); // Highest

  // Control bits
	DDR_NE |= _BV(PIN_NE);
	DDR_NWE |= _BV(PIN_NWE);
	DDR_NOE |= _BV(PIN_NOE);
}

void eeRelax(void) {
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

void eeWriteCycle(uint32_t addr, uint16_t data) {
	PORT_ADDR_LO = addr & 0xFF;
  PORT_ADDR_MID = (addr >> 8) & 0xFF;
  PORT_ADDR_HI |= (addr >> 16) & 0b111;

	// Output
	DDR_DATA_HI = 0xFF;
	DDR_DATA_LO = 0xFF;

	PORT_DATA_LO = data & 0xFF;
  PORT_DATA_HI = (data >> 8) & 0xFF;

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

uint16_t eeReadWord(uint32_t addr) {
  // Set address lines
  PORT_ADDR_LO = addr & 0xFF;
  PORT_ADDR_MID = (addr >> 8) & 0xFF;
  PORT_ADDR_HI = (addr >> 16) & 0b111;

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
  uint16_t uwRead;
  uwRead = (PIN_DATA_HI << 8) | PIN_DATA_LO;

  // Set control lines hi
	PORT_NE |= _BV(PIN_NE);
	PORT_NOE |= _BV(PIN_NOE);

  return uwRead;
}

uint8_t eeToggleCheck(void) {
	constexpr uint8_t ubToggleMask = _BV(2) | _BV(6);
	uint16_t uwStatusOld, uwStatus;
	do {
		uwStatus = eeReadWord(0);
		if((uwStatusOld & ubToggleMask) == (uwStatus & ubToggleMask)) {
			return 1;
		}
		uwStatusOld = uwStatus;
	} while(!(uwStatus & _BV(5)));
	uwStatusOld = eeReadWord(0);
	uwStatus = eeReadWord(0);
	if((uwStatusOld & ubToggleMask) == (uwStatus & ubToggleMask)) {
		return 1;
	}
	return 0;
}

uint8_t eeWriteVerify(uint32_t ulAddr, uint16_t uwVal) {
	eeWriteCycle(0x555, 0xAA);
	eeWriteCycle(0x2AA, 0x55);
	eeWriteCycle(0x555, 0xA0);
	eeWriteCycle(ulAddr, uwVal);

	// Verify
	uint16_t uwVerify;
	do {
		uwVerify = eeReadWord(ulAddr);
		if((uwVerify & _BV(7)) == (uwVal & _BV(7))) {
			return 1;
		}
	} while(uwVerify & _BV(7));

	uwVerify = eeReadWord(ulAddr);
	if((uwVerify & _BV(7)) == (uwVal & _BV(7))) {
		return 1;
	}
	return 0;
}

uint8_t eeErase(void) {
	eeWriteCycle(0x555, 0xAA);
	eeWriteCycle(0x2AA, 0x55);
	eeWriteCycle(0x555, 0x80);
	eeWriteCycle(0x555, 0xAA);
	eeWriteCycle(0x2AA, 0x55);
	eeWriteCycle(0x555, 0x10);

	return eeToggleCheck();
}

static uint8_t onesInWord(uint16_t w) {
	uint8_t cnt = 0;
	for(uint8_t i = 0; i < 16; ++i) {
		if(w & 1) {
			++cnt;
		}
		w >>= 1;
	}
	return cnt;
}

static void serialProcessRx() {
  if(!s_bSerialRxReady)
    return;

  uint32_t ulStart, ulCnt;
  char szCmd[10];
  sscanf(s_szSerialBfr, "%s %lu %lu", szCmd, &ulStart, &ulCnt);
  if(!strcmp(szCmd, "read")) {
    // Read mode
    uint8_t w;
    char bfr[255];
    sprintf(bfr, "start: %lu, cnt: %lu", ulStart, ulCnt);
    Serial.println(bfr);
    for(uint32_t i = ulStart; i < ulStart+ulCnt; ++i) {
      w = eeReadWord(i);
      // w = (w << 8) | (w >> 8); // Endian swap
			sprintf(bfr, "%02x ", w);
			Serial.print(bfr);
      // Serial.write((char*)&w, 1);
    }
		Serial.println("");
  }
  else if(!strcmp(szCmd, "mask")) {
    // Test mode
		char bfr[255];
		sprintf(bfr, "mask: %lu, cnt: %lu", ulStart, ulCnt);
		Serial.println(bfr);
		uint16_t mask = 0;
    for(uint32_t i = ulStart; i < ulStart+ulCnt; ++i) {
      uint16_t w = eeReadWord(i);
      w = (w << 8) | (w >> 8); // Endian swap
			mask |= w;
		}
		Serial.print("Mask: ");
		Serial.write((char*)&mask, 2);
		Serial.print("\r\n");
  }
  else if(!strcmp(szCmd, "chkerase")) {
    // Erase check mode
		char bfr[255];
		sprintf(bfr, "erase chk: %lu, cnt: %lu", ulStart, ulCnt);
		Serial.println(bfr);
		uint32_t ulOnesCnt = 0;
    for(uint32_t i = ulStart; i < ulStart+ulCnt; ++i) {
      uint16_t w = eeReadWord(i) | 0xFF00;
      ulOnesCnt += onesInWord(w);
		}
		uint32_t ulBitCnt = (ulCnt*16);
		uint32_t ulZerosCnt = ulBitCnt - ulOnesCnt;
		double percentage = (ulZerosCnt *100.0)/ulBitCnt;
		char szDbl[10];
		dtostrf(percentage, 0, 2, szDbl);
		sprintf(bfr, "Zeros: %lu / %lu (%s%%)", ulZerosCnt, ulBitCnt, szDbl);
		Serial.println(bfr);
  }
	else if(!strcmp(szCmd, "write")) {
		Serial.println("writing");
		eeWriteVerify(ulStart, ulCnt);
		Serial.println("done");
	}
	else if(!strcmp(szCmd, "erase")) {
		Serial.println("erasing");
		eeErase();
		Serial.println("done");
	}
  s_bSerialRxReady = false;
  s_ubSerialBfrLength = 0;
}

void loop() {
  serialProcessRx();
	eeRelax();
  delay(200);
}

void serialEvent() {
  // Read message from PC app
  while(Serial.available() && !s_bSerialRxReady) {
    char c = (char)Serial.read();
    s_szSerialBfr[s_ubSerialBfrLength] = c;
    ++s_ubSerialBfrLength;

    if(c == '\n') {
      s_bSerialRxReady = true;
      s_szSerialBfr[s_ubSerialBfrLength] = '\0';
      ++s_ubSerialBfrLength;
    }
  }
}
