#include "interrupts.h"
#include "keyboard.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "gpio_interrupts.h"
#include "assert.h"
#include "gpio.h"
#include "gpioextra.h"

void test_clock_events(void)
{
    printf("Type on your PS/2 keyboard to generate clock events. You've got 10 seconds, go!\n");
    timer_delay(10);
    printf("Time's up!\n");
}

/*
 * This function tests the behavior of the assign5 keyboard
 * implementation versus the new-improved assign7 version. If using the
 * assign5 implementation, a key press that arrives while the main program
 * is waiting in delay is simply dropped. Once you upgrade your
 * keyboard implementation to be interrupt-driven, those keys should
 * be queued up and can be read after delay finishes.
 */
void test_read_delay(void)
{
    while (1)
    {
        printf("Test program waiting for you to press a key (q to quit): ");
        uart_flush();
        char ch = keyboard_read_next();
        printf("\nRead: %c\n", ch);
        if (ch == 'q')
            break;
        printf("Test program will now pause for 1 second... ");
        uart_flush();
        timer_delay(1);
        printf("done.\n");
    }
    printf("\nGoodbye!\n");
}

extern unsigned int count_leading_zeroes(unsigned int val); // Defined in assembly

#define BUTTON GPIO_PIN20
#define BUTTON2 GPIO_PIN21

volatile static int counter = 0;
//volatile static rb_t *rb;

static void configure_buttons(void)
{
    gpio_set_input(BUTTON);
    gpio_set_pullup(BUTTON);
    gpio_enable_event_detection(BUTTON, GPIO_DETECT_RISING_EDGE);

    gpio_set_input(BUTTON2);
    gpio_set_pullup(BUTTON2);
    gpio_enable_event_detection(BUTTON2, GPIO_DETECT_RISING_EDGE);
}

static bool button_press(unsigned int pc)
{
    if (gpio_check_and_clear_event(BUTTON))
    {
        printf("times button pressed: %d\n", counter);
        //rb_enqueue(rb, counter);
        counter++;

        timer_delay(5);

        return true;
    }

    return false;
}

static bool button_press2(unsigned int pc)
{
    if (gpio_check_and_clear_event(BUTTON2))
    {
        printf("times button pressed: %d\n", counter);
        //rb_enqueue(rb, counter);
        counter++;
        return true;
    }

    return false;
}

static void test_gpio_interrupts(void)
{
    assert(count_leading_zeroes(0x0) == 32);
    assert(count_leading_zeroes(0xffffffff) == 0);
    assert(count_leading_zeroes(0x4) == 29);
    printf("count_leading_zeros tests success!\n");

    while (counter < 25)
    {
        
    }
}

void main(void)
{
    gpio_init();
    timer_init();
    uart_init();
    //keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    gpio_interrupts_init();
    gpio_interrupts_enable();
    gpio_interrupts_register_handler(BUTTON, button_press);
    gpio_interrupts_register_handler(BUTTON2, button_press2);

    configure_buttons();

    test_gpio_interrupts();

    //test_clock_events();  // wait 10 seconds for clock_edge handler to report clock edges
    //test_read_delay();  // what happens to keys typed while main program blocked in delay?
    uart_putchar(EOT);
}
