# S.L.O.P.E ESP8266 Firmware

Complete Arduino firmware for the **Smart Landslide Observation and Prevention Electronics (S.L.O.P.E)** monitoring system.

## Overview

This firmware runs on an ESP8266 microcontroller and provides real-time landslide risk assessment using multiple sensors. It features advanced analytics including time-sustained analysis, rate of change detection, and hysteresis to prevent false alarms.

## Hardware Requirements

### Microcontroller
- ESP8266 (NodeMCU, Wemos D1 Mini, or compatible)

### Sensors
- **MPU6050** - 6-axis accelerometer/gyroscope for tilt detection
- **Soil Moisture Sensor** - Analog sensor for ground saturation monitoring
- **SW-420** - Vibration sensor module

### Indicators
- 4x LEDs (Green, Yellow, Orange, Red)
- 1x Active buzzer or piezo buzzer

## Pin Configuration

| Component | ESP8266 Pin | GPIO | Description |
|-----------|-------------|------|-------------|
| Green LED | D0 | GPIO16 | Low risk indicator |
| Yellow LED | D3 | GPIO0 | Moderate risk indicator |
| Orange LED | D7 | GPIO13 | High risk indicator |
| Red LED | D8 | GPIO15 | Critical risk indicator |
| Buzzer | RX | GPIO3 | Audio alarm output |
| MPU6050 SCL | D1 | GPIO5 | I2C Clock |
| MPU6050 SDA | D2 | GPIO4 | I2C Data |
| SW-420 Vibration | D4 | GPIO2 | Digital vibration detection |
| Soil Digital | D5 | GPIO14 | Digital soil threshold |
| Soil Analog | A0 | ADC0 | Analog moisture reading |

## Features

### 1. Multi-Sensor Monitoring
- **MPU6050 Accelerometer**: Measures tilt angle and detects ground movement
- **Soil Moisture**: Analog reading converted to 0-100% scale
- **Vibration Detection**: Digital signal from SW-420 module
- All sensors read every 2 seconds

### 2. Risk Level Assessment

The system calculates individual risk levels for each sensor:

#### Soil Moisture Thresholds
- **GREEN**: 0-20% (Low moisture)
- **YELLOW**: 21-35% (Moderate moisture)
- **ORANGE**: 36-45% (High moisture)
- **RED**: 46%+ (Critical moisture)

#### Vibration Thresholds (in G)
- **GREEN**: 0-0.02g (Minimal vibration)
- **YELLOW**: 0.03-0.08g (Light vibration)
- **ORANGE**: 0.09-0.20g (Moderate vibration)
- **RED**: >0.20g (Strong vibration)

#### Tilt Angle Thresholds
- **GREEN**: 0-2° (Stable)
- **YELLOW**: 3-5° (Slight tilt)
- **ORANGE**: 6-10° (Moderate tilt)
- **RED**: >10° (Critical tilt)

#### Combined Risk Logic
- **GREEN**: All sensors at low risk
- **YELLOW**: One sensor at moderate risk
- **ORANGE**: Two or more sensors elevated
- **RED**: Any sensor at critical level
- **CRITICAL**: Advanced condition triggered

### 3. Advanced Analytics

#### Time-Sustained Analysis
- Monitors if soil moisture stays above 40% for 30+ minutes
- Automatically escalates to CRITICAL level
- Helps identify prolonged dangerous conditions

#### Rate of Change Detection
- Checks tilt angle changes every 10 seconds
- If tilt changes >5° in 10 seconds, flags as CRITICAL
- Detects rapid ground movement

#### Hysteresis (2% margin)
- Prevents rapid alarm fluttering
- Adds stability to risk level transitions
- Reduces false positives near threshold boundaries

### 4. Visual & Audio Indicators

#### LED Control
- **Only ONE LED lights at a time** based on overall risk level
- LED corresponds to current risk assessment
- Provides immediate visual status

#### Buzzer Patterns
- **GREEN**: Silent
- **YELLOW**: Silent
- **ORANGE**: Beep-beep-beep pattern (1-second intervals)
- **RED/CRITICAL**: Continuous alarm buzz

### 5. WiFi Data Transmission

Sends JSON data to Flask server every 2 seconds:

```json
{
  "soilMoisture": 35.2,
  "soilDigital": 0,
  "accelX": 0.023,
  "accelY": -0.015,
  "accelZ": 0.982,
  "tiltAngle": 2.3,
  "vibration": false,
  "riskLevel": "YELLOW",
  "riskScore": 45,
  "timestamp": 123456
}
```

### 6. Offline Operation
- All risk calculations happen locally on the ESP8266
- System functions fully without WiFi connection
- LEDs and buzzer work independently of network status

## Installation

### 1. Arduino IDE Setup

1. Install Arduino IDE (1.8.19 or later)
2. Add ESP8266 board support:
   - Go to File → Preferences
   - Add to Additional Boards Manager URLs:
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Go to Tools → Board → Boards Manager
   - Search for "esp8266" and install

### 2. Required Libraries

Install these libraries via Arduino Library Manager (Sketch → Include Library → Manage Libraries):

- **ESP8266WiFi** (included with ESP8266 board package)
- **ESP8266HTTPClient** (included with ESP8266 board package)
- **Wire** (built-in)
- **ArduinoJson** by Benoit Blanchon (version 6.x)

### 3. Configuration

Edit the firmware file `SLOPE_ESP8266.ino` and update:

```cpp
// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Server endpoint
const char* serverUrl = "http://YOUR_SERVER_IP:5000/api/data";
```

### 4. Upload

1. Connect ESP8266 via USB
2. Select board: Tools → Board → ESP8266 Boards → NodeMCU 1.0 (or your board)
3. Select port: Tools → Port → (your COM port)
4. Click Upload button

## Usage

### Serial Monitor

Open Serial Monitor (Tools → Serial Monitor) at **115200 baud** to view:
- System startup messages
- Sensor readings every 2 seconds
- Risk assessment calculations
- WiFi connection status
- Data transmission confirmations

### Example Serial Output

```
=== S.L.O.P.E System Starting ===
Pins initialized
MPU6050 initialized successfully
Connecting to WiFi: MyNetwork
........
WiFi connected!
IP address: 192.168.1.100
=== System Ready ===

===== SENSOR READINGS =====
Soil Moisture: 23.5% | Digital: 0
Acceleration (G): X=0.023 Y=-0.015 Z=0.982
Tilt Angle: 2.3 degrees
Vibration: None

===== RISK ASSESSMENT =====
Risk Level: YELLOW
Risk Score: 34
===========================
```

## Troubleshooting

### MPU6050 Not Detected
- Check I2C connections (SDA to D2, SCL to D1)
- Verify MPU6050 has power (VCC to 3.3V, GND to GND)
- Try different I2C address (some boards use 0x69)

### WiFi Not Connecting
- Verify SSID and password are correct
- Check 2.4GHz WiFi network (ESP8266 doesn't support 5GHz)
- System will still work offline with local LEDs/buzzer

### Soil Moisture Always 0% or 100%
- Check analog sensor connection to A0
- Sensor may need calibration
- Adjust mapping in `readSoilMoisture()` function

### LEDs Not Working
- Verify LED polarity (anode to GPIO pin, cathode to GND via resistor)
- Check pin definitions match your wiring
- Use 220Ω-330Ω resistors with LEDs

### Buzzer Not Working
- RX pin (GPIO3) may conflict with Serial. Consider using different pin.
- Verify buzzer type (active vs passive)
- Check power requirements

## Customization

### Adjusting Thresholds

Edit threshold constants in the firmware:

```cpp
// Soil Moisture (%)
#define SOIL_GREEN_MAX    20.0
#define SOIL_YELLOW_MAX   35.0
#define SOIL_ORANGE_MAX   45.0

// Vibration (g)
#define VIB_GREEN_MAX     0.02
#define VIB_YELLOW_MAX    0.08
#define VIB_ORANGE_MAX    0.20

// Tilt (degrees)
#define TILT_GREEN_MAX    2.0
#define TILT_YELLOW_MAX   5.0
#define TILT_ORANGE_MAX   10.0
```

### Changing Sensor Read Interval

```cpp
const unsigned long SENSOR_READ_INTERVAL = 2000;  // milliseconds
```

### Modifying Buzzer Patterns

Edit the `updateBuzzer()` function to customize beep patterns.

## Safety Notes

⚠️ **Important Safety Information**:

1. This system is a **monitoring tool** and should not be the sole safety measure
2. Always follow official landslide warning systems and evacuation orders
3. Regular maintenance and calibration of sensors is required
4. Battery backup recommended for power reliability
5. Test the system regularly to ensure proper operation

## API Endpoint Requirements

The Flask server should accept POST requests at `/api/data` with JSON payload. Example handler:

```python
@app.route('/api/data', methods=['POST'])
def receive_data():
    data = request.json
    # Process data (store in database, trigger alerts, etc.)
    return jsonify({"status": "success"}), 200
```

## License

This firmware is part of the S.L.O.P.E Dashboard project.

## Contributing

Issues and pull requests are welcome. For major changes, please open an issue first.

## Credits

Developed for landslide early warning and monitoring systems.
