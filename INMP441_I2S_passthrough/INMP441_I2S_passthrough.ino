#include <Arduino.h>
#include <driver/i2s.h>

// Use the same I2S port for RX (microphone) and TX (DAC)
#define I2S_NUM           (I2S_NUM_0)
#define I2S_BCK_PIN       14
#define I2S_WS_PIN        15
#define I2S_DATA_IN_PIN   32
#define I2S_DATA_OUT_PIN  22

#define SAMPLE_RATE       44100
#define I2S_BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_32BIT

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("INMP441 -> ESP32 -> I2S DAC passthrough");
  Serial.println("Using one I2S port for RX and TX on the ESP32");
  Serial.println();

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE,
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
    .mck_io_num = I2S_PIN_NO_CHANGE,
    .bck_io_num = I2S_BCK_PIN,
    .ws_io_num = I2S_WS_PIN,
    .data_out_num = I2S_DATA_OUT_PIN,
    .data_in_num = I2S_DATA_IN_PIN
  };

  Serial.print("Installing I2S driver...");
  esp_err_t err = i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf(" failed, error=%d\n", err);
    while (true) {
      delay(1000);
    }
  }
  Serial.println(" OK");

  Serial.print("Configuring I2S pins...");
  err = i2s_set_pin(I2S_NUM, &pin_config);
  if (err != ESP_OK) {
    Serial.printf(" failed, error=%d\n", err);
    while (true) {
      delay(1000);
    }
  }
  Serial.println(" OK");

  i2s_zero_dma_buffer(I2S_NUM);
  Serial.println("I2S ready.");
  Serial.println("Mic on SD -> 32, BCK -> 14, WS -> 15; DAC DIN -> 22");
}

void loop() {
  const int frames = 64;
  const int32_t buffer_frames = frames * 2; // stereo frame count
  static int32_t rx_buffer[buffer_frames];
  static int32_t tx_buffer[buffer_frames];

  size_t bytes_read = 0;
  size_t bytes_to_read = sizeof(rx_buffer);
  esp_err_t err = i2s_read(I2S_NUM, rx_buffer, bytes_to_read, &bytes_read, portMAX_DELAY);
  if (err != ESP_OK) {
    Serial.printf("i2s_read error %d\n", err);
    delay(100);
    return;
  }

  int samples = bytes_read / sizeof(int32_t);
  for (int i = 0; i < samples; i += 2) {
    int32_t sample = rx_buffer[i];
    tx_buffer[i] = sample;
    tx_buffer[i + 1] = sample;
  }

  size_t bytes_written = 0;
  err = i2s_write(I2S_NUM, tx_buffer, bytes_read, &bytes_written, portMAX_DELAY);
  if (err != ESP_OK) {
    Serial.printf("i2s_write error %d\n", err);
    delay(100);
    return;
  }
}
