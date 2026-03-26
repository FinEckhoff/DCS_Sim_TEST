#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#define BIT_US 104000



void send_byte(uint8_t data)
{
    // start bit (LOW)
    PORTB &= ~(1 << PB0);
    _delay_us(BIT_US);

    // 8 data bits (LSB first)
    for (uint8_t i = 0; i < 8; i++)
    {
        if (data & 1)
            PORTB |= (1 << PB0);
        else
            PORTB &= ~(1 << PB0);

        _delay_us(BIT_US);
        data >>= 1;
    }

    // stop bit (HIGH)
    PORTB |= (1 << PB0);
    _delay_us(BIT_US);
}


int main(void)
{
    DDRB |= (1 << PB0);   // output
    PORTB |= (1 << PB0);  // idle HIGH

    while (1)
    {
        send_byte(0x55);
        
        _delay_ms(3000);
    }
}

