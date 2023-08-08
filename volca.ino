#include "global.h"
#include "volca.h"

int limit(int value, int lowerBound, int upperBound) {
    return max(lowerBound, min(value, upperBound));
}

void setCC(int cc, int value) {
    value = limit(value, 0, 127);
    MIDI1.sendControlChange(cc, value, KICK_CHANNEL);
}

void setPulseColor(int color) { setCC(KICK_CC_PULSE_COLOR, color); }
void setPulseLevel(int level) { setCC(KICK_CC_PULSE_LEVEL, level); }
void setAttack(int attack) { setCC(KICK_CC_ATTACK, attack); }
void setDecay(int decay) { setCC(KICK_CC_DECAY, decay); }
void setDrive(int drive) { setCC(KICK_CC_DRIVE, drive); }
void setTone(int tone) { setCC(KICK_CC_TONE, tone); }
void setPitch(int pitch) { setCC(KICK_CC_PITCH, pitch); }
void setBend(int bend) { setCC(KICK_CC_BEND, bend); }
void setTime(int time) { setCC(KICK_CC_TIME, time); }

void triggerKick(int note, const int velocity) {
    int v = velocity * 2;
    int threshold = 0;


    /*
    0         67 68      89 90      172 173      230 227    255
    60 drive 127 43 bend 64 45 tone 127 70 decay 127 65 bend 90 
    */

    setDrive(KICK_DRIVE_MIN);
    setBend(KICK_BEND1_MIN);
    setDecay(KICK_DECAY_MIN);
    setTone(KICK_TONE_MIN);

    Serial.printf("kick with v=%3d\n", v);
    if( v >= threshold) {
        setDrive(KICK_DRIVE_MIN + (v - threshold));
        threshold += KICK_DRIVE_RANGE;
        Serial.printf("drive=%3d\n", KICK_DRIVE_MIN + (v - threshold));
    }

    if( v >= threshold) {
        setBend(limit(v - threshold, KICK_BEND1_MIN, KICK_BEND1_MAX));
        threshold += KICK_BEND1_RANGE;
        Serial.printf("bend1=%3d\n", limit(v - threshold, KICK_BEND1_MIN, KICK_BEND1_MAX));
    }

    if( v >= threshold) {
        setTone(KICK_TONE_MIN + (v - threshold));
        threshold += KICK_TONE_RANGE;
        Serial.printf("tone=%3d\n", KICK_TONE_MIN + (v - threshold));
    }

    if( v >= threshold) {
        setDecay(KICK_DECAY_MIN + (v - threshold));
        threshold += KICK_DECAY_RANGE;
        Serial.printf("decay=%3d\n", KICK_DECAY_MIN + (v - threshold));
    }

    if( v >= threshold) {
        setBend(limit(v - threshold, KICK_BEND2_MIN, KICK_BEND2_MAX));
        Serial.printf("bend2=%3d\n", limit(v - threshold, KICK_BEND2_MIN, KICK_BEND2_MAX));
    }

    // setDrive(KICK_DRIVE_MIN + velocity);
    // setBend(KICK_BEND1_MIN + velocity);
    // setDecay(KICK_DECAY_MIN + velocity);
    // setTone(KICK_TONE_MIN + velocity);

    note = limit(note, KICK_LOW_NOTE, KICK_HIGH_NOTE);
    MIDI7.sendNoteOn(note, 127, KICK_CHANNEL);
    MIDI7.sendNoteOff(note, 127, KICK_CHANNEL);
}