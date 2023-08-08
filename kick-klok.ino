/*
  Select Serial + MIDIx4 from the "Tools > USB Type" menu
  This example code is in the public domain.
*/

#include "dtx.h"
#include "global.h"
#include "sp404.h"
#include "volca.h"
#include <MIDI.h>
#include <USBHost_t36.h> // access to USB MIDI devices (plugged into 2nd USB port)

void setup() {
    Serial.begin(115200);

    Serial.println("Power on!");

    MIDI1.begin(MIDI_CHANNEL_OMNI);
    MIDI2.begin(MIDI_CHANNEL_OMNI);
    MIDI3.begin(MIDI_CHANNEL_OMNI);
    MIDI4.begin(MIDI_CHANNEL_OMNI);
    MIDI6.begin(MIDI_CHANNEL_OMNI);
    MIDI7.begin(MIDI_CHANNEL_OMNI);

    MIDI1.turnThruOff();
    MIDI2.turnThruOff();
    MIDI3.turnThruOff();
    MIDI4.turnThruOff();
    MIDI6.turnThruOff();
    MIDI7.turnThruOff();

    myusb.begin();
}

void forward_midi(midi::MidiType type, midi::Channel channel, midi::DataByte data1, midi::DataByte data2) {
    usbMIDI.send(type, data1, data2, channel, 0);
    MIDI1.send(type, data1, data2, channel); // send to breakbox via TR8
    // cannot use MIDI2 output with audio board
    MIDI3.send(type, data1, data2, channel);
    MIDI4.send(type, data1, data2, channel);
    MIDI6.send(type, data1, data2, channel); // SP-404
    MIDI7.send(type, data1, data2, channel); // Volca KiCK
}
/*

 -
 - velocity
 - user switched
    - pedal
 - per song basis


things 404 could do
- low probability accents
- momentary fx applied to DTX audio
- automated fx applied to DTX audio
- chopped breaks

automate starting 404 pattern mode clock
at the same time the start message comes from the TR8


have a blank pattern on 404
start pattern when TR8 starts
*/
void read_dtx_msg(midi::MidiType type, midi::Channel channel, midi::DataByte data1, midi::DataByte data2) {

        // print message unless it's clock
        if (type != midi::Clock && type != midi::NoteOn) {
            // Serial.printf("%X ch%2d  %3d  %3d\t", type, channel, data1, data2);
        }

        if (type == midi::Start) {

            Serial.printf("=== starting on channel %d ===\n", channel);
            forward_midi(type, channel, data1, data2);

            // send a start pattern sequence command to 404

        } else if (type == midi::Stop) {

            Serial.printf("=== stopping on channel %d ===\n", channel);
            forward_midi(type, channel, data1, data2);

        } else if (type == midi::ProgramChange) {
            Serial.printf("=== pgrm change [%d %d] on channel %d ===\n", data1, data2, channel);

            MIDI1.send(type, data1, data2, channel); // send to breakbox via TR8

        } else if (type == midi::ControlChange) {

            MIDI1.send(type, data1, data2, channel); // send to breakbox via TR8

            // check if incoming CC is the hi hat depression
            // store the last seen value
            // if it's over a threshold, send notes to 404
            // otherwise send to TR8

        } else if (type == midi::NoteOn) {
            int note    = data1;
            int vel     = data2;
            int note404 = 0;

            switch (note) {
                case DTX_SNARE_NOTE: Serial.println("  S                   "); break;
                case DTX_TOM1_NOTE: Serial.println("    T1                "); break;
                case DTX_TOM2_NOTE: Serial.println("       T2             "); break;
                case DTX_TOM3_NOTE: Serial.println("           T3         "); break;

                case DTX_CYMB_CRASH1_NOTE: Serial.println("K S T1 T2 T3 C1 C2 R H"); break;

                case DTX_CYMB_CRASH2_NOTE: Serial.println("K S T1 T2 T3 C1 C2 R H"); break;

                case DTX_CYMB_RIDE_NOTE: Serial.println("K S T1 T2 T3 C1 C2 R H"); break;
            }

            // decide if BBX or 404
            if (note == DTX_KICK_NOTE) {
                triggerKick(KICK_LOW_NOTE, vel);
                Serial.printf("sent KiCK vel=%d\t", vel);

            } else if (true) {
                MIDI1.send(type, note, vel, channel); // send to breakbox via TR8

            } else if (false) {

                switch (note) {
                    case DTX_SNARE_NOTE: note404 = SP404_NOTE_E5; break;
                    case DTX_TOM1_NOTE: note404 = SP404_NOTE_E9; break;
                    case DTX_TOM2_NOTE: note404 = SP404_NOTE_E10; break;
                    case DTX_TOM3_NOTE: note404 = SP404_NOTE_E12; break;
                    case DTX_CYMB_CRASH1_NOTE: note404 = SP404_NOTE_E7; break;
                    case DTX_CYMB_CRASH2_NOTE: note404 = SP404_NOTE_E8; break;
                    case DTX_CYMB_RIDE_NOTE: note404 = SP404_NOTE_E16; break;
                }
                if (note404) {

                    sp404_fx(BUS1, ON);
                    if (vel > 60) {
                        sp404_fx(BUS2, ON);
                    } else {
                        sp404_fx(BUS2, OFF);
                    }
                    sp404_note(note404);

                    if (vel < GHOST_THRESHOLD) {
                        sp404_fx(BUS1, OFF);
                        sp404_fx(BUS2, OFF);
                        MIDI6.sendNoteOn(note404, MIN_VEL, SP404_CHANNEL);
                        Serial.printf("sent SP404 note %d\t", note404);
                    } else {
                    }
                }
            } else {
                Serial.printf("unknown note %d \t", note);
            }
        }

        Serial.print("\n");
}

void loop() {

    if (MIDI1.read()) {
        midi::MidiType type    = MIDI1.getType();
        midi::Channel  channel = MIDI1.getChannel();
        midi::DataByte data1   = MIDI1.getData1();
        midi::DataByte data2   = MIDI1.getData2();

        if (type == midi::ActiveSensing || type == midi::SystemExclusive) {
            return;
        }
        read_dtx_msg(type, channel, data1, data2);
    }

    if (MIDI2.read()) {
    }

    // cannot use MIDI3 input with audio board

    if (MIDI4.read()) {
    }

    // Next read messages arriving from the (up to) 10 USB devices plugged into the USB Host port
    for (int port = 0; port < 8; port++) {
        if (midilist[port]->read()) {
            uint8_t        type    = midilist[port]->getType();
            uint8_t        data1   = midilist[port]->getData1();
            uint8_t        data2   = midilist[port]->getData2();
            uint8_t        channel = midilist[port]->getChannel();
            byte           cable   = port + 8;
            if (type != midi::SystemExclusive) {
                usbMIDI.send(type, data1, data2, channel, cable);
            }
        }
    }

    if (usbMIDI.read()) {
        byte type    = usbMIDI.getType();
        byte channel = usbMIDI.getChannel();
        byte data1   = usbMIDI.getData1();
        byte data2   = usbMIDI.getData2();
        byte cable   = usbMIDI.getCable();

        // forward this message to 1 of the 3 Serial MIDI OUT ports
        if (type != usbMIDI.SystemExclusive) {
            // Normal messages, first we must convert usbMIDI's type (an ordinary
            // byte) to the MIDI library's special MidiType.
            midi::MidiType mtype = (midi::MidiType)type;

            // Then simply give the data to the MIDI library send()
            switch (cable) {
                case 0: MIDI1.send(mtype, data1, data2, channel); break;
                case 1: break; // cannot send on MIDI2
                case 2: MIDI3.send(mtype, data1, data2, channel); break;
                case 3: MIDI4.send(mtype, data1, data2, channel); break;
                case 4: break; // MIDI5 is not connected
                case 5: MIDI6.send(mtype, data1, data2, channel); break;
                case 6: MIDI7.send(mtype, data1, data2, channel); break;
                case 7: break; // MIDI8 is not connected
                default: 
                    midilist[cable - 8]->send(type, data1, data2, channel);
            }
        }
    }
}
