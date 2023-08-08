
#ifndef GLOBAL_INCLUDED
#define GLOBAL_INCLUDED

#include "sp404.h"
#include <MIDI.h>
#include <USBHost_t36.h>

#define GHOST_THRESHOLD 34
#define MAX_VEL         127
#define MIN_VEL         1

// Create the Serial MIDI ports
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI4);
// MIDI_CREATE_INSTANCE(HardwareSerial, Serial5, MIDI5);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial6, MIDI6);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial7, MIDI7);
// MIDI_CREATE_INSTANCE(HardwareSerial, Serial8, MIDI8);

// Create the ports for USB devices plugged into Teensy's 2nd USB port (via
// hubs)
USBHost     myusb;
USBHub      hub1(myusb);
USBHub      hub2(myusb);
USBHub      hub3(myusb);
USBHub      hub4(myusb);
MIDIDevice  midi01(myusb);
MIDIDevice  midi02(myusb);
MIDIDevice  midi03(myusb);
MIDIDevice  midi04(myusb);
MIDIDevice  midi05(myusb);
MIDIDevice  midi06(myusb);
MIDIDevice  midi07(myusb);
MIDIDevice  midi08(myusb);
MIDIDevice *midilist[8] = {
    &midi01, &midi02, &midi03, &midi04, &midi05, &midi06, &midi07, &midi08,
};

#endif /* GLOBAL_INCLUDED */