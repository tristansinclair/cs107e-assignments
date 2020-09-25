/*
 * LARSON SCANNER
 *
 * Currently this code is the blink code copied from lab 1.
 *
 * Modify the code to implement the larson scanner for assignment 1.
 *
 * Make sure to use GPIO pins 20-23 (or 20-27) for your scanner.
 */

.equ DELAY, 0x3F0000

// configure GPIO 20, 21, 22, 23 for output
ldr r0, FSEL2
mov r1, #0x001200
add r1, r1, #0x0049
str r1, [r0]


mov r1, #(1<<20)

loop:

// set GPIO 2x high
ldr r0, SET0
str r1, [r0]

// delay
mov r2, #DELAY
wait1:
    subs r2, #1
    bne wait1

// set next GPIO low
ldr r0, CLR0
str r1, [r0]
// set next GPIO on
lsl r1, #0x1
str r1, [r0]

// delay
mov r2, #DELAY
wait2:
    subs r2, #1
    bne wait2

b loop

// FSEL used for setting the function of a GPIO pin
FSEL0: .word 0x20200000  // Address of the GPIO Function Select 0
FSEL1: .word 0x20200004  // Address of the GPIO Function Select 1
FSEL2: .word 0x20200008  // Address of the GPIO Function Select 2

SET0:  .word 0x2020001C // Adress where GPIO on/off is controlled
SET1:  .word 0x20200020

CLR0:  .word 0x20200028
CLR1:  .word 0x2020002C
