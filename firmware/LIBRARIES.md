# S.L.O.P.E ESP32 - Library Dependencies

## Required Arduino Libraries

These libraries must be installed via Arduino Library Manager before uploading the firmware.

### Display Libraries
1. **Adafruit GFX Library** (v1.11.0 or later)
   - Author: Adafruit
   - Description: Core graphics library
   - Install: Library Manager > Search "Adafruit GFX"

2. **Adafruit ILI9341** (v1.5.0 or later)
   - Author: Adafruit
   - Description: ILI9341 TFT display driver
   - Install: Library Manager > Search "Adafruit ILI9341"

### Sensor Libraries
3. **MPU6050** (v1.0.0 or later)
   - Author: Electronic Cats or Adafruit
   - Description: MPU6050 6-axis accelerometer/gyroscope
   - Install: Library Manager > Search "MPU6050"
   - Alternatives:
     - "Adafruit MPU6050" by Adafruit
     - "MPU6050" by Electronic Cats

### Networking Libraries
4. **ArduinoJson** (v6.21.0 or later)
   - Author: Benoit Blanchon
   - Description: JSON serialization/deserialization
   - Install: Library Manager > Search "ArduinoJson"
   - Note: Use version 6.x, not 5.x

### Built-in Libraries (No Installation Required)
- **WiFi** - ESP32 WiFi functionality
- **HTTPClient** - HTTP client for REST API calls
- **Wire** - I2C communication
- **SPI** - SPI communication

## Installation Steps

### Method 1: Arduino IDE Library Manager
1. Open Arduino IDE
2. Go to **Sketch > Include Library > Manage Libraries**
3. Search for each library by name
4. Click "Install" for each library
5. Wait for installation to complete

### Method 2: Manual Installation
1. Download library ZIP from GitHub
2. Go to **Sketch > Include Library > Add .ZIP Library**
3. Select the downloaded ZIP file
4. Restart Arduino IDE

## Library Versions Tested

| Library | Version | Status |
|---------|---------|--------|
| Adafruit GFX | 1.11.3 | ✓ Tested |
| Adafruit ILI9341 | 1.5.12 | ✓ Tested |
| MPU6050 (Electronic Cats) | 1.0.3 | ✓ Tested |
| ArduinoJson | 6.21.3 | ✓ Tested |

## Troubleshooting

### Library Conflicts
If you have multiple MPU6050 libraries installed:
1. Uninstall all MPU6050 libraries
2. Install only one (recommended: Electronic Cats version)
3. Restart Arduino IDE

### Compilation Errors
- Ensure all libraries are up to date
- Check library versions compatibility
- Try cleaning build (Sketch > Clean)
- Restart Arduino IDE

### Missing Libraries
Error: `fatal error: [Library].h: No such file or directory`
- Solution: Install the missing library via Library Manager

## Optional Libraries

For advanced features (not required for basic operation):
- **Adafruit BusIO** - Enhanced I2C/SPI support (auto-installed with Adafruit libraries)
- **Adafruit Unified Sensor** - Unified sensor driver (required by some sensor libraries)

## ESP32 Board Support

Required board package:
- **ESP32** by Espressif Systems (v2.0.0 or later)
- Add to Board Manager URLs: 
  ```
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
