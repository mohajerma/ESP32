# NEMA 17 Wiring Guide

## Shared signal pinout

The sketch uses GPIO numbers. These defaults work on a common ESP8266 NodeMCU and ESP32 DevKit when the pins are not occupied by another peripheral.

| Function | ESP8266 NodeMCU | ESP32 DevKit | Driver pin |
|---|---:|---:|---|
| STEP pulse | GPIO5 / D1 | GPIO5 | STEP |
| Direction | GPIO4 / D2 | GPIO4 | DIR |
| Driver enable | GPIO16 / D0 | GPIO16 | EN / ENA |
| Minimum end stop | GPIO14 / D5 | GPIO14 | optional input |
| Maximum end stop | GPIO12 / D6 | GPIO12 | optional input |
| Ground | GND | GND | GND |

Change the constants at the top of `Stepper_Web_Controller.ino` if your board uses different pins. Avoid ESP8266 boot-strap pins GPIO0, GPIO2, and GPIO15 unless you understand their startup requirements. On ESP32, avoid pins reserved by your specific board, flash, PSRAM, or USB interface.

## A4988 / DRV8825 carrier

| Driver connection | Connect to |
|---|---|
| VDD | ESP 3.3 V logic supply |
| GND beside VDD | ESP GND |
| STEP | ESP STEP pin |
| DIR | ESP DIR pin |
| EN / ENA | ESP ENABLE pin; active low |
| VMOT | Positive motor supply, typically 8-35 V for A4988 |
| GND beside VMOT | Motor-supply negative and ESP GND |
| 1A, 1B, 2A, 2B | The two coils of the NEMA 17 |
| RESET and SLEEP | Tie high to VDD if unused, commonly bridge RESET to SLEEP |
| MS1/MS2/MS3 | Set microstepping; leave low for full-step on A4988 |

DRV8825 motor supply limits and microstep settings differ from A4988. Follow the carrier manufacturer's limits.

## Finding the motor coils

Use a multimeter in resistance/continuity mode with the motor disconnected. The two wires of one coil show low resistance; wires from different coils remain open. Connect one coil to `1A/1B` and the other to `2A/2B`. Reversing one coil pair changes direction; swapping only one wire in a pair causes vibration instead of rotation.

## End stops

End stops are disabled by default. To enable them:

1. Set `USE_LIMIT_SWITCHES` to `1`.
2. Wire each normally-closed switch between its GPIO and GND.
3. Use `LIMIT_ACTIVE_LOW 1`.
4. Recompile and test with the motor unloaded.

The minimum switch blocks negative motion and the maximum switch blocks positive motion. End stops are software inputs, not a certified safety system.

## Current limit

Set the driver's current limit before applying a sustained load. A4988 and DRV8825 adjustment formulas are carrier- and sense-resistor-dependent, so use the exact carrier documentation. Begin below the motor's rated current and increase carefully while monitoring temperature.
