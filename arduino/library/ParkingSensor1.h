//-*- mode: c -*-
/* 
 * NAME
 *     ParkingSensor1
 * PURPOSE
 *     Arduino Nano connected to a "TYPE-1" parking sensor controller to decode the PWM and send it via USB to a host (eg a RPi).
 * SEE
 *     http://allegrobotics.com/parkingSensor.html
 * AUTHOR
 *     Scott BARNES 2017/2018. IP freely on non-commercial applications.
 * NOTES
 * It will NOT WORK with the "TYPE-2" parking sensor controller (use ParkingSensor2 for that).
 * It assumes that the sensor is in 'reverse' mode - ie that the red and yellow wires are held high (at 12V).
 * (Making a more sophisticated version with reversing controls would be easy enough, but not yet done).
 * HOW IT WORKS
 * It connects to a host (eg a Raspberry Pi) via USB (which provides power as well as comms.)
 * 12V must be supplied to the parking sensor via other means.
 * PROTOCOL FROM HOST
 * No data is sent from the host.
 * PROTOCOL TO HOST
 * Sends to host text lines many times a second of the form "SXX" where S is the sensor (A,B,C,D,E,F,G,H) and XX is the distance in cm expressed as hex ('FF' is max distance - ie no measurement).
 * This will normally be in cycles of six (A,B,C,D,E,H sensors F and G normally don't work in 'reverse' mode).
 * eg: the line "Pa15\r\nPbFF\r\nPcFF\r\nPdFF\r\nPe40\r\nPh44\r\n"
 * Means sensor A has detected a 21cm object, E has detected a 64cm object, H has detected a 68cm object and the rest have detected nothing.
 *
 * SEE ALSO
 * ParkingSensor1.java - some Java code which talks to this. If you change this code you may have to change that too.
 * 
 * HOW TO CONNECT IT
 *
 *   POWER       MONITOR       E        F        G        H        A        B        C        D
 * +---------+ +---------+  +-----+  +-----+  +-----+  +-----+  +-----+  +-----+  +-----+  +-----+
 * | . . . . | | . . . . |  | . . |  | . . |  | . . |  | . . |  | . . |  | . . |  | . . |  | . . |
 * +---------+ +---------+  +-----+  +-----+  +-----+  +-----+  +-----+  +-----+  +-----+  +-----+
 *   B B Y R     G S 5 ?
 * Key: B=Black, B=Blue, Y=Yellow, R=Red, G=GND, S=SIGNAL, 5=5VDT, ?=WTF above
 * Don't bother plugging in sensors F and G - they don't work in reverse mode.
 * 
 * Connect the POWER-Black to ground on the 12V battery
 * Connect the POWER-Red, POWER-Blue and POWER-Yellow to +12V on the battery
 * Connect the GND and OUTPUT from the parking sensor controller (the left-most and second to left-most pins in the MONITOR socket) to GND and D2 on the Arduino Nano.
 * 
 * THE PROTOCOL - FROM THE parking sensor (see http://allegrobotics.com/parkingSensor.html)
 * 
 * The protocol - from the parking sensor is 33%/66% duty cycle PWM, LOW is 0V, HIGH is 5V.
 * Between each packet, there is:  3500us LOW, 2000us HIGH, 1000us LOW, 100us HIGH.
 * Each packet consists of 48 bits (6 bytes).
 * Each bit is 100us LOW, 100us { LOW if 1, HIGH if 0 }, 100us HIGH.
 * 
 * So the easiest way to read the data is to measure the duration of the LOW periods. 100us is '0', 200us is '1', > 900us is a break between packets (but be warned there are two of them).
 * DIGITAL_PIN_TO_INTERRUPT
 *     digitalPinToInterrupt() doesn't seem to be available in the arduino environment, so we have to hardwire values.
 *     On the Nano (and Uno etc) there are two interrupts: interrupt 0 which can be used on pin D2, and interrupt 1 which can be used on pin D3.
 *     On the Nano (and Uno etc) digitalPinToInterrupt(2) should return 0, and digitalPinToInterrupt(3) should return 1.
 */

#ifndef ParkingSensor1_h
#define ParkingSensor1_h

#include <Arduino.h>
#include "King.h"

#define DEFAULT_PARKING_SENSOR_PIN                   2  /* For Nano and similar 2 for D2, 3 for D3 */
#define DEFAULT_PARKING_SENSOR_READ_PIN              0  /* digitalPinToInterrupt(DEFAULT_PARKING_SENSOR_PIN) */

class ParkingSensor1 : public King {
public:
    ParkingSensor1(byte pin = DEFAULT_PARKING_SENSOR_PIN, byte pinInterrupt = DEFAULT_PARKING_SENSOR_READ_PIN);
    void setup();
    void loop(uint32_t now);
    void command(char *commandLine) {};
private:
    static void risingEdge();
    static void fallingEdge();
    uint32_t dataLastSentAt;   // The time (millis()) we last sent some data to the host.
    byte pin;
    static byte pinInterrupt;
    static char *hex;
    static uint32_t timeOfFall;
    static byte numBitsRead;
    static byte byteCount;
    static byte byteBeingRead;
    static char lineToHost[];
    volatile static byte dataIsAvailable;
    volatile static byte errorNumber;
};

#endif /* ParkingSensor1_h */
