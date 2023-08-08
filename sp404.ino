#include "sp404.h"
#include "global.h"

void sp404_fx(int bus, int enable) {
    MIDI6.sendControlChange(SP404_CC_FX_ENABLE, enable, bus);
}

void sp404_note(int note) {
    MIDI6.sendNoteOn(note, 127, SP404_CHANNEL);
}
