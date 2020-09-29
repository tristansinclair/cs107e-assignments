/*
 * LARSON SCANNER
 *
 * Currently this code is the blink code copied from lab 1.
 *
 * Modify the code to implement the larson scanner for assignment 1.
 *
 * Make sure to use GPIO pins 20-23 (or 20-27) for your scanner.
 */

.equ DELAY, 0x100000

// configure GPIO 20, 21, 22, 23 for output
ldr r0, FSEL2
mov r1, #0x009200
add r1, r1, #0x0049
str r1, [r0]

// Prep register to hold GPIO on/off switch
mov r1, #(1<<20)

// Counter
mov r3, #0
// destination
mov r4, #5

loop:

// set GPIO 20 high
ldr r0, SET0
str r1, [r0]

// delay
mov r2, #DELAY
wait1:
    subs r2, #1
    bne wait1

// set GPIO 20 low
ldr r0, CLR0
str r1, [r0]

cmp r3, r4
  beq switch
  blt forward
  bgt backward
b loop

// Moves the LED forward
forward:
  lsl r1, r1, #1
  add r3, r3, #1
b loop

// Brings the LED back
backward:
  lsr r1, r1, #1
  sub r3, r3, #1
b loop

// Switches the direction
switch:
  cmp r3, #5
    subeq r4, r4, #5
    addne r4, r4, #5
  beq backward
  bne forward



// FSEL used for setting the function of a GPIO pin
FSEL0: .word 0x20200000  // Address of the GPIO Function Select 0
FSEL1: .word 0x20200004  // Address of the GPIO Function Select 1
FSEL2: .word 0x20200008  // Address of the GPIO Function Select 2 (GPIO 20-29)

// SET used for turning on a GPIO pin
SET0:  .word 0x2020001C
SET1:  .word 0x20200020

// CLR used for turning off a GPIO pin
CLR0:  .word 0x20200028
CLR1:  .word 0x2020002C
