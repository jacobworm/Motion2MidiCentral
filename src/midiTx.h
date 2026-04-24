#ifndef MIDITX_H
#define MIDITX_H

#include "midiEvent.h"

int Midi_init(void);
void MidiTransmitThreadFunct(void *p1, void *p2, void *p3);

#endif