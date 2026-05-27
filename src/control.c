#include <zephyr/kernel.h>
#include "control.h"

gestureSetting gestureSettings[8] = {0}; // Array to hold settings for 8 gestures

int Control_init(void) {
  // Initialize control-related resources (none needed for now)
  return 0; // Return 0 on success
}

midiEvent createMidiEventFromGestureEvent(int gesture_event) {    
    midiEvent event;
    event.type = gestureSettings[gesture_event].type;
    event.channel = gestureSettings[gesture_event].channel;
    if(event.type == ControlChange) {
        event.data1 = gestureSettings[gesture_event].controllerNr;
        event.data2 = gestureSettings[gesture_event].value;
    }
    if(event.type == NoteOn || event.type == NoteOff) {
        event.data1 = gestureSettings[gesture_event].noteNr;
        event.data2 = gestureSettings[gesture_event].velocity;
    }
    else {
        event.data1 = gestureSettings[gesture_event].value;
        event.data2 = 0; // Assuming value is stored in data1
    }

    // For testing: static output c-note
    /*event.channel = 1;
    event.type = NoteOn;
    event.data1 = 60;
    event.data2 = 100;*/
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

    // TEST KODE, bare for at se at BLE data rent faktisk ændre på nogle settings
    gestureSettings[0].noteNr = 60;  // C4
    gestureSettings[1].noteNr = 60;  // D4
    gestureSettings[1].velocity = 0;
    gestureSettings[2].noteNr = 64;  // E4
    gestureSettings[3].noteNr = 64;  // F4
    gestureSettings[3].velocity = 0;
    gestureSettings[4].noteNr = 67;  // G4
    gestureSettings[5].noteNr = 67;  // A4
    gestureSettings[5].velocity = 0;
    gestureSettings[6].noteNr = 71;  // B4
    gestureSettings[7].noteNr = 71;  // C5
    gestureSettings[7].velocity = 0;

    struct k_msgq *midi_q = (struct k_msgq *)midi_q_ptr;
    struct k_msgq *control_q = (struct k_msgq *)control_q_ptr;
    
    if (Control_init() != 0) {
        return;
    }
    while (1) {
        gestureEvent gesture;
        k_msgq_get(control_q, &gesture, K_FOREVER); // Wait for a gesture event from the control queue
        
        midiEvent event = createMidiEventFromGestureEvent((int)gesture);

        //dispatch event to midi queue    
        k_msgq_put(midi_q, &event, K_NO_WAIT); // Send the MIDI event to the midi_q    
    }
}