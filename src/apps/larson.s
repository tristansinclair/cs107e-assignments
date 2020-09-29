/*
 * LARSON SCANNER
 *
 * ARM Assembly Code for Raspberry Pi 1A+
 * Controls LEDs connected to GPIO Pins 20-27
 * LED's light up in a BASIC "Larson Scanner" pattern
 * (one LED at a time)
 *
 * written by: Tristan Sinclair 9/29/20
 * for: CS107E
 */

.equ DELAY, 0x120000

// configure GPIO 20 - 27 for output
ldr r0, FSEL2
mov r1, $0x240000
add r1, r1, #0x009200
add r1, r1, #0x000049
str r1, [r0]

// prep register to hold GPIO on/off switch
mov r1, #(1<<20)
// counter
mov r3, #0
// destination
mov r4, #7

loop:

// set current GPIO high
ldr r0, SET0
str r1, [r0]

// delay
mov r2, #DELAY
wait1:
    subs r2, #1
    bne wait1

// set current GPIO low
ldr r0, CLR0
str r1, [r0]

// compare counter to destination
cmp r3, r4
  beq switch
  blt forward
  bgt backward

b loop

// Moves the LED forward 1
forward:
  lsl r1, r1, #1
  add r3, r3, #1
b loop

// Brings the LED back 1
backward:
  lsr r1, r1, #1
  sub r3, r3, #1
b loop

// Switches the direction
switch:
  cmp r3, #7
    subeq r4, r4, #7
    addne r4, r4, #7
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
