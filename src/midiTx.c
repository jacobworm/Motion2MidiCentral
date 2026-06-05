
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
static void uart_cb(const struct device *dev, struct uart_event *evt,
                    void *user_data) { // Callback funktion for UART events.
                                       // Logger når en transmission er aborted.
  switch (evt->type) {

  case UART_TX_DONE:
    break;

  case UART_TX_ABORTED:
    printk("UART transmission aborted\n");
    break;

  case UART_RX_RDY:
    break;

  case UART_RX_BUF_REQUEST:
    break;

  case UART_RX_BUF_RELEASED:
    break;

  case UART_RX_DISABLED:
    break;

  case UART_RX_STOPPED:
    break;

  default:
    break;
  }
}

void MidiTransmitThreadFunct(void *que_ptr, void *p2, void *p3) {
  // Casting the void pointer to a message queue pointer
  struct k_msgq *midi_q = (struct k_msgq *)que_ptr;
  (void)p2;
  (void)p3;
  k_sleep(K_SECONDS(5)); // Give some time for other threads to initialize
  if (Midi_init() != 0) {
    return;
  }
  printk("UART device for midi: %s\n", uart_dev->name);

  int ret = 0;
  // ret = uart_callback_set(uart_dev, uart_cb,
  //                         NULL); // Set the UART callback function
  if (ret) {
    printk("Failed to set UART callback: %d\n", ret);
    return;
  }

  while (1) {
    // Debugging: Send en test MIDI Note On besked hvert sekund med polling
    // midi:
/*
    while (1) {
      printk("UART ready: %d\n", device_is_ready(uart_dev));
      uart_poll_out(uart_dev, 0x90);
      uart_poll_out(uart_dev, 60);
      uart_poll_out(uart_dev, 127);
      printk("Test MIDI Note On sent\n");
      k_sleep(K_SECONDS(1));
    }
*/
    midiEvent event;
    ret = k_msgq_get(midi_q, &event,
                     K_FOREVER); // polling the queue for a midiEvent, returns 0
                                 // if an event was received
    if (ret == 0) {              // on received midiEvent
      static uint8_t midi_msg[3];
      if (event.channel < 1 || event.channel > 16) {
        printk("Invalid MIDI channel: %d, gesture: other\n", event.channel);
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
      // transmission
      if (event.type == ProgramChange ||
          event.type == ChannelPressure) { // Program Change og Channel Pressure
                                           // messages har kun én databyte
        static uint8_t short_msg[2];
        for (int i = 0; i < 2; i++) {
          short_msg[i] = midi_msg[i];
          // uart_poll_out(uart_dev, midi_msg[i]);
        }
        int ret = uart_tx(uart_dev, short_msg, 2, SYS_FOREVER_US);
        int counter = 0;
        while (ret == -EBUSY) { // Hvis UART er optaget, vent og prøv igen
          counter++;
          k_sleep(K_MSEC(5)); // Vent 5 ms før næste forsøg
          ret = uart_tx(uart_dev, short_msg, 2, SYS_FOREVER_US);
          if (counter > 5) { // Hvis det stadig er optaget efter 5 forsøg, log
                             // og drop beskeden
            printk("UART busy after multiple attempts, dropping MIDI event\n");
            break;
          }
        }
      } else {
        int ret = uart_tx(
            uart_dev, midi_msg, 3,
            SYS_FOREVER_US); // Attempt to transmit MIDI message without waiting
        int counter = 0;
        if (ret < 0) {
          printk("uart_tx returned %d\n", ret);
        }

        while (ret == -EBUSY) {
          counter++;
          k_sleep(K_MSEC(5));

          ret = uart_tx(uart_dev, midi_msg, 3, SYS_FOREVER_US);

          printk("uart_tx retry %d returned %d\n", counter, ret);

          if (counter > 5) {
            printk("UART busy after multiple attempts\n");
            break;
          }
        }

        /*        while (ret == -EBUSY) { // Hvis UART er optaget, vent og prøv
           igen counter++; k_sleep(K_MSEC(5)); // Vent 5 ms før næste forsøg ret
           = uart_tx(uart_dev, midi_msg, 3, SYS_FOREVER_US); if (counter > 5) {
           // Hvis det stadig er optaget efter 5 forsøg, log
                                     // og drop beskeden
                    printk("UART busy after multiple attempts, dropping MIDI
           event\n"); break;
                  }
                }*/
      }
      printk(
          "MIDI event transmitted: Type=%d, Channel=%d, Data1=%d, Data2=%d\n",
          event.type, event.channel, event.data1, event.data2);
    }
  }
}