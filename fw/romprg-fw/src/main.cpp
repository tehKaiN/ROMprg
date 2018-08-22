#include <Arduino.h>

const uint8_t s_ubSerialBfrMaxLength = 150;
bool s_bSerialRxReady = false;
char s_szSerialBfr[s_ubSerialBfrMaxLength];
uint8_t s_ubSerialBfrLength = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(76800);
  pinMode(13, OUTPUT);
  // Usable ports:
  // PORTB - scattered on DIO & PWM region
  // PORTF, PORTK - analog in region
  // PORTA, PORTC - DIO region

  // Data bus
  DDRA = 0; // Lower
  DDRC = 0; // Upper

  // Address bus
  DDRL = 0xFF; // Lower
  DDRK = 0xFF; // Higher
  DDRB = _BV(PB0) | _BV(PB1); // Highest

  // Control bits
  DDRB |= _BV(PB2);           // ~BYTE
  DDRG = _BV(PG0) | _BV(PG2); // ~E, ~G

}

void eeLow(void) {
	// Set all lines low
	PORTL = 0;
	PORTK = 0;
	PORTB = 0;

	// Set control lines low
	PORTG &= ~(_BV(PG0) | _BV(PG2));
}

uint16_t eeReadWord(uint32_t addr) {
  // Set WORD mode
  PORTB &= ~_BV(PB2);

  // Set address lines
  PORTL = addr & 0xFF;
  PORTK = (addr >> 8) & 0xFF;
  PORTB = (addr >> 16) & 0b11;

  // Set control lines lo
  PORTG &= ~(_BV(PG0) | _BV(PG2));
  delayMicroseconds(1);

  // Read from data lines
  uint16_t uwRet;
  uwRet = (PINC << 8) | PINA;

  // Set control lines hi
  // PORTG |= _BV(PG0) | _BV(PG2);

  return uwRet;
}

uint8_t eeWriteVerify(uint32_t ulAddr, uint16_t uwVal) {
  // Set WORD mode
  PORTB &= ~_BV(PB2);

  // Set address lines
  PORTL = ulAddr & 0xFF;
  PORTK = (ulAddr >> 8) & 0xFF;
  PORTB = (ulAddr >> 16) & 0b11;

	// TODO Set /E line lo, /G line hi
	// TODO Wait for write

	// Set /E line hi, /G line lo
	PORTG |= _BV(PG0);
  PORTG &= ~ _BV(PG2);
  delayMicroseconds(1);

	while(1) {}

  // Read from data lines
  uint16_t uwRet;
  uwRet = (PINC << 8) | PINA;


  // Set control lines hi
  PORTG |= _BV(PG0) | _BV(PG2);
  return 0;
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
    uint16_t w;
    char bfr[255];
    sprintf(bfr, "start: %lu, cnt: %lu", ulStart, ulCnt);
    Serial.println(bfr);
    for(uint32_t i = ulStart; i < ulStart+ulCnt; ++i) {
      w = eeReadWord(i);
      w = (w << 8) | (w >> 8); // Endian swap
      Serial.write((char*)&w, 2);
    }
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
      uint16_t w = eeReadWord(i);
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
		eeWriteVerify(0, 0);
	}

  s_bSerialRxReady = false;
  s_ubSerialBfrLength = 0;
}

void loop() {
  serialProcessRx();
	eeLow();
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
