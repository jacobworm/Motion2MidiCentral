#include <zephyr/kernel.h>
#include "control.h"

gestureSetting gestureSettings[8] = {0}; // Array to hold settings for 8 gestures

int Control_init(void) {
  // Initialize control-related resources (none needed for now)
  return 0; // Return 0 on success
}

midiEvent createMidiEventFromBluetooth(int bluetooth_event) {    
    midiEvent event;
    event.type = gestureSettings[bluetooth_event].type;
    event.channel = gestureSettings[bluetooth_event].channel;
    if(event.type == ControlChange) {
        event.data1 = gestureSettings[bluetooth_event].controllerNr;
        event.data2 = gestureSettings[bluetooth_event].value;
    }
    if(event.type == NoteOn || event.type == NoteOff) {
        event.data1 = gestureSettings[bluetooth_event].noteNr;
        event.data2 = gestureSettings[bluetooth_event].velocity;
    }
    else {
        event.data1 = gestureSettings[bluetooth_event].value;
        event.data2 = 0; // Assuming value is stored in data1
    }

    // For testing: static output c-note
    event.channel = 1;
    event.type = NoteOn;
    event.data1 = 60;
    event.data2 = 100;
    return event;
}

void gesture_setting_default(gestureSetting *setting) {
    setting->channel = 1;
    setting->value = 0;
    setting->controllerNr = 0;
    setting->noteNr = 60;
    setting->velocity = 100;
    setting->type = NoteOn;
}

void ControlThreadFunct(void *midi_q_ptr, void *control_q_ptr, void *p3) {
    for (int i = 0; i < 8; i++) {
        gesture_setting_default(&gestureSettings[i]);
    };
    (void)p3;

    struct k_msgq *midi_q = (struct k_msgq *)midi_q_ptr;
    struct k_msgq *control_q = (struct k_msgq *)control_q_ptr;
    
    if (Control_init() != 0) {
        return;
    }
    while (1) {
    // Waiting for bluetooth queue simulated by wait
    k_sleep(K_MSEC(1000)); // Simulate waiting for a bluetooth_q event to be ready
    int bluetooth_event = 1; // Simulated Bluetooth event for testing

    
    midiEvent event = createMidiEventFromBluetooth(bluetooth_event);

    //dispatch event to midi queue    
    k_msgq_put(midi_q, &event, K_NO_WAIT); // Send the MIDI event to the midi_q    
    }
}