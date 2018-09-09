#include <Arduino.h>
#include "am29f040.hpp"

const uint8_t s_ubSerialBfrMaxLength = 150;
bool s_bSerialRxReady = false;
char s_szSerialBfr[s_ubSerialBfrMaxLength];
uint8_t s_ubSerialBfrLength = 0;
tFlash *s_pFlash = nullptr;

// http://osoyoo.com/wp-content/uploads/2017/08/arduino_mega_2560_pinout.png

void setup() {
  // put your setup code here, to run once:
  Serial.begin(76800);
  pinMode(13, OUTPUT);
  // Usable ports:
  // PORTB - scattered on DIO & PWM region
  // PORTF, PORTK - analog in region
  // PORTA, PORTC - DIO region
}

static void serialProcessRx() {
  if(!s_bSerialRxReady) {
    return;
	}

  char szCmd[10];
  sscanf(s_szSerialBfr, "%s", szCmd);

	if(!strcmp(szCmd, "chip")) {
		char szChip[20];
		sscanf(s_szSerialBfr, "%s %s", szCmd, szChip);
		if(s_pFlash != nullptr) {
			delete s_pFlash;
		}
		s_pFlash = tFlash::fromString(szChip);
		if(s_pFlash == nullptr) {
			Serial.print("ERR: Unsupported chip: '");
			Serial.print(szChip);
			Serial.println("'");
		}
		else {
			Serial.println("OK");
		}
	}
	else if(s_pFlash != nullptr) {
		// You are like a little baby, let me show you
		Serial.println(s_pFlash->processCommand(s_szSerialBfr) ? "SUCC" : "FAIL");
	}
	else {
		Serial.println("ERR: No chip selected!");
	}
	s_bSerialRxReady = false;
	s_ubSerialBfrLength = 0;
}

void loop() {
  serialProcessRx();
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
