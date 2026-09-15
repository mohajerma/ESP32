/*
 * ESP32 External I2S DAC Audio Test
 *
 * This sketch generates a steady 440Hz sine tone and sends it to an external
 * I2S DAC such as the PCM5102, PCM5122, or similar.
 *
 * Hardware:
 *  - ESP32 GPIO14 --> DAC BCK (Bit Clock)
 *  - ESP32 GPIO15 --> DAC LCK / LRCK (Word Select)
 *  - ESP32 GPIO22 --> DAC DIN (Data Out)
 *  - ESP32 3.3V    --> DAC VCC
 *  - ESP32 GND     --> DAC GND
 *
 * If your DAC board requires an MCLK input, make sure it is either not needed
 * or provided by the board. Many boards generate MCLK internally.
 */

#include <driver/i2s.h>
#include <math.h>

#define I2S_NUM         (I2S_NUM_0)
#define I2S_BCK_PIN     14
#define I2S_WS_PIN      15
#define I2S_DATA_PIN    22

#define SAMPLE_RATE     44100
#define TONE_FREQUENCY  440
#define AMPLITUDE_24    4000000

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("========================================");
  Serial.println("ESP32 External I2S DAC Audio Test");
  Serial.println("========================================");
  Serial.println("Hardware Setup:");
  Serial.println("  ESP32 GPIO14 --> DAC BCK");
  Serial.println("  ESP32 GPIO15 --> DAC LCK / LRCK");
  Serial.println("  ESP32 GPIO22 --> DAC DIN");
  Serial.println("  ESP32 3.3V    --> DAC VCC");
  Serial.println("  ESP32 GND     --> DAC GND");
  Serial.println("  Connect headphones or amplifier to DAC OUTL/OUTR");
  Serial.println();

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK_PIN,
    .ws_io_num = I2S_WS_PIN,
    .data_out_num = I2S_DATA_PIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  Serial.print("Installing I2S driver... ");
  esp_err_t err = i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("FAILED! Error: %d\n", err);
    while (true) {
      delay(1000);
    }
  }
  Serial.println("OK");

  Serial.print("Configuring I2S pins... ");
  err = i2s_set_pin(I2S_NUM, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("FAILED! Error: %d\n", err);
    while (true) {
      delay(1000);
    }
  }
  Serial.println("OK");

  i2s_zero_dma_buffer(I2S_NUM);

  Serial.println("\n========================================");
  Serial.println("✓ I2S initialized successfully");
  Serial.println("========================================");
  Serial.println("Generating a 440Hz sine tone on the external DAC...");
  Serial.println("If you hear nothing, verify wiring and power to the DAC board.");
  Serial.println();
}

void loop() {
  static uint32_t sample_index = 0;
  const int samples_per_frame = 64;
  int32_t buffer[samples_per_frame * 2];

  for (int i = 0; i < samples_per_frame; ++i) {
    float phase = 2.0f * PI * TONE_FREQUENCY * ((float)sample_index / SAMPLE_RATE);
    int32_t sample24 = (int32_t)(AMPLITUDE_24 * sinf(phase));
    // Left-align 24-bit into 32-bit container (MSB-first I2S)
    sample24 <<= 8;
    buffer[i * 2] = sample24;
    buffer[i * 2 + 1] = sample24;
    sample_index++;
  }

  size_t bytes_written = 0;
  i2s_write(I2S_NUM, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);

  static unsigned long last_print = 0;
  if (millis() - last_print > 5000) {
    Serial.printf("Tone playing... %lu samples written\n", sample_index);
    last_print = millis();
  }
}
