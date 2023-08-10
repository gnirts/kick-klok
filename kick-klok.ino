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

unsigned long loop_count  = 0;
unsigned long prev_millis = 0;

void setup() {
    Serial.begin(115200);

    Serial.println("Power on!");

    MIDI1.begin(MIDI_CHANNEL_OMNI);
    MIDI2.begin(MIDI_CHANNEL_OMNI);
    MIDI3.begin(MIDI_CHANNEL_OMNI);
    MIDI4.begin(MIDI_CHANNEL_OMNI);
    // can't use MIDI5 because of the audio board
    MIDI6.begin(MIDI_CHANNEL_OMNI);
    MIDI7.begin(MIDI_CHANNEL_OMNI);

    MIDI1.turnThruOff();
    MIDI2.turnThruOff();
    MIDI3.turnThruOff();
    MIDI4.turnThruOff();
    // can't use MIDI5 because of the audio board
    MIDI6.turnThruOff();
    MIDI7.turnThruOff();

    myusb.begin();
}

void read_dtx_msg(midi::MidiType type, midi::Channel channel, midi::DataByte data1, midi::DataByte data2) {

    // print message unless it's clock
    if (type != midi::Clock && type != midi::NoteOn) {
        Serial.printf("%X ch%2d  %3d  %3d\t", type, channel, data1, data2);
    }

    if (type == midi::Start) {
        Serial.printf("=== starting on channel %d ===\n", channel);
        // forward_midi(type, channel, data1, data2);
    } else if (type == midi::Stop) {
        Serial.printf("=== stopping on channel %d ===\n", channel);
        // forward_midi(type, channel, data1, data2);

    } else if (type == midi::ProgramChange) {
        Serial.printf("=== pgrm change [%d %d] on channel %d ===\n", data1, data2, channel);
        DTX_IN.send(type, data1, data2, channel); // send to breakbox via TR8

    } else if (type == midi::ControlChange) {
        DTX_IN.send(type, data1, data2, channel); // send to breakbox via TR8
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
            TR8S_THRU.send(type, note, vel, channel); // send to breakbox via TR8
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
                    SP404_OUT.sendNoteOn(note404, MIN_VEL, SP404_CHANNEL);
                    Serial.printf("sent SP404 note %d\t", note404);
                } 
            }
        } else {
            Serial.printf("unknown note %d \t", note);
        }
    }

    Serial.print("\n");
}

void read_sp404_msg(midi::MidiType type, midi::Channel channel, midi::DataByte data1, midi::DataByte data2) {
    if (type == midi::ActiveSensing || type == midi::SystemExclusive) {
        return;
    }

    if (channel == 16) {
        if (type == midi::NoteOn) {
            Serial.printf("chromatic note: %3d  %3d\n", data1, data2);
        } else {
            Serial.printf("chromatic 0x%X: %3d  %3d\n", type, data1, data2);
        }
        return;
    }

    if (type == midi::Start) {
        Serial.printf("=== starting on ch%d ===\n", channel);
        return;
    }

    if (type == midi::Stop) {
        Serial.printf("=== stopping on ch%d ===\n", channel);
        return;
    }

    if (type == midi::ProgramChange) {
        Serial.printf("=== pgrm change [%d %d] on ch%d ===\n", data1, data2, channel);
        return;
    }

    if (type == midi::ControlChange) {
        Serial.printf("=== ctrl change [%d %d] on ch%d ===\n", data1, data2, channel);
        return;
    }

    if (type == midi::NoteOn) {
        Serial.printf("ch%d  On %d      vel %d\n", channel, data1, data2);
        return;
    }

    if (type == midi::NoteOff) {
        Serial.printf("ch%d     %d Off  vel %d\n", channel, data1, data2);
        return;
    }

    if (type == midi::AfterTouchPoly) {
        Serial.printf("ch%d     %d      %d AfterTouchPoly\n", channel, data1, data2);
        return;
    }

    Serial.printf("\t\t\t\t%X ch%2d  %3d  %3d\n", type, channel, data1, data2);
}

void read_tr8_msg(midi::MidiType type, midi::Channel channel, midi::DataByte data1, midi::DataByte data2) {
    if (type == midi::ActiveSensing || type == midi::SystemExclusive) {
        return;
    }

    TR8S_THRU.send(type, data1, data2, channel);

    if (type == midi::Start) {
        Serial.printf("=== TR8 starting on ch%d ===\n", channel);

        // reset the SP404 pattern clock

        return;
    }

    if (type == midi::Stop) {
        Serial.printf("=== TR8 stopping on ch%d ===\n", channel);

        // stop the SP404 pattern clock
        return;
    }

    if (type == midi::ProgramChange) {
        Serial.printf("=== TR8 pgrm change [%d %d] on ch%d ===\n", data1, data2, channel);

        // reset the SP404 pattern clock

                // 404 example:
        // 0xC3 0F -> Bank D Pattern 16
        // PC#0 = Pattern 1

        return;
    }

    if (type == midi::ControlChange) {
        Serial.printf("=== TR8 ctrl change [%d %d] on ch%d ===\n", data1, data2, channel);
        return;
    }

    if (type == midi::NoteOn) {
        // can we detect anything interesting?
        // ext trig
        // bass drum
        return;
    }

    if (type == midi::NoteOff) {
        return;
    }

    Serial.printf("\t\t\t\t%X ch%2d  %3d  %3d\n", type, channel, data1, data2);
}

void read_usb(void) {
    // Next read messages arriving from the (up to) 10 USB devices plugged into the USB Host port
    for (int port = 0; port < 8; port++) {
        if (midilist[port]->read()) {
            uint8_t type    = midilist[port]->getType();
            uint8_t data1   = midilist[port]->getData1();
            uint8_t data2   = midilist[port]->getData2();
            uint8_t channel = midilist[port]->getChannel();
            byte    cable   = port + 8;
            Serial.printf("midilist[%d] cable=%d: %X ch%2d  %3d  %3d\n", port, cable, type, channel, data1, data2);
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

            Serial.printf("usbMIDI cable=%d: %X ch%2d  %3d  %3d\n", cable, type, channel, data1, data2);

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
                default: midilist[cable - 8]->send(type, data1, data2, channel);
            }
        }
    }
}

void loop() {
    unsigned long current_millis = millis();

    if (prev_millis == 0 || current_millis - prev_millis > 1000) {
        // Serial.printf("loop_count=%u\n", loop_count);
        prev_millis = current_millis;
    }
    loop_count++;

    if (DTX_IN.read()) {
        midi::MidiType type    = DTX_IN.getType();
        midi::Channel  channel = DTX_IN.getChannel();
        midi::DataByte data1   = DTX_IN.getData1();
        midi::DataByte data2   = DTX_IN.getData2();

        if (type == midi::ActiveSensing || type == midi::SystemExclusive) {
            return;
        }
        read_dtx_msg(type, channel, data1, data2);
    }

    if (TR8S_IN.read()) {
        midi::MidiType type    = TR8S_IN.getType();
        midi::Channel  channel = TR8S_IN.getChannel();
        midi::DataByte data1   = TR8S_IN.getData1();
        midi::DataByte data2   = TR8S_IN.getData2();

        read_tr8_msg(type, channel, data1, data2);
    }

    if (SP404_IN.read()) {
        // Pin       Input  Output
        // SLEEVE    Red    Green
        // RING2
        // RING1     Brown  Blue
        // TIP       Orange Yellow

        midi::MidiType type    = SP404_IN.getType();
        midi::Channel  channel = SP404_IN.getChannel();
        midi::DataByte data1   = SP404_IN.getData1();
        midi::DataByte data2   = SP404_IN.getData2();

        read_sp404_msg(type, channel, data1, data2);
    }

    if (USB_ENABLED) {
        read_usb();
    }
}
