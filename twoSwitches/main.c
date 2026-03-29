#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#define READ_BIT_US 500
#define WRITE_BIT_US 104
#include <avr/interrupt.h>
#define BYTESTOSEND 1
volatile uint8_t triggered = 0;
uint8_t val = 0;
uint8_t valueChanged = 0;
ISR(PCINT0_vect)
{
    // check if PB3 is HIGH (rising edge detection)
    if (PINB & (1 << PB3) )
    {
        PORTB |= (1<<PB4);
        triggered = 1;
    }
}


static uint8_t read_byte(void)
{
    uint8_t data = 0;

    // Wait for start bit: line goes LOW
    while (PINB & (1 << PB0)) { }
           


    _delay_us(1.5*READ_BIT_US);
    // Move to middle of first data bit
    //_delay_us(BIT_US + (BIT_US / 2));

    // Read 8 data bits, LSB first
    for (uint8_t i = 0; i < 8; i++)
    {
    if (PINB & (1 << PB0)){
        
        data |= (1 << i);    
    } 
    
        

    _delay_us(READ_BIT_US);
    }

    // Optional: sample/skip stop bit
    _delay_us(READ_BIT_US);

    return data;
}



void send_byte(uint8_t data)
{
    // start bit (LOW)
    PORTB &= ~(1 << PB2);
    _delay_us(WRITE_BIT_US);

    // 8 data bits (LSB first)
    for (uint8_t i = 0; i < 8; i++)
    {
        if (data & 1)
            PORTB |= (1 << PB2);
        else
            PORTB &= ~(1 << PB2);

        _delay_us(WRITE_BIT_US);
        data >>= 1;
    }

    // stop bit (HIGH)
    PORTB |= (1 << PB2);
    _delay_us(WRITE_BIT_US);
}


void send_init(){
    send_byte(BYTESTOSEND);
    _delay_us(WRITE_BIT_US);
}





int main(void)
{
    DDRB &= ~(1 << PB3); 
    DDRB &= ~(1 << PB0);  // input
    PORTB |= (1 << PB0);  // enable pull-up (important!)

    DDRB |= (1 << PB1);   // debug LED output
    DDRB |= (1 << PB2); 
    DDRB |= (1 << PB4); 


    sei();
    GIMSK |= (1<<PCIE);
    PCMSK |=(1<<PCINT3);
    while (1)
    {
        uint8_t newval = read_byte();
        if(newval != val){
            val = newval;
            valueChanged = 1;
        }
        
        if(triggered){
            send_init();
            send_byte(val);
            triggered=0;
        }

        
        
        // simple debug: toggle LED if nonzero
        
    }
}
