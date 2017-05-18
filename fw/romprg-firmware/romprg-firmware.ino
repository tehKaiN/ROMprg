
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
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

uint16_t eeReadWord(uint32_t addr) {
  // Set WORD mode
  PORTB &= ~_BV(PB2);
  
  // Set address lines
  PORTL = addr & 0xFF;
  PORTK = (addr >> 8) & 0xFF;
  PORTB = (addr >> 16) & 0b11;

  // Set control lines lo
  PORTG &= ~(_BV(PG0) | _BV(PG2));
  delay(10);

  // Read from data lines
  uint16_t ret;
  ret = (PINC << 8) | PINA;

  // Set control lines hi
  PORTG |= _BV(PG0) | _BV(PG2);
  
  return ret;
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(1000);
  int i;
  uint16_t w;
  char bfr[255];
  for(i = 0; i != 100; ++i) {
    w = eeReadWord(i);
    w = (w << 8) | (w >> 8);
    Serial.write((char*)&w, 2);
  }
  delay(200);
}
