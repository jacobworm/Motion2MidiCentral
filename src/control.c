#include "control.h"
#include <zephyr/kernel.h>

gestureSetting gestureSettings[8] = {
    0}; // Array to hold settings for 8 gestures
int prev_gesture = 4;

int Control_init(void) {
  // Initialize control-related resources (none needed for now)
  return 0; // Return 0 on success
}

midiEvent createMidiEventFromGestureEvent(int gesture_event) {
  midiEvent event;
  event.type = gestureSettings[gesture_event].type;
  event.channel = gestureSettings[gesture_event].channel;
  if (event.type == ControlChange) {
    event.data1 = gestureSettings[gesture_event].controllerNr;
    event.data2 = gestureSettings[gesture_event].value;
  } else if (event.type == NoteOn || event.type == NoteOff) {
    event.data1 = gestureSettings[gesture_event].noteNr;
    event.data2 = gestureSettings[gesture_event].velocity;
  } else {
    event.data1 = gestureSettings[gesture_event].value;
    event.data2 = 0; // Assuming value is stored in data1
  }
  if (gesture_event == 4) { // 'Other' gesture skal ikke sende midi.
    event.channel = 17;     // set ulovlig channel for at ignorere i midiTx.
  }
  // Special handling for continuous gestures (5, 6 og 7) ved 'step out of'
  if ((prev_gesture == 5 && gesture_event != 5) ||
      (prev_gesture == 6 && gesture_event != 6) ||
      (prev_gesture == 7 && gesture_event != 7)) {
    event.channel = gestureSettings[prev_gesture].channel; // behold samme kanal
    event.type =
        gestureSettings[prev_gesture].type; // behold samme midi-besked-type
    if (event.type == ControlChange) {
      event.data1 = gestureSettings[prev_gesture].controllerNr;
      event.data2 =
          0; // Set value to 0 to indicate "off" for the control change
    }
    if (event.type == NoteOn) {
      event.data1 = gestureSettings[prev_gesture].noteNr;
      event.data2 = 0; // Set velocity to 0 to indicate "off" for the note
    }
  }
  prev_gesture = gesture_event;
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
  /*
  // definér gestureSettings, debug med toner:
  gestureSettings[0].noteNr = 60; // C4
  gestureSettings[1].noteNr = 60; // D4
  gestureSettings[1].velocity = 0;
  gestureSettings[2].noteNr = 64; // E4
  gestureSettings[3].noteNr = 64; // F4
  gestureSettings[3].velocity = 0;
  gestureSettings[4].noteNr = 67; // G4
  gestureSettings[5].noteNr = 67; // A4
  gestureSettings[5].velocity = 0;
  gestureSettings[6].noteNr = 71; // B4
  gestureSettings[7].noteNr = 71; // C5
  gestureSettings[7].velocity = 0;
  */
  
  // definér gestureSettings, med control change:
  gestureSettings[0].type = ControlChange;
  gestureSettings[0].controllerNr = 20;
  gestureSettings[0].value = 127;
  gestureSettings[1].type = ControlChange;
  gestureSettings[1].controllerNr = 21;
  gestureSettings[1].value = 127;
  gestureSettings[2].type = ControlChange;
  gestureSettings[2].controllerNr = 22;
  gestureSettings[2].value = 127;
  gestureSettings[3].type = ControlChange;
  gestureSettings[3].controllerNr = 23;
  gestureSettings[3].value = 127;
  gestureSettings[5].type = ControlChange;
  gestureSettings[5].controllerNr = 24;
  gestureSettings[5].value = 127;
  gestureSettings[6].type = ControlChange;
  gestureSettings[6].controllerNr = 25;
  gestureSettings[6].value = 127;
  gestureSettings[7].type = ControlChange;
  gestureSettings[7].controllerNr = 26;
  gestureSettings[7].value = 127;


  struct k_msgq *midi_q = (struct k_msgq *)midi_q_ptr;
  struct k_msgq *control_q = (struct k_msgq *)control_q_ptr;

  if (Control_init() != 0) {
    return;
  }
  while (1) {
    gestureEvent gesture;
    k_msgq_get(control_q, &gesture,
               K_FOREVER); // Wait for a gesture event from the control queue

    midiEvent event = createMidiEventFromGestureEvent((int)gesture);

    // dispatch event to midi queue
    k_msgq_put(midi_q, &event, K_NO_WAIT); // Send the MIDI event to the midi_q
  }
}