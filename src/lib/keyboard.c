#include "gpio.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "ps2.h"
#include "timer.h"
#include "printf.h"
#include "gpio_interrupts.h"
#include "ringbuffer.h"
#include "malloc.h"
#include "assert.h"

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

// all for read_bit
static unsigned int bit = 0;
static unsigned int bit_counter = 0;
static unsigned char scancode = 0;
static unsigned int timer = 0;
static int split = 0;
static bool first_bit = true;

rb_t *rb;

static void wait_for_falling_clock_edge(void)
{
    while (gpio_read(CLK) == 0)
    {
    }
    while (gpio_read(CLK) == 1)
    {
    }
}

/** 
 * has_odd_parity
 * from Peter
 * returns the number of 1s in a bit
*/
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

// from Phil for handling the reset code
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

// from Phil for handling the reset code
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
    //wait_for_falling_clock_edge();
}

static void reset_scancode(void)
{
    scancode = 0;
    bit_counter = 0;
    first_bit = true;
}

static bool read_bit(unsigned int pc)
{
    if (!gpio_check_and_clear_event(CLK)) // check and clear event
    {
        return false;
    }

    // timing of falling edge to ensure no more than 3ms occurs between clock edges
    split = timer_get_ticks() - timer;      // split in between read_bit calls
    timer = timer_get_ticks();              // when read_bit is called, get the current clock
    if (first_bit == false && split > 3000) // if this isn't the first bit we're reading, check the split
    {
        reset_scancode();
        return false;
    }
    timer = timer_get_ticks(); // start timer for next clock line

    bit = gpio_read(DATA);

    switch (bit_counter)
    {
    // start bit
    case 0:
        if (bit == 0)
        {
            bit_counter++;
            first_bit = false;
            break;
        }
        else
        {
            scancode = 0;
            break;
        }
    // data bits 1 - 8
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        scancode |= (bit << (bit_counter - 1));
        bit_counter++;
        break;
    // parity bit
    case 9:
        if ((has_odd_parity(scancode) + bit) % 2 == 0)
        {
            reset_scancode();
            break;
        }

        bit_counter++;
        break;
    // stop bit
    case 10:
        if (bit == 0)
        {
            reset_scancode();
            break;
        }

        break;
    }

    if (bit_counter == 10)
    {
        rb_enqueue(rb, scancode);
        reset_scancode();
        return true;
    }

    return false;
}

void keyboard_init(unsigned int clock_gpio, unsigned int data_gpio)
{
    ps2_write(PS2_CMD_RESET); // send reset code

    CLK = clock_gpio;
    gpio_set_input(CLK);
    gpio_set_pullup(CLK);

    DATA = data_gpio;
    gpio_set_input(DATA);
    gpio_set_pullup(DATA);

    // setup gpio interrupts
    gpio_interrupts_init();
    gpio_interrupts_enable();
    gpio_enable_event_detection(CLK, GPIO_DETECT_FALLING_EDGE);    // enables event detection on CLK line for falling edge
    gpio_interrupts_register_handler(CLK, (handler_fn_t)read_bit); // registers the read_bit function to be called when CLK is triggered

    rb = rb_new();
}

static int *p_elem; // gives us a place to write scancode to

unsigned char keyboard_read_scancode(void)
{
    while (rb_empty(rb))
        ;
    rb_dequeue(rb, p_elem);
    unsigned char scancode = *p_elem;

    return scancode;
}

key_action_t keyboard_read_sequence(void)
{
    key_action_t action;
    unsigned char scancode = keyboard_read_scancode();

    // check for EXTENDED code
    if (scancode == PS2_CODE_EXTENDED) // [0xE0]
    {
        scancode = keyboard_read_scancode(); // look at next code
    }
    // now check for RELEASE code
    if (scancode == PS2_CODE_RELEASE) // [0xF0]
    {
        action.what = KEY_RELEASE;
        action.keycode = keyboard_read_scancode(); //  next code is keycode
    }
    else // if it isn't a release (single scancode)
    {
        action.what = KEY_PRESS;
        action.keycode = scancode;
    }

    return action;
}

keyboard_modifiers_t keyboard_modifier_check(key_action_t keypress, keyboard_modifiers_t modifiers)
{
    // KEYBOARD_MOD_SHIFT = 1 << 0
    if (ps2_keys[keypress.keycode].ch == PS2_KEY_SHIFT)
    {
        if (keypress.what == KEY_PRESS)
        {
            modifiers |= KEYBOARD_MOD_SHIFT;
        }
        else
        {
            modifiers &= ~KEYBOARD_MOD_SHIFT;
        }
    }
    // KEYBOARD_MOD_ALT = 1 << 1
    if (ps2_keys[keypress.keycode].ch == PS2_KEY_ALT)
    {
        if (keypress.what == KEY_PRESS)
        {
            modifiers |= KEYBOARD_MOD_ALT;
        }
        else
        {
            modifiers &= ~KEYBOARD_MOD_ALT;
        }
    }
    // KEYBOARD_MOD_CTRL = 1 << 2
    if (ps2_keys[keypress.keycode].ch == PS2_KEY_CTRL)
    {
        if (keypress.what == KEY_PRESS)
        {
            modifiers |= KEYBOARD_MOD_CTRL;
        }
        else
        {
            modifiers &= ~KEYBOARD_MOD_CTRL;
        }
    }

    /* ------------- LOCKING MODIFIERS ------------- */
    // KEYBOARD_MOD_CAPS_LOCK = 1 << 3
    if (ps2_keys[keypress.keycode].ch == PS2_KEY_CAPS_LOCK)
    {
        if (keypress.what == KEY_PRESS)
        {
            if (modifiers & KEYBOARD_MOD_CAPS_LOCK)
            {
                modifiers &= ~KEYBOARD_MOD_CAPS_LOCK;
            }
            else
            {
                modifiers |= KEYBOARD_MOD_CAPS_LOCK;
            }
        }
    }
    // KEYBOARD_MOD_SCROLL_LOCK = 1 << 4
    if (ps2_keys[keypress.keycode].ch == PS2_KEY_SCROLL_LOCK)
    {
        if (keypress.what == KEY_PRESS)
        {
            if (modifiers & KEYBOARD_MOD_SCROLL_LOCK)
            {
                modifiers &= ~KEYBOARD_MOD_SCROLL_LOCK;
            }
            else
            {
                modifiers |= KEYBOARD_MOD_SCROLL_LOCK;
            }
        }
    }
    // KEYBOARD_MOD_NUM_LOCK = 1 << 5
    if (ps2_keys[keypress.keycode].ch == PS2_KEY_NUM_LOCK)
    {
        if (keypress.what == KEY_PRESS)
        {
            if (modifiers & KEYBOARD_MOD_NUM_LOCK)
            {
                modifiers &= ~KEYBOARD_MOD_NUM_LOCK;
            }
            else
            {
                modifiers |= KEYBOARD_MOD_NUM_LOCK;
            }
        }
    }

    return modifiers;
}

key_event_t keyboard_read_event(void)
{
    key_event_t event;
    static keyboard_modifiers_t modifiers = 0b0; // 0 = off, 1 = on for each modifier

    key_action_t keypress = keyboard_read_sequence();         // read in keypress
    modifiers = keyboard_modifier_check(keypress, modifiers); // add/delete modifiers

    event.modifiers = modifiers;
    event.action = keypress;
    event.key = ps2_keys[keypress.keycode];

    return event;
}

unsigned char keyboard_read_next(void)
{
    key_event_t event = {};
    unsigned char keycode = 0;
    unsigned char character = 0;

    while (1)
    {
        event = keyboard_read_event();
        if (event.action.what == KEY_RELEASE) // if it's a release key, don't send a char
            continue;

        keycode = event.action.keycode;

        character = ps2_keys[keycode].ch;

        if ( // if it's a modifier key, don't send a char
            character == PS2_KEY_SHIFT ||
            character == PS2_KEY_CAPS_LOCK ||
            character == PS2_KEY_ALT ||
            character == PS2_KEY_CTRL ||
            character == PS2_KEY_NUM_LOCK ||
            character == PS2_KEY_SCROLL_LOCK ||
            character == 0)
        {
            continue;
        }

        // control caps lock
        if (('a' <= character && character <= 'z') && event.modifiers & KEYBOARD_MOD_CAPS_LOCK)
        {
            character = ps2_keys[keycode].other_ch;
        }
        // control shift
        if (event.modifiers & KEYBOARD_MOD_SHIFT && ps2_keys[keycode].other_ch != 0)
        {
            character = ps2_keys[keycode].other_ch;
        }

        if (character > 128 && character != PS2_KEY_ESC) // ascii up to 128 && need esc for testing
        {
            continue;
        }

        break;
    }

    return character;
}