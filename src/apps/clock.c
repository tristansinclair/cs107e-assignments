#include "gpio.h"
#include "timer.h"

// 0 - 0b00111111
// 1 - 0b00000110
// 2 - 0b01011011
// 3 - 0b01001111
// 4 - 0b01100110
// 5 - 0b01101101
// 6 - 0b01111101
// 7 - 0b00000111
// 8 - 0b01111111
// 9 - 0b01100111
// A - 0b01110111
// B - 0b01111100
// C - 0b00111001
// D - 0b01011110
// E - 0b01111001

// struct Clock_Data
// {
//     int data[16];
// };

// struct Clock_Data;

// int zero = 0b00111111;
// int one = 0b00000110;
// int two = 0b01011011;
// int three = 0b01001111;
// int four = 0b01100110;
// int five = 0b01101101;
// int six = 0b01111101;
// int seven = 0b00000111;
// int eight = 0b01111111;
// int nine = 0b01100111;
// int A = 0b01110111;
// int B = 0b01111100;
// int C = 0b00111001;
// int D = 0b01011110;
// int E = 0b01111001;

int number_data[] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111,
                     0b01111111, 0b01100111, 0b01110111, 0b01111100, 0b00111001, 0b01011110, 0b01111001};

// Clock_Data[0] = zero;
// Clock_Data[1] = one;
// Clock_Data[2] = two;
// Clock_Data[3] = three;
// Clock_Data[4] = four;

void display_on(int num)
{
    gpio_set_output(num + 10);
    gpio_write(num + 10, 1);
}

void segment_on(int n)
{
    gpio_set_output(n + 20);
    gpio_write(n + 20, 1);
}

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
    display_on(display);
}

void off(void)
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

void display_time(int seconds, int minutes)
{
    for (int i = 200; i != 0; i--)
    {
        number_on(0b00000110, 0);
        timer_delay_us(2500);
        off();
        number_on(0b01011011, 1);
        timer_delay_us(2500);
        off();
    }
}

// void start_timer(int seconds, int minutes)
// {

//     while (1)
//     {
//         if (seconds == 60)
//         {
//             seconds = 0;
//             minutes++;
//         }

//         display_time()

//             display_time(seconds, minutes);

//         seconds++;
//     }
// }

void main(void)
{
    int seconds = 0;
    int minutes = 0;
    bool start_stop = false;

    while (start_stop = false) {
        
    }


    while (1)
    {
        if (seconds == 60)
        {
            minutes++;
            seconds = 0;
        }

        for (int i = 100; i != 0; i--)
        {
            number_on(number_data[seconds % 10], 3);
            timer_delay_us(2500);
            off();
            number_on(number_data[seconds / 10], 2);
            timer_delay_us(2500);
            off();
            number_on(number_data[minutes % 10], 1);
            timer_delay_us(2500);
            off();
            number_on(number_data[minutes / 10], 0);
            timer_delay_us(2500);
            off();
        }
        seconds++;
    }
}