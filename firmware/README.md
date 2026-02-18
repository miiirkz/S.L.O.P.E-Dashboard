# S.L.O.P.E Dashboard - ESP32 Firmware

Complete Arduino firmware for the ESP32-based landslide monitoring system with real-time sensor data collection, risk assessment, and WiFi connectivity.

## Hardware Requirements

### Microcontroller
- **ESP32 Development Board** (2.4GHz WiFi 802.11 b/g/n)

### Display
- **TFT Display**: ILI9341 2.4" 240x320 SPI
  - Resolution: 240x320 pixels
  - Interface: SPI
  - Touchscreen support (optional)

### Sensors
1. **MPU6050** - 6-Axis Accelerometer/Gyroscope (I2C)
   - Measures acceleration and tilt
   - I2C address: 0x68

2. **Soil Moisture Sensor**
   - Digital output: Pin D5
   - Analog output: Pin A0 (GPIO36)

3. **SW-420 Vibration Sensor**
   - Digital output: Pin D4

### Output Components
1. **Status LEDs** (4 LEDs):
   - D0 (GPIO0): GREEN - Low Risk
   - D3 (GPIO15): YELLOW - Moderate Risk
   - D7 (GPIO13): ORANGE - High Risk
   - D8 (GPIO12): RED - Critical Risk

2. **Buzzer**: GPIO3 (RX pin)
   - Active buzzer or passive buzzer with tone support

## Pin Configuration

### TFT Display (SPI)
| Pin Function | GPIO Pin | Description |
|-------------|----------|-------------|
| CS (Chip Select) | GPIO15 | SPI Chip Select |
| RST (Reset) | GPIO4 | Display Reset |
| DC (Data/Command) | GPIO2 | Data/Command Select |
| MOSI (SDI) | GPIO23 | SPI Data Out |
| CLK (SCK) | GPIO18 | SPI Clock |
| MISO (SD0) | GPIO19 | SPI Data In |
| LED (Backlight) | GPIO5 | Backlight Control |

### I2C (MPU6050)
| Pin Function | GPIO Pin |
|-------------|----------|
| SDA | GPIO21 |
| SCL | GPIO22 |

### Sensors
| Sensor | Pin | GPIO |
|--------|-----|------|
| Soil Moisture (Digital) | D5 | GPIO5 |
| Soil Moisture (Analog) | A0 | GPIO36 |
| Vibration Sensor | D4 | GPIO4 |

### LEDs
| LED Color | Pin | GPIO |
|-----------|-----|------|
| Green | D0 | GPIO0 |
| Yellow | D3 | GPIO15 |
| Orange | D7 | GPIO13 |
| Red | D8 | GPIO12 |

### Buzzer
| Component | Pin | GPIO |
|-----------|-----|------|
| Buzzer | RX | GPIO3 |

## Risk Assessment Thresholds

### Soil Moisture (%)
- **GREEN** (Low Risk): 0-20%
- **YELLOW** (Moderate): 21-35%
- **ORANGE** (High): 36-45%
- **RED** (Critical): 46%+

### Vibration (g-force)
- **GREEN** (Low Risk): 0-0.02g
- **YELLOW** (Moderate): 0.03-0.08g
- **ORANGE** (High): 0.09-0.20g
- **RED** (Critical): >0.20g

### Tilt Angle (degrees)
- **GREEN** (Low Risk): 0-2°
- **YELLOW** (Moderate): 3-5°
- **ORANGE** (High): 6-10°
- **RED** (Critical): >10°

## Buzzer Patterns

- **GREEN/YELLOW**: Silent (no alert)
- **ORANGE**: 500ms beep, 1500ms pause (slow warning)
- **RED**: 200ms rapid beeps (critical alert)

## Required Arduino Libraries

Install these libraries via Arduino Library Manager:

```
1. Adafruit_GFX (by Adafruit)
2. Adafruit_ILI9341 (by Adafruit)
3. MPU6050 (by Electronic Cats or Adafruit)
4. ArduinoJson (by Benoit Blanchon)
5. WiFi (built-in with ESP32)
6. HTTPClient (built-in with ESP32)
7. Wire (built-in)
8. SPI (built-in)
```

## Installation Instructions

### 1. Arduino IDE Setup

1. Install Arduino IDE (v1.8.19 or later / v2.x)
2. Add ESP32 board support:
   - Open **File > Preferences**
   - Add to "Additional Board Manager URLs": 
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools > Board > Boards Manager**
   - Search for "ESP32" and install "esp32 by Espressif Systems"

### 2. Install Required Libraries

Go to **Sketch > Include Library > Manage Libraries** and install:
- Adafruit GFX Library
- Adafruit ILI9341
- MPU6050 by Electronic Cats (or Adafruit MPU6050)
- ArduinoJson

### 3. Configure WiFi Settings

Edit the firmware file `slope_esp32.ino`:

```cpp
// Update these lines with your WiFi credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* SERVER_URL = "http://YOUR_SERVER_IP:5000/api/sensor-data";
```

### 4. Upload Firmware

1. Connect ESP32 to computer via USB
2. Select board: **Tools > Board > ESP32 Arduino > ESP32 Dev Module**
3. Select port: **Tools > Port > [Your ESP32 Port]**
4. Click **Upload**
5. Open **Serial Monitor** (115200 baud) to view debug output

## Features

### 1. Sensor Data Collection
- Real-time monitoring of soil moisture, acceleration, tilt, and vibration
- Auto-calibration of MPU6050 on startup
- Sensor health checks during initialization

### 2. TFT Display
- Real-time sensor readings with color-coded risk indicators
- 2-second auto-refresh
- System status (WiFi, uptime)
- Clear visual feedback with risk level display

### 3. Risk Assessment
- Simultaneous monitoring of three risk factors
- Overall risk level = highest individual risk
- Color-coded LED indicator for current risk level

### 4. Alert System
- Single active LED per risk level
- Progressive buzzer patterns based on risk severity
- Non-blocking alert implementation

### 5. WiFi Connectivity
- Automatic connection to 2.4GHz WiFi (802.11 b/g/n)
- JSON data transmission to Flask backend every 5 seconds
- Auto-reconnection on connection loss
- Graceful offline mode if WiFi unavailable

### 6. Debug Output
- Serial monitor output every 3 seconds
- Detailed sensor readings
- Connection status
- Risk level information

## Data Format (JSON)

Data sent to Flask backend:

```json
{
  "soil_moisture": 25.5,
  "accel_x": 0.01,
  "accel_y": -0.02,
  "accel_z": 1.00,
  "accel_magnitude": 0.03,
  "tilt_angle": 2.5,
  "vibration": 0,
  "risk_level": 1,
  "timestamp": 123456
}
```

## Testing

### Individual Sensor Tests

1. **Serial Monitor Test**
   - Open Serial Monitor (115200 baud)
   - Verify all sensors initialize successfully
   - Check sensor readings every 3 seconds

2. **TFT Display Test**
   - Verify display shows startup sequence
   - Confirm all sensor values update every 2 seconds
   - Check color-coded risk indicators

3. **LED Test**
   - Simulate different risk conditions
   - Verify only one LED is active at a time
   - Confirm LED matches calculated risk level

4. **Buzzer Test**
   - Test silent operation (GREEN/YELLOW)
   - Verify ORANGE pattern (500ms/1500ms)
   - Verify RED pattern (200ms rapid)

5. **WiFi Test**
   - Verify connection to WiFi network
   - Check data transmission in Serial Monitor
   - Test offline mode by disconnecting WiFi

## Troubleshooting

### Display Issues
- Check SPI connections
- Verify TFT_CS, TFT_DC, TFT_RST pins
- Ensure backlight LED pin is HIGH
- Try different SPI speed in library

### MPU6050 Not Found
- Check I2C connections (SDA=21, SCL=22)
- Verify I2C address (default 0x68)
- Test with I2C scanner sketch
- Check power supply (3.3V)

### WiFi Connection Failed
- Verify SSID and password
- Ensure router is configured for 2.4GHz (ESP32 does not support 5GHz)
- Check signal strength
- Verify ESP32 board with WiFi capability

### Sensor Reading Issues
- Check pin connections
- Verify sensor power (3.3V or 5V depending on sensor)
- Test with simple read sketch
- Check for loose connections

### Upload Failed
- Check USB cable (must support data)
- Hold BOOT button during upload
- Verify correct board and port selection
- Try different USB port

## Power Considerations

- **ESP32**: 3.3V logic, 5V USB power
- **TFT Display**: 3.3V (check your model)
- **MPU6050**: 3.3V-5V
- **Sensors**: Check individual requirements
- **LEDs**: Use current-limiting resistors (220Ω recommended)
- **Buzzer**: Check if 3.3V or 5V variant

**Recommended**: Use external 5V power supply for complete system.

## Customization

### Adjusting Risk Thresholds

Edit threshold definitions in the firmware:

```cpp
// Soil Moisture (%)
#define SOIL_GREEN_MAX    20
#define SOIL_YELLOW_MAX   35
#define SOIL_ORANGE_MAX   45

// Vibration (g)
#define VIB_GREEN_MAX     0.02
#define VIB_YELLOW_MAX    0.08
#define VIB_ORANGE_MAX    0.20

// Tilt (degrees)
#define TILT_GREEN_MAX    2
#define TILT_YELLOW_MAX   5
#define TILT_ORANGE_MAX   10
```

### Changing Update Intervals

```cpp
// Display update (in loop)
if (millis() - lastDisplayUpdate >= 2000) { // Change 2000 to desired ms

// Server data transmission (in sendDataToServer)
if (currentTime - lastSendTime < 5000) { // Change 5000 to desired ms

// Serial debug output (in printDebugInfo)
if (currentTime - lastPrintTime < 3000) { // Change 3000 to desired ms
```

## License

This firmware is part of the S.L.O.P.E Dashboard project.

## Support

For issues or questions:
1. Check Serial Monitor for error messages
2. Verify all hardware connections
3. Ensure all libraries are installed
4. Review pin configuration matches your hardware

## Version History

- **v1.0.0** (2026-02-18): Initial release
  - Complete hardware integration
  - TFT display with real-time updates
  - Risk assessment engine
  - WiFi connectivity with JSON transmission
  - LED and buzzer alert system
  - Serial debug output
