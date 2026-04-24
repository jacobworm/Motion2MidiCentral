#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include <stdint.h>

typedef enum {
    NoteOff = 8,
    NoteOn = 9,
    PolyKeyPres = 10,
    ControlChange = 11,
    ProgramChange = 12,
    ChannelPressure = 13,
    PitchBend = 14
} midiMsgType;

typedef struct {
    midiMsgType type; 
	uint8_t channel;
	uint8_t data1;
	uint8_t data2;
} midiEvent;

#endif


