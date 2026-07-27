# 📐 LED Matrix Coordinate System Guide

Understanding how rows, columns, and LED indices work in your matrix display.

---

## 🎯 Basic Concept

When you arrange LEDs in a matrix (grid), you need to map 2D coordinates (row, column) to 1D LED indices (0 to NUM_LEDS-1).

### Key Definitions

In your [config.h](config.h):
```cpp
#define NUM_LEDS 60        // Total number of LEDs (calculated)
#define MATRIX_WIDTH 10    // Number of columns
#define MATRIX_HEIGHT 6    // Number of rows
```

**Important relationship:** 
```
NUM_LEDS = MATRIX_WIDTH × MATRIX_HEIGHT
60 LEDs = 10 columns × 6 rows
```

💡 **Tip**: When using the web interface, you only need to set width and height. The total LED count is automatically calculated!

---

## 📊 Coordinate System

### Standard Layout (Horizontal Serpentine/Zigzag)

This is the most common LED strip arrangement when folded into a matrix:

```
Row 0:  [0]  [1]  [2]  [3]  [4]  [5]  [6]  [7]  [8]  [9]  →
Row 1:  [19] [18] [17] [16] [15] [14] [13] [12] [11] [10] ←
Row 2:  [20] [21] [22] [23] [24] [25] [26] [27] [28] [29] →
Row 3:  [39] [38] [37] [36] [35] [34] [33] [32] [31] [30] ←
Row 4:  [40] [41] [42] [43] [44] [45] [46] [47] [48] [49] →
Row 5:  [59] [58] [57] [56] [55] [54] [53] [52] [51] [50] ←
```

**Pattern:** 
- **Even rows** (0, 2, 4): Left to right →
- **Odd rows** (1, 3, 5): Right to left ←

### Alternative Layout (Horizontal Straight)

If you cut the strip and reconnect for straight wiring:

```
Row 0:  [0]  [1]  [2]  [3]  [4]  [5]  [6]  [7]  [8]  [9]  →
Row 1:  [10] [11] [12] [13] [14] [15] [16] [17] [18] [19] →
Row 2:  [20] [21] [22] [23] [24] [25] [26] [27] [28] [29] →
Row 3:  [30] [31] [32] [33] [34] [35] [36] [37] [38] [39] →
Row 4:  [40] [41] [42] [43] [44] [45] [46] [47] [48] [49] →
Row 5:  [50] [51] [52] [53] [54] [55] [56] [57] [58] [59] →
```

**Pattern:** All rows go left to right →

---

## 🧮 Coordinate Calculations

### Formula: From (Row, Column) to LED Index

#### For Zigzag Layout (Most Common):
```cpp
int xyToIndex(int x, int y) {
  if (y % 2 == 0) {
    // Even rows: left to right
    return y * MATRIX_WIDTH + x;
  } else {
    // Odd rows: right to left
    return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
  }
}
```

**Examples:**
- (0, 0) → LED 0
- (9, 0) → LED 9
- (0, 1) → LED 19
- (9, 1) → LED 10
- (5, 3) → LED 34

#### For Straight Layout:
```cpp
int xyToIndex(int x, int y) {
  return y * MATRIX_WIDTH + x;
}
```

**Examples:**
- (0, 0) → LED 0
- (9, 0) → LED 9
- (0, 1) → LED 10
- (9, 1) → LED 19
- (5, 3) → LED 35

### Formula: From LED Index to (Row, Column)

#### For Any Layout:
```cpp
void indexToXY(int index, int &x, int &y) {
  y = index / MATRIX_WIDTH;
  
  if (y % 2 == 0) {
    // Even row: left to right
    x = index % MATRIX_WIDTH;
  } else {
    // Odd row: right to left (for zigzag)
    x = MATRIX_WIDTH - 1 - (index % MATRIX_WIDTH);
  }
}
```

**Examples:**
- LED 0 → (0, 0)
- LED 9 → (9, 0)
- LED 10 → (9, 1) for zigzag, (0, 1) for straight
- LED 25 → (5, 2)
- LED 59 → (0, 5) for zigzag, (9, 5) for straight

---

## 🎨 Practical Examples

### Example 1: Light Up Specific Pixel

```cpp
void setPixel(int x, int y, CRGB color) {
  if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
    int index = xyToIndex(x, y);
    leds[index] = color;
  }
}

// Usage
setPixel(5, 3, CRGB::Red);  // Light pixel at column 5, row 3
```

### Example 2: Draw Horizontal Line

```cpp
void drawHLine(int y, int x1, int x2, CRGB color) {
  for (int x = x1; x <= x2; x++) {
    setPixel(x, y, color);
  }
}

// Draw line across row 2, from column 2 to 7
drawHLine(2, 2, 7, CRGB::Blue);
```

### Example 3: Draw Vertical Line

```cpp
void drawVLine(int x, int y1, int y2, CRGB color) {
  for (int y = y1; y <= y2; y++) {
    setPixel(x, y, color);
  }
}

// Draw line down column 5, from row 1 to 4
drawVLine(5, 1, 4, CRGB::Green);
```

### Example 4: Draw Rectangle

```cpp
void drawRect(int x1, int y1, int x2, int y2, CRGB color) {
  // Top and bottom
  drawHLine(y1, x1, x2, color);
  drawHLine(y2, x1, x2, color);
  
  // Left and right
  drawVLine(x1, y1, y2, color);
  drawVLine(x2, y1, y2, color);
}

// Draw rectangle from (2,1) to (7,4)
drawRect(2, 1, 7, 4, CRGB::Yellow);
```

### Example 5: Display Character (5x7 Font)

```cpp
void drawChar(char c, int xPos, int yPos, CRGB color) {
  int charIndex = getCharIndex(c);
  
  for (int col = 0; col < 5; col++) {
    byte column = font5x7[charIndex][col];
    
    for (int row = 0; row < 7; row++) {
      if (column & (1 << row)) {
        setPixel(xPos + col, yPos + row, color);
      }
    }
  }
}

// Display 'A' at position (2, 0)
drawChar('A', 2, 0, CRGB::White);
```

---

## 🔧 Configuration in Your Project

### Current Settings

Your project currently uses these values in [config.h](config.h):

```cpp
#define MATRIX_WIDTH 10
#define MATRIX_HEIGHT 6
#define MATRIX_TYPE VERTICAL_ZIGZAG
```

### How to Change Matrix Size

**Via Web Interface** (Recommended!):
1. Connect to your LED Matrix web panel
2. Scroll to "Matrix Configuration" section
3. Enter Width and Height values
4. Total LEDs automatically calculated and displayed
5. Click "Apply"
6. Restart ESP32 for changes to take effect

**Via config.h** (Traditional method):
1. Open `config.h`
2. Modify these lines:
   ```cpp
   #define MATRIX_WIDTH 12   // Change to your width
   #define MATRIX_HEIGHT 8   // Change to your height
   ```
3. Update `NUM_LEDS = MATRIX_WIDTH × MATRIX_HEIGHT` (or let it calculate automatically)
4. Upload the modified code to ESP32

---

## 📝 Common Matrix Sizes

| LEDs | Common Layouts | Aspect Ratio |
|------|----------------|--------------|
| 60   | 10×6, 12×5, 15×4, 20×3 | Various |
| 64   | 8×8 | Square ⬜ |
| 100  | 10×10 | Square ⬜ |
| 144  | 12×12, 16×9 | Square/16:9 |
| 256  | 16×16 | Square ⬜ |
| 300  | 30×10, 25×12 | Wide screen |

### Choosing Matrix Dimensions

**For Text Display:**
- Minimum height: 7 rows (for 5×7 font)
- Recommended: 8-10 rows (allows spacing)

**For Graphics:**
- Square matrices (8×8, 16×16) work best for icons
- Wide matrices (30×10) work best for scrolling text

**For Effects:**
- Any size works
- Larger = more impressive
- Consider power requirements

---

## 🐛 Troubleshooting

### My matrix displays scrambled patterns
**Solution:** Check if you're using the correct matrix type (zigzag vs straight)

### Text or graphics appear mirrored
**Solution:** Your strip might be wired opposite to expected direction

### Only first row lights up
**Solution:** Check if MATRIX_WIDTH and NUM_LEDS match your actual strip

### Coordinates don't match expected positions
**Solution:** Verify your physical strip arrangement matches your code configuration

---

## 📚 Code Reference

The matrix scrolling text code is in [led_effects.h](led_effects.h):
```cpp
void effectMatrixScrollText(String text, int yPos) {
  // ...
  int ledIndex = y * MATRIX_WIDTH + x;
  // ...
}
```

Key line: `int ledIndex = y * MATRIX_WIDTH + x;`

This assumes a **zigzag pattern**. Modify this formula if your strip is wired differently.

---

## 💡 Tips

1. **Test your layout**: Use this simple test pattern to verify your matrix wiring:
   ```cpp
   // Light up corners
   leds[0] = CRGB::Red;                                // Top-left
   leds[MATRIX_WIDTH - 1] = CRGB::Green;               // Top-right
   leds[NUM_LEDS - MATRIX_WIDTH] = CRGB::Blue;         // Bottom-left
   leds[NUM_LEDS - 1] = CRGB::Yellow;                  // Bottom-right
   ```

2. **Measure your strip**: Count the actual LEDs and verify it matches NUM_LEDS

3. **Document your wiring**: Take a photo of how you folded/arranged your strip

4. **Start simple**: Test with solid colors before trying complex patterns

5. **Use helper functions**: The `setPixel()` function shown above makes matrix code much cleaner
