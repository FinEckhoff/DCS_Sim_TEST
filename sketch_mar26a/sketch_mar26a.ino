
/*
 * Incoming data format (8 bits total):
 *
 *   1ABC DEFG
 *
 */


#define BIT_US 104
#define BIT_A 6

typedef struct {
  const char* command;
  uint8_t lsb;     // lowest bit position
  uint8_t width;   // number of bits
} FieldDef;


typedef struct {
  uint8_t address;
  const FieldDef* fields;
  uint8_t count;

} MapEntry;


const FieldDef fields_air_con1[] = {
  { "PLT_TEMP_AUTO_MAN",    6, 1 },  // A
  { "PLT_CABIN_PRESS_DUMP", 5, 1 },  // B
  { "PLT_RAM_AIR",          4, 1 },  // C
  { "PLT_TEMP",             0, 4 }   // DEFG
};


const MapEntry lookupTable[] = {
  { 0x01, fields_air_con1,   sizeof(fields_air_con1) / sizeof(fields_air_con1[0]) }
};


static uint8_t extractBits(uint8_t value, uint8_t lsb, uint8_t width) {
    uint8_t mask = (1u << width) - 1u;
  return (value >> lsb) & mask;
  
}

const MapEntry* lookupEntry(uint8_t address) {
  for (uint8_t i = 0; i < sizeof(lookupTable) / sizeof(lookupTable[0]); i++) {
    if (lookupTable[i].address == address) {
      return &lookupTable[i];
    }
  }
  return 0;
}


static void decodeAndPrint(uint8_t address, uint8_t val) {
  const MapEntry* e = lookupEntry(address);
  if (!e) {
    return;
  }

  for (uint8_t i = 0; i < e->count; i++) {
    uint8_t fieldValue = extractBits(val, e->fields[i].lsb, e->fields[i].width);

    Serial.print(e->fields[i].command);
    Serial.print(" ");
    Serial.println(fieldValue);
  }
}





static uint8_t read_byte(void) {
  uint8_t data = 0;

  // Wait for start bit: line goes LOW
  while (PIND & (1 << PD7)) {}



  _delay_us(2.25 * BIT_US);
  // Move to middle of first data bit
  //_delay_us(BIT_US + (BIT_US / 2));

  // Read 8 data bits, LSB first
  for (uint8_t i = 0; i < 8; i++) {
    if (PIND & (1 << PD7)) {

      data |= (1 << i);
    }


    _delay_us(BIT_US);
  }

  // Optional: sample/skip stop bit
  _delay_us(BIT_US);

  return data;
}



void setup() {
  pinMode(7, INPUT);
  pinMode(6, OUTPUT);
  DDRD |= (1 << DDD6);
  Serial.begin(19200);
}


void loop() {

  for (uint8_t addr = 0; addr <= 0x0F; addr++) {



    PORTD &= ~(1 << 6);  //pin6
    _delay_us(20 * BIT_US);
    PORTD |= (1 << 6);

    uint8_t val = read_byte();



    const MapEntry* e = lookupEntry(0x01);

    if (addr == 1) {
        Serial.print("VAL");
    Serial.print(" ");
    Serial.println(val);
      decodeAndPrint(addr, val);
    }
  }
  //_delay_us(10*BIT_US);
  //delay(500);
}
