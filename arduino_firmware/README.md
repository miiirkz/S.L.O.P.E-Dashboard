# S.L.O.P.E Dashboard - ESP8266 Firmware

## Overview
This firmware enables the ESP8266 to monitor landslide risk using multiple sensors and communicate with the S.L.O.P.E Dashboard Flask backend.

## Hardware Requirements

### Microcontroller
- ESP8266 (NodeMCU or similar)

### Sensors
- **MPU6050**: 6-axis accelerometer/gyroscope for tilt and vibration detection
- **Soil Moisture Sensor**: Analog sensor for soil moisture measurement
- **SW-420 Vibration Sensor**: Digital vibration detection

### Output Devices
- 4x LEDs (Green, Yellow, Orange, Red) for visual risk indication
- 1x Buzzer for audio alerts

## Pin Configuration

| Component | ESP8266 Pin | GPIO | Description |
|-----------|-------------|------|-------------|
| MPU6050 SDA | D2 | GPIO4 | I2C Data |
| MPU6050 SCL | D1 | GPIO5 | I2C Clock |
| Soil Digital | D5 | GPIO14 | Digital soil sensor |
| Soil Analog | A0 | ADC | Analog soil moisture |
| SW-420 Vibration | D4 | GPIO2 | Digital vibration sensor |
| GREEN LED | D0 | GPIO16 | Low risk indicator |
| YELLOW LED | D3 | GPIO0 | Medium risk indicator |
| ORANGE LED | D7 | GPIO13 | High risk indicator |
| RED LED | D8 | GPIO15 | Critical risk indicator |
| BUZZER | RX | GPIO3 | Audio alert |

## Required Libraries

Install these libraries through Arduino IDE Library Manager:

1. **ESP8266WiFi** - Built-in with ESP8266 board package
2. **ESP8266HTTPClient** - Built-in with ESP8266 board package
3. **Wire** - Built-in I2C library
4. **MPU6050** by Electronic Cats
5. **ArduinoJson** by Benoit Blanchon (v6.x)

## Installation Steps

### 1. Arduino IDE Setup
1. Install Arduino IDE (version 1.8.x or 2.x)
2. Add ESP8266 board support:
   - Go to `File > Preferences`
   - Add to "Additional Boards Manager URLs": 
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Go to `Tools > Board > Boards Manager`
   - Search for "ESP8266" and install

### 2. Install Required Libraries
1. Open Arduino IDE
2. Go to `Tools > Manage Libraries`
3. Install:
   - "MPU6050" by Electronic Cats
   - "ArduinoJson" by Benoit Blanchon

### 3. Configure Firmware
1. Open `slope_monitor.ino` in Arduino IDE
2. Update WiFi credentials:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
3. Update server URL with your Flask server IP:
   ```cpp
   const char* serverUrl = "http://YOUR_SERVER_IP:5000/api/sensor-data";
   ```

### 4. Upload Firmware
1. Connect ESP8266 to computer via USB
2. Select board: `Tools > Board > ESP8266 Boards > NodeMCU 1.0 (ESP-12E Module)`
3. Select correct COM port: `Tools > Port`
4. Click `Upload` button
5. Monitor serial output: `Tools > Serial Monitor` (115200 baud)

## Risk Calculation

### Thresholds

#### Soil Moisture
- **GREEN**: 0-20%
- **YELLOW**: 21-35%
- **ORANGE**: 36-45%
- **RED**: 46%+

#### Vibration
- **GREEN**: 0-0.02g
- **YELLOW**: 0.03-0.08g
- **ORANGE**: 0.09-0.20g
- **RED**: >0.20g

#### Tilt Angle
- **GREEN**: 0-2°
- **YELLOW**: 3-5°
- **ORANGE**: 6-10°
- **RED**: >10°

### Overall Risk Level
The system calculates individual risk scores for each sensor and uses the **highest score** as the overall risk level.

## Features

### 1. Risk Calculation Engine
- Monitors soil moisture, vibration, and tilt angle
- Calculates individual and combined risk scores
- Real-time risk level determination

### 2. LED Control
- Visual indication of current risk level
- Only one LED active at a time
- Color-coded: Green (safe) → Yellow (caution) → Orange (warning) → Red (critical)

### 3. Buzzer Patterns
- **GREEN/YELLOW**: Silent
- **ORANGE**: Beep pattern (500ms ON, 1500ms OFF)
- **RED**: Continuous fast beeping (200ms ON, 200ms OFF)

### 4. Time-Sustained Analysis
- Monitors high moisture conditions over time
- Triggers warning if moisture stays above ORANGE threshold for 30+ seconds

### 5. Rate of Change Detection
- Detects sudden tilt changes
- Calculates tilt rate (degrees per second)
- Alerts on rapid ground movement

### 6. Data Transmission
- Sends sensor data to Flask backend every 5 seconds
- JSON format with all sensor readings and risk assessment
- Includes error handling and retry logic

## Data Format

The ESP8266 sends data in JSON format:

```json
{
  "moisture": 25.3,
  "accel_x": 0.012,
  "accel_y": -0.008,
  "accel_z": 0.995,
  "vibration": 0.015,
  "tilt": 1.8,
  "risk_level": "GREEN",
  "risk_score": 0
}
```

## Troubleshooting

### WiFi Connection Issues
- Verify SSID and password are correct
- Ensure ESP8266 is within WiFi range
- Check if WiFi network is 2.4GHz (ESP8266 doesn't support 5GHz)

### Sensor Not Detected
- **MPU6050**: Check I2C connections (SDA/SCL), verify I2C address (default 0x68)
- **Soil Sensor**: Verify analog pin connection
- **Vibration**: Check digital pin connection

### LEDs Not Working
- Verify correct pin connections
- Check LED polarity (anode to pin, cathode to ground with resistor)
- Test with digitalWrite(LED_PIN, HIGH) in setup()

### Server Communication Failed
- Verify server IP address and port
- Ensure Flask server is running
- Check firewall settings
- Verify both devices on same network

## Serial Monitor Output

Example output at 115200 baud:
```
=================================
S.L.O.P.E Dashboard - ESP8266
=================================

Connecting to WiFi........
WiFi Connected!
IP Address: 192.168.1.100
Initializing sensors...
MPU6050 connected successfully!
System Ready!

--- Sensor Reading ---
Moisture: 18.5%
Accel X: 0.012g
Accel Y: -0.008g
Accel Z: 0.995g
Vibration: 0.015g
Tilt: 1.8°
Risk Level: GREEN
Risk Score: 0
---------------------

Data sent successfully! Response code: 200
Server response: {"status":"success","risk":{"overall":"GREEN","score":0}}
```

## Safety Notes

1. **Power Supply**: Use stable 5V power supply for ESP8266
2. **Sensor Protection**: Keep electronic components dry
3. **Deployment**: Protect sensors from weather in waterproof enclosure
4. **Testing**: Test system thoroughly before field deployment
5. **Monitoring**: Regularly check system status and battery level

## License

Part of the S.L.O.P.E Dashboard system for landslide monitoring.
