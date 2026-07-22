/*
 * LED Effects Library
 * Collection of visual effects and animations for addressable LED strips
 */

#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <FastLED.h>

extern CRGB leds[];

// Global effect variables
static uint8_t gHue = 0;
static uint8_t gCurrentPatternNumber = 0;
static int scrollPosition = 0;

// ================================
// Basic Effects
// ================================

// Rainbow cycle effect
void effectRainbow() {
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
  gHue++;
}

// Solid color
void effectSolid(CRGB color) {
  fill_solid(leds, NUM_LEDS, color);
}

// Color wipe effect
void effectColorWipe(CRGB color) {
  static int pos = 0;
  leds[pos] = color;
  pos = (pos + 1) % NUM_LEDS;
}

// Theater chase effect
void effectTheaterChase(CRGB color) {
  static int q = 0;
  
  for (int i = 0; i < NUM_LEDS; i += 3) {
    leds[i + q] = color;
  }
  FastLED.show();
  
  delay(50);
  
  for (int i = 0; i < NUM_LEDS; i += 3) {
    leds[i + q] = CRGB::Black;
  }
  
  q = (q + 1) % 3;
}

// ================================
// Advanced Effects
// ================================

// Fire effect
void effectFire() {
  static byte heat[NUM_LEDS];
  
  // Cool down every cell a little
  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((55 * 10) / NUM_LEDS) + 2));
  }
  
  // Heat from each cell drifts up and diffuses a little
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
  
  // Randomly ignite new sparks near the bottom
  if (random8() < 120) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }
  
  // Convert heat to LED colors
  for (int j = 0; j < NUM_LEDS; j++) {
    CRGB color = HeatColor(heat[j]);
    leds[j] = color;
  }
}

// Confetti effect
void effectConfetti() {
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
  gHue++;
}

// Sinelon effect
void effectSinelon() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 192);
  gHue++;
}

// BPM effect
void effectBPM() {
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
  gHue++;
}

// Juggle effect
void effectJuggle() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

// ================================
// Text Effects (Simple 5x7 Font)
// ================================

// Simple 5x7 font for basic characters
const byte font5x7[][5] = {
  {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
  {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
  {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
  {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
  {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
  {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
  {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
  {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
  {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
  {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
  {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
  {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
  {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
  {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
  {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
  {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
  {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
  {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
  {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
  {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
  {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
  {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
  {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
  {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S
  {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
  {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
  {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
  {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X
  {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
  {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
  {0x00, 0x00, 0x00, 0x00, 0x00}  // Space
};

// Get character index in font array
int getCharIndex(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
  if (c >= 'a' && c <= 'z') return c - 'a' + 10;
  return 36; // Space
}

// Scrolling text effect (for linear strips)
void effectScrollText(String text) {
  fadeToBlackBy(leds, NUM_LEDS, 255); // Clear
  
  // Simple single-line scroll
  int textWidth = text.length() * 6; // 5 pixels + 1 space per character
  
  for (int i = 0; i < text.length(); i++) {
    int charPos = scrollPosition + (i * 6);
    
    if (charPos >= 0 && charPos < NUM_LEDS) {
      // For simple single-line display, just light up pixels
      int index = charPos % NUM_LEDS;
      if (index >= 0 && index < NUM_LEDS) {
        leds[index] = CHSV(gHue + (i * 10), 255, 255);
      }
    }
  }
  
  scrollPosition--;
  if (scrollPosition < -textWidth) {
    scrollPosition = NUM_LEDS;
  }
  
  gHue++;
}

// Matrix text scroll (for 2D matrix arrangements)
void effectMatrixScrollText(String text, int yPos) {
  #ifdef MATRIX_WIDTH
  // For matrix displays, draw actual characters
  fadeToBlackBy(leds, NUM_LEDS, 255);
  
  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    int charIndex = getCharIndex(c);
    int xPos = scrollPosition + (i * 6);
    
    // Draw character
    for (int col = 0; col < 5; col++) {
      int x = xPos + col;
      if (x >= 0 && x < MATRIX_WIDTH) {
        byte column = font5x7[charIndex][col];
        
        for (int row = 0; row < 7; row++) {
          if (column & (1 << row)) {
            int y = yPos + row;
            if (y >= 0 && y < MATRIX_HEIGHT) {
              int ledIndex = y * MATRIX_WIDTH + x;
              if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
                leds[ledIndex] = CHSV(gHue + (i * 10), 255, 255);
              }
            }
          }
        }
      }
    }
  }
  
  scrollPosition--;
  if (scrollPosition < -(int)(text.length() * 6)) {
    scrollPosition = MATRIX_WIDTH;
  }
  
  gHue++;
  #else
  // Fall back to simple scroll for non-matrix strips
  effectScrollText(text);
  #endif
}

// ================================
// Utility Functions
// ================================

// Flash all LEDs
void flashAll(CRGB color, int times, int duration) {
  for (int i = 0; i < times; i++) {
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
    delay(duration);
    FastLED.clear();
    FastLED.show();
    delay(duration);
  }
}

// Fade in effect
void fadeIn(CRGB color, int duration) {
  for (int brightness = 0; brightness < 255; brightness++) {
    fill_solid(leds, NUM_LEDS, color);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(duration / 255);
  }
}

// Fade out effect
void fadeOut(int duration) {
  for (int brightness = 255; brightness >= 0; brightness--) {
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(duration / 255);
  }
}

#endif // LED_EFFECTS_H
