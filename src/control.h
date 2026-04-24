#ifndef CONTROL_H
#define CONTROL_H

#include "midiEvent.h"
#include "gestureEvent.h"
#include "midiTx.h"

typedef struct{
    uint8_t channel;
    uint8_t value;
    uint8_t controllerNr;
    uint8_t noteNr;
    uint8_t velocity;
    midiMsgType type; 
} gestureSetting;

int Control_init(void);
void ControlThreadFunct(void *p1, void *p2, void *p3);

#endif