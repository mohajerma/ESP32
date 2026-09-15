# INMP441 I2S Passthrough

This Arduino sketch runs on an ESP32 and uses the same I2S peripheral for both the INMP441 microphone input and an external I2S DAC output.

## Wiring

- INMP441 BCLK -> ESP32 GPIO14
- INMP441 LRCL / WS -> ESP32 GPIO15
- INMP441 SD -> ESP32 GPIO32
- INMP441 VCC -> ESP32 3.3V
- INMP441 GND -> ESP32 GND

- DAC BCK -> ESP32 GPIO14
- DAC LRCK / WS -> ESP32 GPIO15
- DAC DIN -> ESP32 GPIO22
- DAC VCC -> ESP32 3.3V
- DAC GND -> ESP32 GND

> The sketch configures `I2S_NUM_0` with both RX and TX enabled. The microphone is read from `data_in_num` and the DAC is written to `data_out_num`.

## Notes

- The INMP441 is a mono microphone; this sketch duplicates the captured channel to both left/right output channels.
- If your DAC module requires MCLK, provide it separately or choose a DAC board that does not need MCLK.
- If you change the GPIO pins, update the pin definitions in `INMP441_I2S_passthrough.ino`.

## Build

Use the Arduino CLI or Arduino IDE with an ESP32 board package installed.

Example Arduino CLI command:

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 INMP441_I2S_passthrough
```
