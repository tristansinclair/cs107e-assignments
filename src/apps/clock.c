/*
 * clock.c
 *
 * C for Raspberry Pi 1A+
 * Controls 4-digit 7-segment display
 * GPIO Pins 10-13 digit control, GPIO Pins 20-27 segment control
 * GPIO Pin 2 - Button input
 * idle - starting point, nothing is happening yet
 * count - hit the red button once, start the timer
 * stop - hti the red button again to stop the timer
 * reset - hit once more to return to idle
 * 
 * written by: Tristan Sinclair 10/5/20
 * for: CS107E
 */


#include "gpio.h"
#include "timer.h"

int number_data[] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111,
                     0b01111111, 0b01100111, 0b01110111, 0b01111100, 0b00111001, 0b01011110, 0b01111001};

/* segment_on - turns on 1 segments of a digit */
void segment_on(int n)
{
    gpio_set_output(n + 20);
    gpio_write(n + 20, 1);
}

/* digit_on - turns on individual digit */
void digit_on(int num)
{
    gpio_set_output(num + 10);
    gpio_write(num + 10, 1);
}

/* number_on - used 8bit binary code to turn on corresponding display segments */
void number_on(int n, int display)
{
    int counter = 0;
    while (n > 0)
    {
        // If current bit is 1
        if (n & 1)
        {
            segment_on(counter);
        }
        counter++;
        n = n >> 1;
    }
    digit_on(display);
}

/* number_off - shuts down the entire display - to use for refresh rate */
void number_off(void)
{
    gpio_write(10, 0);
    gpio_write(11, 0);
    gpio_write(12, 0);
    gpio_write(13, 0);
    gpio_write(20, 0);
    gpio_write(21, 0);
    gpio_write(22, 0);
    gpio_write(23, 0);
    gpio_write(24, 0);
    gpio_write(25, 0);
    gpio_write(26, 0);
    gpio_write(27, 0);
}

/* button_control - controls button: once button is *released* after press, control is iterated */
void button_control(int *button, int *control)
{
    if (gpio_read(2) == 0) // if button is pressed
    {
        *button = 1;
    }
    if (*button == 1 && gpio_read(2) == 1) // if button is released after being pressed
    {
        (*control)++;
        *button = 0;
        return;
    }
}

/* display_time - displays minutes and seconds */
void display_time(int *seconds, int *minutes, int *control, int *button)
{
    button_control(button, control); // checks for button press

    if (*seconds == 60) // second to minute rollover
    {
        (*minutes)++;
        *seconds = 0;
    }

    // diplays one digit at a time
    number_on(number_data[*seconds % 10], 3);
    timer_delay_us(2500);
    number_off();
    number_on(number_data[*seconds / 10], 2);
    timer_delay_us(2500);
    number_off();
    number_on(number_data[*minutes % 10], 1);
    timer_delay_us(2500);
    number_off();
    number_on(number_data[*minutes / 10], 0);
    timer_delay_us(2500);
    number_off();
}

/* start_timer - used to run the timer */
void start_timer(int *seconds, int *minutes, int *control, int *button)
{
    (*seconds)++; // adds a second every iteration

    for (int i = 100; i != 0; i--) // 2500 microseconds x 4 x 100 == 1s
    {
        display_time(seconds, minutes, control, button); // displays current time
    }
}

/* reset - used to reset the timer back to idle and 0s and 0mins */
void reset(int *seconds, int *minutes, int *control)
{
    *seconds = -1; // fixes slow stop if seconds starts at 0
    *minutes = 0;
    *control = 0;
}

/* idle - idle screen w/ no numbers displayed */
void idle(void)
{
    segment_on(6);
    digit_on(0);
    digit_on(1);
    digit_on(2);
    digit_on(3);
}

void main(void)
{
    int seconds = -1; // fixes slow stop if seconds starts at 0
    int minutes = 0;
    int control = 0; // controls what state we are in: idle, counting, stop, reset
    int button = 0;  // used for preventing problems w/ button sensing

    while (1)
    {
        while (control == 0) // idle, ready to start from button on GPIO 2
        {
            idle();
            button_control(&button, &control);
        }
        while (control == 1) // timer running
        {
            start_timer(&seconds, &minutes, &control, &button); // adds a second every iteration
        }
        while (control == 2) // stops the timer and diplays the time (like a stop watch)
        {
            display_time(&seconds, &minutes, &control, &button);
        }
        if (control == 3) // resets the timer back to idle
        {
            reset(&seconds, &minutes, &control);
        }
    }
}