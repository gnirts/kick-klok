#ifndef SP404_CHANNEL

#define SP404_CHANNEL 1
#define SP404_NOTE_OFFSET 76

#define SP404_NOTE_E1  88
#define SP404_NOTE_E2  89
#define SP404_NOTE_E3  90
#define SP404_NOTE_E4  91
#define SP404_NOTE_E5  84
#define SP404_NOTE_E6  85
#define SP404_NOTE_E7  86
#define SP404_NOTE_E8  87
#define SP404_NOTE_E9  80
#define SP404_NOTE_E10 81
#define SP404_NOTE_E11 82
#define SP404_NOTE_E12 83
#define SP404_NOTE_E13 76
#define SP404_NOTE_E14 77
#define SP404_NOTE_E15 78
#define SP404_NOTE_E16 79

#define BUS1      1
#define BUS2      2
#define BUS3      3
#define BUS4      4
#define INPUT_BUS 5

#define SP404_CC_FX_ENABLE 19
#define SP404_CC_FX_NUMBER 83
#define SP404_CC_FX_CTRL1  16
#define SP404_CC_FX_CTRL2  17
#define SP404_CC_FX_CTRL3  18
#define SP404_CC_FX_CTRL4  80
#define SP404_CC_FX_CTRL5  81
#define SP404_CC_FX_CTRL6  82

#define ON  127
#define OFF 0

void sp404_fx(int bus, int enable);
void sp404_note(int note);

#endif