#include "assert_internal.h"
#include "gpio.h"
#include "timer.h"

void test_gpio_set_get_function(void)
{
    gpio_init();

    // Test get pin function (pin2 defaults to input)
    assert(gpio_get_function(GPIO_PIN2) == GPIO_FUNC_INPUT);

    // Test set pin to output
    gpio_set_output(GPIO_PIN2);

    // Test get pin function after setting
    assert(gpio_get_function(GPIO_PIN2) == GPIO_FUNC_OUTPUT);

    // Test Input
    gpio_set_function(2, GPIO_FUNC_INPUT);
    gpio_set_function(3, GPIO_FUNC_INPUT);
    assert(gpio_get_function(GPIO_PIN2) == GPIO_FUNC_INPUT);
    assert(gpio_get_function(GPIO_PIN3) == GPIO_FUNC_INPUT);

    // Test each register w/ numbers
    gpio_set_function(1, 1);
    gpio_set_function(11, 1);
    gpio_set_function(21, 1);
    gpio_set_function(31, 1);
    gpio_set_function(41, 1);
    gpio_set_function(51, 1);
    assert(gpio_get_function(1) == 1);
    assert(gpio_get_function(11) == 1);
    assert(gpio_get_function(21) == 1);
    assert(gpio_get_function(31) == 1);
    assert(gpio_get_function(41) == 1);
    assert(gpio_get_function(51) == 1);

    // Now change them to input
    gpio_set_function(1, 0);
    gpio_set_function(11, 0);
    gpio_set_function(21, 0);
    gpio_set_function(31, 0);
    gpio_set_function(41, 0);
    gpio_set_function(51, 0);
    assert(gpio_get_function(1) == 0);
    assert(gpio_get_function(11) == 0);
    assert(gpio_get_function(21) == 0);
    assert(gpio_get_function(31) == 0);
    assert(gpio_get_function(41) == 0);
    assert(gpio_get_function(51) == 0);

    // Test edge case (GPIO 0 and 52)
    gpio_set_function(0, 1);
    gpio_set_function(52, 1);
    assert(gpio_get_function(0) == 1);
    assert(gpio_get_function(52) == 1);

    // Test alternate function
    gpio_set_function(21, 7);
    assert(gpio_get_function(21) == 7);

    // Multiple GPIO pins in same register
    gpio_set_function(31, 1);
    gpio_set_function(32, 1);
    gpio_set_function(33, 1);
    assert(gpio_get_function(31) == 1);
    assert(gpio_get_function(32) == 1);
    assert(gpio_get_function(33) == 1);

    // Improper GPIO pin
    assert(gpio_get_function(55) == GPIO_INVALID_REQUEST);
    assert(gpio_get_function(-2) == GPIO_INVALID_REQUEST);

}

void test_gpio_read_write(void)
{
    gpio_init();
    gpio_set_function(GPIO_PIN20, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN2, GPIO_FUNC_OUTPUT);
    gpio_set_function(GPIO_PIN3, GPIO_FUNC_OUTPUT);

    // Test gpio_write low, then gpio_read
    gpio_write(GPIO_PIN20, 0);
    assert(gpio_read(GPIO_PIN20) == 0);

    // Test gpio_write high, then gpio_read
    gpio_write(GPIO_PIN20, 1);
    assert(gpio_read(GPIO_PIN20) == 1);

    // Test gpio write on off
    gpio_write(GPIO_PIN2, 1);
    gpio_write(GPIO_PIN3, 1);
    assert(gpio_read(GPIO_PIN3) == 1);
    gpio_write(GPIO_PIN3, 0);
    assert(gpio_read(GPIO_PIN2) == 1);
    assert(gpio_read(GPIO_PIN3) == 0);

    // Test invalid GPIO pin
    assert(gpio_read(55) == GPIO_INVALID_REQUEST);
    assert(gpio_read(-2) == GPIO_INVALID_REQUEST);
}

void test_timer(void)
{
    timer_init();

    // Test timer tick count incrementing
    unsigned int start = timer_get_ticks();
    for (int i = 0; i < 10; i++)
    { /* Spin */
    }
    unsigned int finish = timer_get_ticks();
    assert(finish > start);

    // Test timer delay usecs
    int usecs = 100;
    start = timer_get_ticks();
    timer_delay_us(usecs);
    finish = timer_get_ticks();
    assert(finish >= start + usecs);

    // Test timer delay millisecs
    int msecs = 100;
    start = timer_get_ticks();
    timer_delay_ms(msecs);
    finish = timer_get_ticks();
    assert(finish >= start + msecs);

    // Test timer delay 1 second
    int sec = 1;
    start = timer_get_ticks();
    timer_delay(sec);
    finish = timer_get_ticks();
    assert(finish >= start + sec);

    // Test timer delay 2 seconds
    int secs = 2;
    start = timer_get_ticks();
    timer_delay(secs);
    finish = timer_get_ticks();
    assert(finish >= start + secs);
}

// Uncomment each call below when you have implemented the functions
// and are ready to test them

void main(void)
{
    test_gpio_set_get_function();
    test_gpio_read_write();
    test_timer();
}
