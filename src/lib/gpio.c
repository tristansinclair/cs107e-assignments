#include "gpio.h"

struct gpio
{
    unsigned int FSEL[6];
    unsigned int reservedA;
    unsigned int SET[2];
    unsigned int reservedB;
    unsigned int CLR[2];
    unsigned int reservedC;
    unsigned int LEV[2];
};

volatile struct gpio *gpio = (struct gpio *)0x20200000;

void gpio_init(void)
{
}

void gpio_set_function(unsigned int pin, unsigned int function)
{
    /* 
    * Look at pin number and see which GPIO FSEL we need to configure
    * Go to that FSEL
    * Find location in FSEL: location = ((pin % 10) * 3)
    * Send the function over to that location
    */

    unsigned int register_num = (pin / 10);                // which GPIO config register do we need?
    unsigned int register_data = gpio->FSEL[register_num]; // copies register data to be modified

    unsigned int bit_shift = (pin % 10) * 3; // shift to get to desired GPIO config
    unsigned int clear = ~(7 << bit_shift);  // makes sequence like 111000111... for clearing GPIO config

    register_data &= clear;                   // clears the 3 bits of the function
    register_data |= (function << bit_shift); // Or's the bits into the register

    gpio->FSEL[register_num] = register_data; // Sends the updated register to the real register address
}

unsigned int gpio_get_function(unsigned int pin)
{
    /* 
    * Take pin # and go to correct GPIO FSEL
    * location = ((pin % 10) * 3)
    * return the 3 bits at the location
    * mask and shift?
    * (value & 7) >> 1
    */
    if (pin > 54 || pin < 0)
    {
        return GPIO_INVALID_REQUEST;
    }

    unsigned int register_num = (pin / 10);                // which GPIO config register do we need?
    unsigned int register_data = gpio->FSEL[register_num]; // copies register data to be inspected

    unsigned int bit_shift = (pin % 10) * 3;
    unsigned int bits_we_want = 7 << bit_shift;

    return ((register_data & bits_we_want) >> bit_shift); // returns & of register data and the bits_we_want
}

void gpio_set_input(unsigned int pin)
{
    gpio_set_function(pin, 0);
}

void gpio_set_output(unsigned int pin)
{
    gpio_set_function(pin, 1);
}

void gpio_write(unsigned int pin, unsigned int value)
{
    if (pin > 54 || pin < 0)
    {
        return;
    }

    unsigned int bit_location = pin % 32; // location of bit we want to manipulate
    unsigned int bit = 1 << bit_location; // creates: 000...1...000 w/ 1 at target bit

    unsigned int register_num = (pin / 32); // which GPIO config register do we need?

    if (value == 0)
    {
        gpio->CLR[register_num] = bit;
    }
    else if (value == 1)
    {
        gpio->SET[register_num] = bit;
    }
}

unsigned int gpio_read(unsigned int pin)
{
    if (pin > 54 || pin < 0)
    {
        return GPIO_INVALID_REQUEST;
    }

    unsigned int bit_location = pin % 32; // location of bit we want to inspect
    unsigned int bit = 1 << bit_location; // creates: 000...1...000 w/ 1 at target bit

    unsigned int register_num = (pin / 32); // which GPIO config register do we need?

    unsigned int register_data = gpio->LEV[register_num];

    if ((register_data & bit) > 0) {
        return 1;
    } else {
        return 0;
    }
}
