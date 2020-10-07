/*
 * LARSON SCANNER
 *
 * ARM Assembly Code for Raspberry Pi 1A+
 * Controls LEDs connected to GPIO Pins 20-27
 * LED's light up in a "Larson Scanner" pattern
 * written by: Tristan Sinclair 9/29/20
 * for: CS107E
 */
// delays are used for controlling brightness
.equ DELAY, 0x150
.equ DELAY2, 0x300
.equ DELAY3, 0x400
// speed can be used to increase or decrease movement speed
.equ SPEED, 0x300

// configure GPIO 20 - 27 for output
ldr r0, FSEL2
mov r1, #0x240000
add r1, r1, #0x009200
add r1, r1, #0x000049
str r1, [r0]

// prep register to hold GPIO on/off switches
mov r1, #(31<<18) // all LEDs on
mov r6, #(17<<18) // middle 3 LEDs
mov r7, #(10<<18) // main LED on

// counter
mov r3, #0
// destination
mov r4, #7

loop:

// set up timer
mov r5, #SPEED

  brightness:
    // set all LEDs high
    ldr r0, SET0
    str r1, [r0]

    // delay
    mov r2, #DELAY
    wait1:
        subs r2, #1
        bne wait1

    // set edge GPIOs low
    ldr r0, CLR0
    str r6, [r0]

    // delay
    mov r2, #DELAY2
    wait2:
        subs r2, #1
        bne wait2

    // set all GPIOs low except middle
    ldr r0, CLR0
    str r7, [r0]

    // delay
    mov r2, #DELAY3
    wait3:
        subs r2, #1
        bne wait3

  subs r5, #1
  bne brightness

// compare counter and location
cmp r3, r4
  beq switch
  blt forward
  bgt backward

b loop

// Moves the LED forward
forward:
  lsl r1, r1, #1
  lsl r6, r6, #1
  lsl r7, r7, #1
  add r3, r3, #1
b loop

// Brings the LED back
backward:
  lsr r1, r1, #1
  lsr r6, r6, #1
  lsr r7, r7, #1
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
