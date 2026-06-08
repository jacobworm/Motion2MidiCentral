# Motion2Midi_central

Det her repo indeholder kildekode og konfiguration til projektet.

Nedenfor er et kort overbilk over hvad projektet indeholder.

## `src/` - vores implementerings kode

- `main.c`
- `BLE_central.c` / `.h` 
- `BLE_Simulator.c` / `.h` 
- `control.c` / `.h` 
- `midiTx.c` / `.h` 
- `gestureEvent.h` / `midiEvent.h`

## `prj.conf` & `sysbuild.conf`
Nogle af vores konfigurationsfiler til projeket:
– Her i aktivers nogle af de features som systemet skal bruge

- `prj.conf` 
- `sysbuild.conf` 

## `boards/`
Devicetree overlay (ændringer til device tree. UART)

## `Reference/`
Referencer til zephyr devicetree filer som er inkluderet i applikationen.


