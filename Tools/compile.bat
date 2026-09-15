C:\code\ESP32\Tools\arduino-cli.exe compile --fqbn esp32:esp32:esp32 .

C:\code\ESP32\Tools\arduino-cli.exe compile --fqbn esp32:esp32:esp32 --library C:\code\ESP32\arduino-audio-tools\src .

C:\code\ESP32\Tools\arduino-cli.exe compile --fqbn esp32:esp32:esp32 --library C:\code\ESP32\RDA5807\src --library C:\code\ESP32\arduino-audio-tools\src .

C:\code\ESP32\Tools\arduino-cli.exe compile --fqbn esp32:esp32:esp32:PartitionScheme=huge_app .


C:\code\ESP32\Tools\arduino-cli.exe upload -p COM3 --fqbn esp32:esp32:esp32 .

C:\code\ESP32\Tools\arduino-cli.exe upload -p COM3 --fqbn esp8266:esp8266:generic .


C:\code\ESP32\Tools\arduino-cli.exe upload -p COM3 --fqbn esp32:esp32:esp32:PartitionScheme=huge_app .

C:\code\ESP32\Tools\arduino-cli.exe monitor -p COM3 -c baudrate=115200