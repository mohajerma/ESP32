# Quick Pinout Reference

## ESP8266 NodeMCU

| Board label | GPIO | Default use |
|---|---:|---|
| D1 | GPIO5 | STEP |
| D2 | GPIO4 | DIR |
| D0 | GPIO16 | ENABLE, active low |
| D5 | GPIO14 | MIN end stop, optional |
| D6 | GPIO12 | MAX end stop, optional |
| GND | - | Common ground |

## ESP32 DevKit V1

| Board label | GPIO | Default use |
|---|---:|---|
| GPIO5 | 5 | STEP |
| GPIO4 | 4 | DIR |
| GPIO16 | 16 | ENABLE, active low |
| GPIO14 | 14 | MIN end stop, optional |
| GPIO12 | 12 | MAX end stop, optional |
| GND | - | Common ground |

The signal pins are 3.3 V logic. Confirm that your driver carrier accepts 3.3 V HIGH levels. The motor supply is separate from the logic supply.
