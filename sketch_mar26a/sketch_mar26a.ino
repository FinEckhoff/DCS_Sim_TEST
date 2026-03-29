
/*
 * Incoming data format (8 bits total):
 *
 *   ABCD EFGH
 *
 */


#define BIT_US 104
#define BIT_A 6
#define RESPONSE_TIMEOUT_US BIT_US*500
typedef struct {
  const char* command;
  uint8_t lsb;    // lowest bit position
  uint8_t width;  // number of bits
} FieldDef;


typedef struct {
  uint8_t address;
  const FieldDef* fields;
  uint8_t count;

} MapEntry;


const FieldDef fields_air_con1[] = {
  { "PLT_TEMP_AUTO_MAN", 7, 1 },     // A
  { "PLT_CABIN_PRESS_DUMP", 6, 1 },  // B
  { "PLT_RAM_AIR", 5, 1 },           // C
  { "PLT_TEMP", 1, 4 }               // DEFG
};


const MapEntry lookupTable[] = {
  { 0x01, fields_air_con1, sizeof(fields_air_con1) / sizeof(fields_air_con1[0]) }
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



static uint8_t read_byte_timeout(uint8_t* out, uint32_t timeout_us) {
  uint8_t data = 0;
  uint32_t start = micros();

  // Wait for start bit: line goes LOW
  while (PIND & (1 << PD7)) {
    if ((uint32_t)(micros() - start) >= timeout_us) {
      return 0;   // timed out waiting for start bit
    }
  }

  // Verify it is still low in the middle of the start bit
  _delay_us(BIT_US / 2);
  if (PIND & (1 << PD7)) {
    return 0;   // glitch, not a real start bit
  }

  // Move to middle of first data bit
  _delay_us(BIT_US);

  // Read 8 data bits, LSB first
  for (uint8_t i = 0; i < 8; i++) {
    if (PIND & (1 << PD7)) {
      data |= (1 << i);
    }
    _delay_us(BIT_US);
  }

  // Stop bit should be HIGH
  if (!(PIND & (1 << PD7))) {
    return 0;   // framing error
  }

  *out = data;
  return 1;
}





void setup() {
  pinMode(7, INPUT);
  pinMode(6, OUTPUT);
  DDRD |= (1 << DDD6);
  Serial.begin(19200);
}


void loop() {

  for (uint8_t addr = 0; addr <= 0x0F; addr++) {



    PORTD &= ~(1 << 6);  //pin6 //send addr
    _delay_us(20 * BIT_US);
    PORTD |= (1 << 6);

    Serial.print("addr ");
    Serial.println(addr);
    uint8_t numOfBytes;
    if (!read_byte_timeout(&numOfBytes, RESPONSE_TIMEOUT_US)) {
      Serial.println("timeout waiting for byte count");
      continue;   // skip this address
    }
    if (numOfBytes > 16) {
      Serial.print("invalid byte count ");
      Serial.println(numOfBytes);
      continue;
    }

    for (uint8_t bytenr = 0; bytenr < numOfBytes; bytenr++) {
      uint8_t val;
      if (!read_byte_timeout(&val, RESPONSE_TIMEOUT_US)) {
        Serial.print("timeout at addr ");
        Serial.print(addr); 
        Serial.print(" byte ");
        Serial.println(bytenr);
        break;   // abort this slave, continue with next address
      }

      
      if (addr == 1) {
        Serial.print("VAL");
        Serial.print(" ");
        Serial.println(val);
        decodeAndPrint(addr, val);
        //delay(1000);
      }
    }
  }




  //_delay_us(10*BIT_US);
}
