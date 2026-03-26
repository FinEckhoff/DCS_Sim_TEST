#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#define BIT_US 104000
#define SR_CLK  PB2
#define SR_LOAD PB1


static void pulse_clock(void)
{
    PORTB &= ~(1 << SR_CLK);
    _delay_us(BIT_US/2);

    PORTB |= (1 << SR_CLK);
    _delay_us(BIT_US/2);
}

static void load_inputs(void)
{
    PORTB &= ~(1 << SR_LOAD);  // active low
    _delay_us(BIT_US);

    PORTB |= (1 << SR_LOAD);
    
}

static uint8_t read(void)
{
    

    load_inputs();

    for (uint8_t i = 0; i < 8; i++)
    {
        

        pulse_clock();
    }
    
    PORTB &= ~(1 << SR_CLK);
    return 0;
}

int main(void)
{
    DDRB |= (1 << SR_CLK) | (1 << SR_LOAD);   // outputs
                   

    PORTB &= ~(1 << SR_CLK);                  // idle low
    PORTB |= (1 << SR_LOAD);                  // inactive high

    while (1)
    {
        volatile uint8_t value = read();
       
        _delay_us(8*BIT_US);
    }
}