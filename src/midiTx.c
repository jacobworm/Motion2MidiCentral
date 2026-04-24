
#include "midiTx.h"
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

static const struct device *uart_dev;

int Midi_init(void) {
  uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart1));
  if (!device_is_ready(uart_dev)) {
    printk("UART device not ready\n");
    return -1; // Return -1 if UART device is not ready
  }
  return 0;
}

void MidiTransmitThreadFunct(void *que_ptr, void *p2, void *p3) {
    // Casting the void pointer to a message queue pointer
    struct k_msgq *midi_q = (struct k_msgq *)que_ptr;
    (void)p2;
    (void)p3;

  if (Midi_init() != 0) {
    return;
  }

  while (1) {
    /*// Waiting for que simulated by wait
    k_sleep(K_MSEC(1000)); // Simulate waiting for a midi_q event to be ready
    midiEvent event = {NoteOn, 1, 60, 100}; // Simulated MIDI event for testing*/
    int ret = 0;
    midiEvent event;
    ret = k_msgq_get(midi_q, &event, K_NO_WAIT); // polling the queue for a midiEvent, returns 0 if an event was received
    if(ret==0){ //on received midiEvent
        uint8_t midi_msg[3];
        if (event.channel < 1 || event.channel > 16) {
        printk("Invalid MIDI channel: %d\n", event.channel);
        continue;
        }
        if (event.data1 > 127 || event.data2 > 127) {
        printk("Data bytes must be in the range 0-127\n");
        continue;
        }
        if (event.type == ControlChange && event.data1 > 119) {
        printk("Control Change controller number must be in the range 0-119\n");
        continue;
        }
        midi_msg[0] = (event.type << 4) |
                    ((event.channel - 1) & 0x0F); // Combine type and channel
        midi_msg[1] = event.data1;
        midi_msg[2] = event.data2;
        unsigned int key =
            irq_lock(); // Disable interrupts to ensure atomic transmission
        if (event.type == ProgramChange ||
            event.type == ChannelPressure) { // Program Change and Channel Pressure
                                            // messages only have one data byte
        for (int i = 0; i < 2; i++) {
            uart_poll_out(uart_dev, midi_msg[i]);
        }
        } else {
        for (int i = 0; i < 3; i++) {
            uart_poll_out(uart_dev, midi_msg[i]);
        }
        }
        irq_unlock(key);
        printk("MIDI event transmitted: Type=%d, Channel=%d, Data1=%d, Data2=%d\n",
            event.type, event.channel, event.data1, event.data2);
        }
    }    
}
