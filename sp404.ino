#include "sp404.h"
#include "global.h"

void sp404_fx(int bus, int enable) {
    SP404_OUT.sendControlChange(SP404_CC_FX_ENABLE, enable, bus);
}

void sp404_note(int note) {
    SP404_OUT.sendNoteOn(note, 127, SP404_CHANNEL);
}
