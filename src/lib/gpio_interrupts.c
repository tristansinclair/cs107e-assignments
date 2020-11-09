#include <stdbool.h>
#include "gpio_interrupts.h"
#include "assert.h"
#include "gpio.h"
#include "gpioextra.h"

/*
 * Notes:
 * Throw away scancode 
 * clear the event if infinite loop
 * 
 */

#define GPIO_PIN_COUNT 54

struct gpio
{
    unsigned int FSEL[6];
    unsigned int reservedA;
    unsigned int SET[2];
    unsigned int reservedB;
    unsigned int CLR[2];
    unsigned int reservedC;
    unsigned int LEV[2];
    unsigned int reservedD;
    unsigned int EDS[2];
};

volatile struct gpio *gpio = (struct gpio *)0x20200000; // Point to beginning of struct
static struct
{
    handler_fn_t fn;
} handlers[GPIO_PIN_COUNT];

/*
 * Module to configure GPIO interrupts for Raspberry Pi.
 * Because all of the GPIO pins share a small set of GPIO
 * interrupts, you need a level of indirectiom to be able
 * to handle interrupts for particular pins. This module
 * allows a client to register one handler for each
 * GPIO pin.
 *
 * Author: Philip Levis <pal@cs.stanford.edu>
 *
 * Last update:   May 2020
 */
bool gpio_default_handler(unsigned int pc)
{
    return false;
}

/*
 * `gpio_interrupts_init`
 *
 * Initialize the GPIO interrupt modules. The init function must be 
 * called before any calls to other functions in this module.
 * The init function configures gpio interrupts to a clean state.
 * This function registers a handler for GPIO interrupts, which it
 * uses to call handlers on a per-pin basis.
 *
 * Disables GPIO interrupts if they were previously enabled.
 * 
 */
void gpio_interrupts_init(void)
{
    // set every pin to default handler
    // set all pins to default handler
    gpio_interrupts_disable();

    // for (int i = 0; i < GPIO_PIN_COUNT; i++)
    // {
    //     handlers[i].fn = gpio_default_handler(0);
    // }

    // then register our own handler
}

/*
 * `gpio_interrupts_enable`
 *
 * Enables GPIO interrupts.
 */
void gpio_interrupts_enable(void)
{
    interrupts_enable_source(INTERRUPTS_GPIO3); // enable interrupts on all GPIO Pins
}

/*
 * `gpio_interrupts_disable`
 *
 * Disables GPIO interrupts.
 */
void gpio_interrupts_disable(void)
{
    interrupts_disable_source(INTERRUPTS_GPIO3); // enable interrupts on all GPIO Pins
}

extern unsigned int count_leading_zeroes(unsigned int val); // Defined in assembly

static int gpio_interrupts_get_next(void)
{
    unsigned int eds0_zeroes = count_leading_zeroes(gpio->EDS[0]);
    unsigned int eds1_zeroes = count_leading_zeroes(gpio->EDS[1]);

    if (eds0_zeroes != 32)
    {
        return 31 - eds0_zeroes;
    }
    else if (eds1_zeroes != 32)
    {
        return 31 - eds1_zeroes;
    }
    else
    {
        return INTERRUPTS_NONE;
    }
}

void gpio_interrupt_dispatch(unsigned int pc)
{
    int next_interrupt = gpio_interrupts_get_next();
    if (next_interrupt < GPIO_PIN_COUNT)
    {
        handlers[next_interrupt].fn(pc);
    }
}

/* 
 * `interrupts_attach_handler`
 *
 * Attach a handler function to a given GPIO pin. Each pin
 * source can have one handler: further dispatch should be invoked by
 * the handler itself. Whether or not a particular pin will
 * generate interrupts is specified by the events system,
 * defined in `gpioextra.h`. 
 *
 * Asserts if failed to install handler (e.g., the pin is invalid).
 * Pins are defined in `gpio.h`.
 */
handler_fn_t gpio_interrupts_register_handler(unsigned int pin, handler_fn_t fn)
{
    assert(GPIO_PIN_FIRST <= pin && pin <= GPIO_PIN_LAST);

    handler_fn_t old_handler = handlers[pin].fn;
    handlers[pin].fn = fn;
    return old_handler;
}
