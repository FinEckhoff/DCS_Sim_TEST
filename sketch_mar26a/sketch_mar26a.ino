void setup() {
  pinMode(7,INPUT);
  pinMode(6,OUTPUT);

  Serial.begin(9600);
}

#define BIT_US 104000

static uint8_t read_byte(void)
{
    uint8_t data = 0;

    // Wait for start bit: line goes LOW
    while (PIND & (1 << PD7)) { }
           


    _delay_us(2.5*BIT_US);
    // Move to middle of first data bit
    //_delay_us(BIT_US + (BIT_US / 2));

    // Read 8 data bits, LSB first
    for (uint8_t i = 0; i < 8; i++)
    {
    if (PIND & (1 << PD7)){
       
        data |= (1 << i);    
    } 
    

    _delay_us(BIT_US);
    }

    // Optional: sample/skip stop bit
    _delay_us(BIT_US);

    return data;
}



void loop() {

  digitalWrite(6,0);
  delay(200);
  digitalWrite(6,1);

  uint8_t val = read_byte();
  Serial.println(val);

  delay(5000);


  //Serial.println(digitalRead(7));
  //delay(10);

}
