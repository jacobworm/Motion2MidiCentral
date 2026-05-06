#include "control.h"
#include "midiEvent.h"
#include "gestureEvent.h"
#include <zephyr/kernel.h>
#include "BLE_central.h"

#define MIDI_TX_STACK_SIZE 1024
#define MIDI_TX_PRIORITY 7
#define CONTROL_STACK_SIZE 1024
#define CONTROL_PRIORITY 7
#define BLE_STACK_SIZE 2048
#define BLE_PRIORITY 7

// Static queue creation
K_MSGQ_DEFINE(midi_q, sizeof(midiEvent), 10, 4);

K_MSGQ_DEFINE(control_q, sizeof(gestureEvent), 10, 4);

// Static thread creation
K_THREAD_DEFINE(midiTxThread, MIDI_TX_STACK_SIZE, MidiTransmitThreadFunct,
                &midi_q, NULL, NULL, MIDI_TX_PRIORITY, 0, 0);

K_THREAD_DEFINE(controlThread, CONTROL_STACK_SIZE, ControlThreadFunct, &midi_q, &control_q,
                NULL, CONTROL_PRIORITY, 0, 0);

K_THREAD_DEFINE(bleThread, BLE_STACK_SIZE, BLE_ThreadFunct, &control_q, NULL, NULL, BLE_PRIORITY, 0, 0);

int main(void) { 
    return 0; 
}
