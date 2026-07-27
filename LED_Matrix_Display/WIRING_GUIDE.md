# 🔌 LED Matrix Display - Wiring & Power Supply Guide

Complete guide for connecting your ESP32, LED strip, and power supply safely.

## 📋 Components Required

### Essential Components
- **ESP32 Development Board** (ESP32-WROOM-32 or similar)
- **WS2812B Addressable LED Strip** (60 LEDs or your desired count)
- **5V Power Supply** (recommended: 5V 4-5A for 60 LEDs)
- **Jumper Wires** (preferably 18-20 AWG for power)
- **470Ω Resistor** (for data line protection)

### Recommended Components
- **1000µF Capacitor** (16V or higher, electrolytic)
- **Logic Level Shifter** (3.3V to 5V) - optional but improves reliability
- **470Ω Resistor** (for data line protection) - optional, see note below
- **Heat shrink tubing** or electrical tape for insulation
- **Screw terminal blocks** for secure power connections

**⚠️ Note on Data Line Resistor:**
- The 470Ω resistor is traditionally recommended for data line protection
- However, **it may cause signal issues** with some WS2812B strips
- **For short wire runs (< 1m)**, the resistor can be omitted
- If LEDs don't work with resistor, try removing it
- For long wire runs (> 1m), use a logic level shifter instead

---

## 🎯 Complete Wiring Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    5V Power Supply (5V 5A)                  │
│                         with terminals                       │
└────────────┬───────────────────────────┬────────────────────┘
             │                           │
          +5V│                        GND│
             │                           │
    ┌────────┴────────┐         ┌────────┴────────┐
    │                 │         │                 │
    │  [1000µF Cap]   │         │                 │
    │   (+)    (-)    │         │                 │
    └────┬──────┬─────┘         │                 │
         │      └───────────────┼─────────┐       │
         │                      │         │       │
         │                      │         │       │
    ┌────┴────────────────┐     │    ┌────┴───────┴──────┐
    │   ESP32 Board       │     │    │   LED Strip       │
    │                     │     │    │   WS2812B         │
    │  VIN ◄──────────────┘     │    │                   │
    │  GND ◄────────────────────┴───►│ GND               │
    │                                │ 5V/VCC ◄──────────┘
    │  GPIO 5 ──[470Ω]──────────────►│ DI (Data In)      │
    │                                │                   │
    └────────────────────────────────┴───────────────────┘
```

---

## 🔧 Step-by-Step Wiring Instructions

### Step 1: Prepare Components

1. **Identify LED strip connections:**
   - Find the **input end** (usually marked with arrows pointing away from it)
   - Locate three wires/pads: **5V (or VCC)**, **GND**, and **DI (or Data In)**

2. **Identify ESP32 pins:**
   - **VIN** or **5V** pin (usually near USB connector)
   - **GND** pin (multiple available)
   - **GPIO 5** pin (configurable in software)

3. **Check power supply:**
   - Verify it outputs **5V DC**
   - Confirm it's rated for at least **4-5 Amps**
   - Identify positive (+) and negative (-) terminals

### Step 2: Add Capacitor (Recommended)

**Purpose:** Stabilizes power and protects against voltage spikes

1. Connect **1000µF capacitor** across power supply terminals
2. **Positive leg (longer)** → Connect to +5V terminal
3. **Negative leg (shorter, marked with stripe)** → Connect to GND terminal
4. Keep capacitor close to LED strip power connection

### Step 3: Power Connections

⚠️ **CRITICAL: Double-check polarity before connecting power!**

**Connect 5V Rail:**
```
Power Supply (+5V) ──┬──► LED Strip (5V/VCC)
                     └──► ESP32 Board (VIN pin)
```

**Connect GND Rail:**
```
Power Supply (GND) ──┬──► LED Strip (GND)
                     └──► ESP32 Board (GND pin)
```

**Best Practices:**
- Use **thick wires** (18-20 AWG) for power connections
- Keep power wires as **short as possible**
- Twist power wires together to reduce interference
- Use **screw terminals** or solder for secure connections

### Step 4: Data Line Connection

**Option A: Direct Connection (Recommended for short distances)**
1. Connect ESP32 **GPIO 14** pin directly to LED strip **DI (Data In)** pin
2. Keep data wire away from power wires to reduce interference
3. Works well for wire lengths < 1 meter

**Option B: With Resistor (Optional protection)**
1. Connect **470Ω resistor** to ESP32 **GPIO 14** pin
2. Connect other end of resistor to LED strip **DI (Data In)** pin
3. ⚠️ If LEDs don't light up, remove resistor and try Option A

**Option C: Logic Level Shifter (Best for long distances)**
```
ESP32 GPIO 14 ──► Level Shifter (3.3V → 5V) ──► LED Strip DI
```
Use for wire runs > 1 meter or for maximum reliability

### Step 5: Final Checks Before Power-On

✅ **Safety Checklist:**
- [ ] All GND connections are secure and common
- [ ] 5V connections are to correct terminals (not reversed)
- [ ] No exposed wire touching other connections
- [ ] Capacitor polarity is correct
- [ ] Resistor is in data line
- [ ] Power supply voltage is 5V (measure with multimeter if possible)
- [ ] Power supply is NOT plugged in yet

---

## ⚡ Power Supply Options

### Option 1: Dedicated 5V Power Supply (Best)

**Recommended specifications:**
- **Voltage:** 5V DC (regulated)
- **Current:** 4-5A minimum for 60 LEDs (higher for more LEDs)
- **Connection:** Screw terminals or barrel jack
- **Cost:** $8-15

**Where to use:**
- Permanent installations
- Maximum brightness needed OR USB breakout board

**⚠️ Important: USB Charger Triggering**

Many USB chargers (especially USB-C) need to detect a proper load before outputting power:

**USB-A Chargers:**
- Usually trigger automatically when connected
- If not working, try connecting a small load (100Ω resistor) between +5V and GND temporarily

**USB-C Chargers (PD/Quick Charge):**
- Require 5.1kΩ pull-down resistors on CC (Configuration Channel) pins:
  ```
  USB-C CC1 pin ──[5.1kΩ resistor]── GND
  USB-C CC2 pin ──[5.1kΩ resistor]── GND
  ```
- **Easier solution**: Use a USB-C breakout board (like Adafruit #4090) that includes these resistors
- Or use a USB-C to USB-A adapter (which handles this automatically)

**Testing your USB charger:**
1. Use a multimeter to check voltage between +5V and GND
2. Should read 4.75V - 5.25V when active
3. If 0V, charger needs triggering (see above solutions)
- Large LED counts (100+)

### Option 2: USB Charger (Limited Use)

**Requirements:**
- High-current USB charger (3A minimum)
- USB cable you can cut/modify

**Limitations:**
- ⚠️ Only suitable for LOW brightness (< 50%)
- ⚠️ Limited to small LED counts (< 30 LEDs)
- ⚠️ May cause flickering or brownouts

**Wiring:**
1. Cut USB cable and strip wires
2. Identify Red (+5V) and Black (GND) wires
3. Connect to LED strip and ESP32 as shown in diagram

### Option 3: Computer PSU (Advanced)

**For large installations:**
- Use old ATX computer power supply
- Yellow wire = +12V (not used)
- Red wire = +5V ✓
- Black wire = GND ✓
- Bridge Green and Black to turn on PSU

---

## 🛡️ Safety & Protection

### Overcurrent Protection

**Add a fuse (optional but recommended):**
```
Power Supply (+5V) ──[5A Fuse]──► LED Strip & ESP32
```

### Prevent Reverse Polarity

- Use **diode** on power input (causes small voltage drop)
- Or use **polarized connectors** that can't be reversed

### Heat Management

- ESP32 may get warm during WiFi/BLE operation
- LEDs generate heat at high brightness
- Ensure adequate ventilation
- Consider heat sinks for continuous operation

---

## 🔍 Troubleshooting Power Issues

### LEDs flickering or showing random colors
**Causes:**
- Insufficient power supply current
- Voltage drop due to thin wires
- Loose connections

**Solutions:**
- Upgrade to higher amperage power supply
- Use thicker wire (lower AWG number)
- Add power injection for long LED strips

### ESP32 keeps resetting
**Causes:**
- Power supply voltage dropping below 4.5V
- Shared power with LEDs causing brownouts

**Solutions:**
- Use separate power supply for ESP32
- Add larger capacitor (2200µF or higher)
- Reduce LED brightness in software

### Some LEDs not working
**Causes:**
- Insufficient power reaching far LEDs
- Damaged LED in strip

**Solutions:**
- Power injection: Add 5V/GND at middle or end of strip
- Replace damaged section
- Check all solder joints

### No LEDs lighting up
**Causes:**
- Reversed power polarity (⚠️ may damage LEDs!)
- No data signal
- Wrong GPIO pin in configuration

**Solutions:**
- Verify 5V and GND are correct
- Check data line resistor and connection
- Verify GPIO 5 is configured in config.h
- Test with multimeter

---

## 📐 Wire Gauge Recommendations

### Power Wires (5V and GND)

| LED Count | Wire Length | Recommended AWG |
|-----------|-------------|-----------------|
| 1-30      | < 3 feet    | 22 AWG          |
| 30-60     | < 3 feet    | 20 AWG          |
| 60-100    | < 3 feet    | 18 AWG          |
| 100+      | < 3 feet    | 16 AWG          |

**Note:** For longer distances, use thicker wire or add power injection

### Data Wire (GPIO to DI)
- **22-24 AWG** is fine
- Keep as short as possible (< 6 feet)
- Use shielded cable for runs over 3 feet

---

## 💡 Advanced: Power Injection

For LED strips with 100+ LEDs, inject power at multiple points:

```
Power Supply
     │
     ├─► Strip Start (5V, GND)
     │
     ├─► Strip Middle (5V, GND)  ← Power Injection
     │
     └─► Strip End (5V, GND)     ← Power Injection

ESP32 GPIO 5 ──► Strip Start (DI only)
```

**Benefits:**
- Even brightness across entire strip
- Prevents voltage drop
- Reduces wire heating

---

## 🎚️ Power Consumption Reference

### WS2812B LED Power Draw

| Scenario | Current per LED | 60 LEDs Total |
|----------|----------------|---------------|
| Off      | 1 mA           | 60 mA         |
| Low brightness (25%) | 15 mA | 900 mA       |
| Medium brightness (50%) | 30 mA | 1.8A        |
| High brightness (75%) | 45 mA | 2.7A        |
| Full white (100%) | 60 mA    | 3.6A        |

### ESP32 Power Draw

| Activity | Current |
|----------|---------|
| Deep sleep | 10-150 µA |
| WiFi off | 80-160 mA |
| WiFi active | 160-260 mA |
| WiFi + BLE | 200-350 mA |

### Total System Power

**Example: 60 LEDs at 50% brightness**
- LEDs: 1.8A
- ESP32: 0.25A (WiFi active)
- **Total: ~2A**
- **Recommended supply: 3-4A** (50-100% headroom)

---

## 🔒 Safety Warnings

⚠️ **DO NOT:**
- Connect USB and external 5V simultaneously
- Exceed power supply's rated current
- Use power supply with wrong voltage
- Leave exposed wires that could short
- Run LEDs at 100% brightness continuously without cooling

✅ **DO:**
- Double-check polarity before connecting power
- Use appropriate wire gauge
- Add fuse for protection
- Insulate all connections properly
- Monitor temperature during extended operation
- Start with low brightness and increase gradually

---

## 📞 Need Help?

**Check these first:**
1. Review the [README.md](README.md) for software setup
2. Verify all connections match the diagram above
3. Measure power supply voltage with multimeter (should be 5V ±0.25V)
4. Check Serial Monitor output (115200 baud) for error messages

**Common Questions:**
- "Can I power ESP32 from LED strip?" → Yes, but ensure stable power
- "Do I need level shifter?" → Recommended but usually works without
- "Can I use 12V power supply?" → NO! Only 5V for WS2812B
- "How many LEDs can ESP32 control?" → Thousands, limited by memory and power

---

**Created:** 2026-07-17  
**Project:** LED Matrix Display Controller  
**Board:** ESP32  
**LED Type:** WS2812B
