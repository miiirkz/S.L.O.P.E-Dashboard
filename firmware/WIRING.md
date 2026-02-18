# ESP32 Wiring Diagram - S.L.O.P.E Dashboard

## Complete Hardware Wiring Guide

This document provides detailed wiring instructions for connecting all components to the ESP32.

## Component Connection Summary

### 1. TFT Display (ILI9341 2.4" 240x320) - SPI Interface

| TFT Pin | Function | ESP32 GPIO | Notes |
|---------|----------|------------|-------|
| VCC | Power | 3.3V | Check if your display needs 5V |
| GND | Ground | GND | Common ground |
| CS | Chip Select | GPIO15 | SPI Chip Select |
| RESET | Reset | GPIO4 | Display reset control |
| DC | Data/Command | GPIO2 | Command/Data select |
| SDI (MOSI) | Data Out | GPIO23 | SPI Master Out Slave In |
| SCK | Clock | GPIO18 | SPI Clock |
| LED | Backlight | GPIO5 | Backlight control (can use 3.3V) |
| SDO (MISO) | Data In | GPIO19 | SPI Master In Slave Out |
| T_CLK | Touch Clock | Not Connected | Optional touchscreen |
| T_CS | Touch CS | Not Connected | Optional touchscreen |
| T_DIN | Touch Data In | Not Connected | Optional touchscreen |
| T_DO | Touch Data Out | Not Connected | Optional touchscreen |
| T_IRQ | Touch Interrupt | Not Connected | Optional touchscreen |

**Important Notes:**
- Most ILI9341 displays work with 3.3V logic
- Some displays may require 5V for VCC (check your module)
- LED pin can be connected to 3.3V directly for always-on backlight
- Touch screen pins are optional for this project

### 2. MPU6050 - I2C Interface

| MPU6050 Pin | Function | ESP32 GPIO |
|-------------|----------|------------|
| VCC | Power | 3.3V or 5V |
| GND | Ground | GND |
| SDA | I2C Data | GPIO21 |
| SCL | I2C Clock | GPIO22 |
| XDA | Auxiliary Data | Not Connected |
| XCL | Auxiliary Clock | Not Connected |
| AD0 | Address Select | GND (for 0x68) |
| INT | Interrupt | Not Connected |

**Important Notes:**
- AD0 to GND sets I2C address to 0x68
- AD0 to VCC sets I2C address to 0x69
- INT pin is optional for this application
- Use 4.7kΩ pull-up resistors on SDA/SCL if not on module

### 3. Soil Moisture Sensor

| Sensor Pin | Function | ESP32 GPIO |
|------------|----------|------------|
| VCC | Power | 3.3V or 5V |
| GND | Ground | GND |
| D0 | Digital Out | GPIO5 (D5) |
| A0 | Analog Out | GPIO36 (A0) |

**Important Notes:**
- ESP32 ADC reads 0-4095 (12-bit)
- GPIO36 is ADC1_CH0, suitable for analog reading
- Digital output triggers at threshold (sensor-dependent)
- Some sensors need 5V for optimal operation

### 4. SW-420 Vibration Sensor

| Sensor Pin | Function | ESP32 GPIO |
|------------|----------|------------|
| VCC | Power | 3.3V or 5V |
| GND | Ground | GND |
| D0 | Digital Out | GPIO4 (D4) |

**Important Notes:**
- Digital output: HIGH when vibration detected
- Adjust sensitivity with onboard potentiometer
- Some modules have LED indicator

### 5. Status LEDs

| LED Color | Anode (+) | Cathode (-) | GPIO | Resistor |
|-----------|-----------|-------------|------|----------|
| Green | Via 220Ω | GND | GPIO0 (D0) | 220Ω |
| Yellow | Via 220Ω | GND | GPIO15 (D3) | 220Ω |
| Orange | Via 220Ω | GND | GPIO13 (D7) | 220Ω |
| Red | Via 220Ω | GND | GPIO12 (D8) | 220Ω |

**Wiring:**
```
ESP32 GPIO → 220Ω Resistor → LED Anode (+) → LED Cathode (-) → GND
```

**Important Notes:**
- Always use current-limiting resistors (220Ω recommended)
- LEDs are active HIGH (HIGH = LED on)
- Check LED forward voltage (typically 2-3V)
- Resistor value can be 220Ω - 1kΩ for brightness adjustment

### 6. Buzzer

| Buzzer Pin | Function | ESP32 GPIO |
|------------|----------|------------|
| + (Positive) | Signal | GPIO3 (RX) |
| - (Negative) | Ground | GND |

**Important Notes:**
- Active buzzer: Beeps when HIGH
- Passive buzzer: Requires PWM/tone for sound
- GPIO3 is normally RX pin - be careful during upload
- May need transistor driver for loud buzzers

## Power Supply Recommendations

### Option 1: USB Power
- Connect ESP32 via USB (5V)
- All 3.3V components powered from ESP32 regulator
- **Limitation**: May not provide enough current for all components

### Option 2: External 5V Supply (Recommended)
- Use 5V 2A power adapter
- Connect to ESP32 VIN and GND
- Powers ESP32 and all peripherals
- **Recommended** for complete system with TFT display

### Power Distribution
```
5V Power Supply
    ├── ESP32 VIN (powers ESP32)
    ├── TFT VCC (if 5V display)
    ├── MPU6050 VCC
    ├── Soil Sensor VCC
    └── Vibration Sensor VCC

Common Ground (GND)
    ├── ESP32 GND
    ├── TFT GND
    ├── MPU6050 GND
    ├── Soil Sensor GND
    ├── Vibration Sensor GND
    ├── LED Cathodes (-)
    └── Buzzer (-)
```

## Breadboard Layout Tips

1. **Power Rails**: Use breadboard power rails for VCC and GND distribution
2. **Decoupling**: Add 0.1µF capacitors near each IC for noise filtering
3. **I2C Pull-ups**: If not on module, add 4.7kΩ resistors from SDA/SCL to 3.3V
4. **Wire Management**: Use different colored wires:
   - Red: VCC/Power
   - Black: GND
   - Yellow: I2C (SDA/SCL)
   - Blue: SPI signals
   - Green: Digital signals
   - White: Analog signals

## GPIO Pin Usage Reference

| GPIO | Function | Component |
|------|----------|-----------|
| GPIO0 | LED Green | Status LED |
| GPIO2 | TFT DC | TFT Display |
| GPIO3 | Buzzer | Alert Buzzer |
| GPIO4 | Vibration | SW-420 Sensor |
| GPIO5 | TFT LED / Soil Digital | TFT Backlight / Soil Sensor |
| GPIO12 | LED Red | Status LED |
| GPIO13 | LED Orange | Status LED |
| GPIO15 | TFT CS / LED Yellow | TFT Display / Status LED |
| GPIO18 | TFT SCK | TFT Display |
| GPIO19 | TFT MISO | TFT Display |
| GPIO21 | I2C SDA | MPU6050 |
| GPIO22 | I2C SCL | MPU6050 |
| GPIO23 | TFT MOSI | TFT Display |
| GPIO36 | Soil Analog | Soil Moisture Sensor |

## Troubleshooting Wiring Issues

### Display Not Working
- ✓ Check power (VCC/GND)
- ✓ Verify SPI connections (MOSI, CLK, CS, DC)
- ✓ Check LED backlight connection
- ✓ Verify voltage level (3.3V vs 5V)

### MPU6050 Not Found
- ✓ Check I2C connections (SDA=21, SCL=22)
- ✓ Verify power supply
- ✓ Check I2C address (0x68 with AD0 to GND)
- ✓ Add pull-up resistors if needed

### Sensors Not Reading
- ✓ Check power and ground
- ✓ Verify correct GPIO pins
- ✓ Test with multimeter
- ✓ Check for loose connections

### LEDs Not Lighting
- ✓ Check orientation (anode to resistor, cathode to GND)
- ✓ Verify resistor values (220Ω)
- ✓ Test LED with 3.3V directly
- ✓ Check GPIO pin configuration

## Safety Warnings

⚠️ **Important Safety Notes:**
- Never connect 5V directly to ESP32 GPIO pins (3.3V maximum)
- Always use current-limiting resistors with LEDs
- Check component voltage requirements before connecting
- Use proper gauge wires for power connections
- Ensure common ground for all components
- Do not exceed ESP32 maximum current per pin (12mA)

## Testing Procedure

1. **Power Test**: Connect power, verify ESP32 LED lights up
2. **TFT Test**: Upload test sketch, verify display shows output
3. **Sensor Test**: Check Serial Monitor for sensor readings
4. **LED Test**: Verify each LED can be controlled
5. **Buzzer Test**: Verify buzzer produces sound
6. **Complete Test**: Upload full firmware and verify all functions

## Additional Resources

- ESP32 Pinout: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
- ILI9341 Wiring: https://learn.adafruit.com/adafruit-2-dot-8-color-tft-touchscreen-breakout-v2/spi-wiring-and-test
- MPU6050 Guide: https://learn.adafruit.com/mpu6050-6-dof-accelerometer-and-gyro

## Revision History

- v1.0 (2026-02-18): Initial wiring guide
