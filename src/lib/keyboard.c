#include "gpio.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "ps2.h"
#include "timer.h"
#include "printf.h"

enum
{
    PS2_CMD_RESET = 0xFF,
    PS2_CODE_ACK = 0xFA,
    PS2_CMD_FLAGS = 0xED,
    PS2_CMD_ENABLE_DATA_REPORTING = 0xF4
};
enum
{
    START_BIT = 0,
    STOP_BIT = 1
};
static unsigned CLK = GPIO_PIN3;
static unsigned DATA = GPIO_PIN4;

static void wait_for_falling_clock_edge(void)
{
    while (gpio_read(CLK) == 0)
    {
    }
    while (gpio_read(CLK) == 1)
    {
    }
}

static int has_odd_parity(unsigned char code)
{
    unsigned int sum = 0;
    for (unsigned int i = 0; i < 8; i++)
    {
        sum += code & 1;
        code >>= 1;
    }
    return sum;
}

static void write_bit(int nbit, unsigned char code)
{
    switch (nbit)
    {
    case 1:
        gpio_write(DATA, START_BIT);
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        gpio_write(DATA, (code >> (nbit - 2)) & 1);
        break;
    case 10:
        gpio_write(DATA, has_odd_parity(code));
        break;
    case 11:
        gpio_write(DATA, STOP_BIT);
        break;
    }
}

static void ps2_write(unsigned char command)
{
    gpio_set_output(CLK); // swap GPIO from read to write
    gpio_set_output(DATA);
    gpio_write(CLK, 0);  // bring clock line low to inhibit communication
    timer_delay_us(200); // hold time > 100 us
    gpio_write(DATA, 0); // initiate request-to-send, this is START bit
    gpio_write(CLK, 1);
    gpio_set_input(CLK); // release clock line, device will now pulse clock for host to write

    for (int i = 2; i <= 11; i++)
    { // START bit sent above, now send bits 2-11
        wait_for_falling_clock_edge();
        write_bit(i, command);
    }

    gpio_set_input(DATA); // done writing, exit from request-to-send
    wait_for_falling_clock_edge();
}

static int read_bit(void)
{
    // falling edge on clock triggers read
    // wait until clock reads high, then wait until clock reads low
    // now read data
    wait_for_falling_clock_edge();
    return gpio_read(DATA);
}

void keyboard_init(unsigned int clock_gpio, unsigned int data_gpio)
{
    ps2_write(PS2_CMD_RESET);
    printf("Reset code Sent.\n");

    CLK = clock_gpio;
    gpio_set_input(CLK);
    gpio_set_pullup(CLK);

    DATA = data_gpio;
    gpio_set_input(DATA);
    gpio_set_pullup(DATA);
}

unsigned char keyboard_read_scancode(void)
{
    unsigned char scancode = 0;
    while (read_bit() != 0)
    {
        scancode = 0;
    }
    for (int i = 0; i < 8; i++)
    {
        scancode |= (read_bit() << i);
    }

    int parity = read_bit();
    if (has_odd_parity(parity) == 0)
    {
    }
    //int stop = read_bit();

    return scancode;
}

key_action_t keyboard_read_sequence(void)
{
    key_action_t action;
    unsigned char scancode = keyboard_read_scancode();

    if (scancode == PS2_CODE_EXTENDED)
    {
        scancode = keyboard_read_scancode();
    }
    if (scancode == PS2_CODE_RELEASE)
    {
        action.what = KEY_RELEASE;
        action.keycode = keyboard_read_scancode();
    }
    else
    {
        action.what = KEY_PRESS;
        action.keycode = scancode;
    }

    return action;
}

key_event_t keyboard_read_event(void)
{
    // TODO: Your code here
    key_event_t event;
    return event;
}

unsigned char keyboard_read_next(void)
{
    // TODO: Your code here
    return '!';
}
