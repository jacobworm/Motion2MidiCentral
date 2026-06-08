# Motion2Midi – Central Unit

Dette repository indeholder kildekode og konfiguration til **Motion2Midi central unit**, udviklet i Zephyr RTOS til Nordic Semiconductors nRF7002DK.

Nedenfor gives et kort overblik over, hvad mapperne og filerne indeholder.

## `src/`
Vores egne implementeringsfiler – selve applikationen:

- `main.c` – opsætning af tråde og message queues.
- `BLE_central.c` / `.h` – Bluetooth-modul. Scanner efter peripheral unit, opretter forbindelse og modtager gesture-data.
- `BLE_Simulator.c` / `.h` – simulering af BLE-input, brugt til test uden fysisk peripheral unit.
- `control.c` / `.h` – tolker gesture-events og sammensætter midi-events ud fra state.
- `midiTx.c` / `.h` – omsætter midi-events til MIDI-messages og sender dem via UART.
- `gestureEvent.h` / `midiEvent.h` – datastrukturer brugt til kommunikation mellem modulerne.

## `prj.conf` & `sysbuild.conf`
Nogle af vores konfigurationsindstillinger til Zephyr-projektet:

- `prj.conf` – slår de nødvendige features til, bl.a. Bluetooth som central med GATT-client samt UART async API til MIDI-transmission.
- `sysbuild.conf` – konfiguration på tværs af images, her opsætning af network core (HCI IPC) på den dual-core nRF5340.

## `boards/`
Board-specifik konfiguration i form af et devicetree-overlay til nRF7002DK. Her sættes UART1 op til MIDI-baudraten (31250), og ubrugte peripherals (fx i2c1) slås fra.

## `Reference/`
Kopier af Nordics oprindelige devicetree- og defconfig-filer for nRF5340/nRF7002DK. Disse er **ikke** ændret af os, men er medtaget som reference, så det er muligt at slå boardets standardopsætning op.

## `CMakeLists.txt`
Byggekonfiguration for projektet.
