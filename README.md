# S.L.O.P.E Dashboard

**S**oil **L**andslide **O**bservation & **P**revention **E**ngineering

A complete landslide monitoring system with ESP32 hardware integration and web-based dashboard for real-time sensor data visualization and risk assessment.

## Overview

This project consists of two main components:

1. **ESP32 Firmware** - Hardware integration for real-time sensor monitoring
2. **Flask Backend** - Web server for data collection and dashboard visualization

## Features

### ESP32 Firmware
- Real-time monitoring of multiple sensors (soil moisture, acceleration, tilt, vibration)
- TFT display with live sensor readings and color-coded risk indicators
- 4-level LED status system (GREEN, YELLOW, ORANGE, RED)
- Progressive buzzer alerts based on risk level
- WiFi connectivity with automatic data transmission
- Graceful offline mode operation
- Serial debug output for troubleshooting

### Flask Backend
- RESTful API for receiving sensor data from ESP32
- Real-time data storage and retrieval
- Web-based dashboard for monitoring
- CORS-enabled for cross-origin requests

## Quick Start

### ESP32 Firmware Setup

1. **Navigate to firmware directory:**
   ```bash
   cd firmware/
   ```

2. **Read the documentation:**
   - [README.md](firmware/README.md) - Complete firmware documentation
   - [WIRING.md](firmware/WIRING.md) - Hardware wiring guide
   - [LIBRARIES.md](firmware/LIBRARIES.md) - Required Arduino libraries
   - [TESTING.md](firmware/TESTING.md) - Comprehensive testing procedures

3. **Install Arduino IDE and required libraries** (see LIBRARIES.md)

4. **Configure WiFi credentials** in `slope_esp32.ino`:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   const char* SERVER_URL = "http://YOUR_SERVER_IP:5000/api/sensor-data";
   ```

5. **Upload firmware to ESP32** using Arduino IDE

### Flask Backend Setup

1. **Install Python dependencies:**
   ```bash
   pip install -r requirements.txt
   ```

2. **Run the Flask server:**
   ```bash
   python app.py
   ```

3. **Access the dashboard:**
   ```
   http://localhost:5000
   ```

## Hardware Requirements

### ESP32 System Components
- ESP32 Development Board (5GHz WiFi capable)
- ILI9341 TFT Display (2.4" 240x320, SPI)
- MPU6050 Accelerometer/Gyroscope (I2C)
- Soil Moisture Sensor (Digital + Analog)
- SW-420 Vibration Sensor
- 4x Status LEDs (Green, Yellow, Orange, Red)
- Active Buzzer
- Jumper wires and breadboard
- 5V 2A Power Supply (recommended)

See [WIRING.md](firmware/WIRING.md) for complete wiring instructions.

## Risk Assessment System

The system monitors three critical parameters and calculates an overall risk level:

### Thresholds

| Parameter | GREEN (Low) | YELLOW (Moderate) | ORANGE (High) | RED (Critical) |
|-----------|-------------|-------------------|---------------|----------------|
| Soil Moisture | 0-20% | 21-35% | 36-45% | 46%+ |
| Vibration | 0-0.02g | 0.03-0.08g | 0.09-0.20g | >0.20g |
| Tilt Angle | 0-2° | 3-5° | 6-10° | >10° |

### Alert System
- **Overall Risk** = Highest risk level among all three sensors
- **LED Indicators**: Only one LED active at a time, corresponding to current risk
- **Buzzer Patterns**:
  - GREEN/YELLOW: Silent
  - ORANGE: 500ms beep, 1500ms pause
  - RED: 200ms rapid beeps

## API Endpoints

### Receive Sensor Data (POST)
```
POST /api/sensor-data
Content-Type: application/json

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

### Get Latest Data (GET)
```
GET /api/data
GET /api/sensor-data
```

Returns latest sensor readings in JSON format.

## Project Structure

```
S.L.O.P.E-Dashboard/
├── firmware/
│   ├── slope_esp32/
│   │   └── slope_esp32.ino    # Main ESP32 firmware
│   ├── README.md              # Firmware documentation
│   ├── WIRING.md             # Hardware wiring guide
│   ├── LIBRARIES.md          # Required libraries
│   └── TESTING.md            # Testing procedures
├── static/
│   └── style.css             # Dashboard styling
├── templates/
│   └── index.html            # Dashboard HTML
├── app.py                    # Flask backend
├── requirements.txt          # Python dependencies
└── README.md                 # This file
```

## Testing

Comprehensive testing procedures are available in [TESTING.md](firmware/TESTING.md), including:

- Individual sensor tests
- Display functionality tests
- LED and buzzer tests
- WiFi connectivity tests
- Risk calculation verification
- Long-term stability tests
- Complete integration tests

## Troubleshooting

### ESP32 Issues
- **Display not working**: Check SPI connections and power supply
- **Sensors not reading**: Verify pin connections and sensor power
- **WiFi won't connect**: Check credentials, try 2.4GHz network
- **System crashes**: Ensure adequate power supply (5V 2A recommended)

See [TESTING.md](firmware/TESTING.md) for detailed troubleshooting guide.

### Flask Issues
- **Port already in use**: Change port in app.py or stop conflicting service
- **No data received**: Verify ESP32 can reach server, check firewall

## Development

### Customizing Risk Thresholds

Edit threshold definitions in `firmware/slope_esp32/slope_esp32.ino`:

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

### Customizing Update Intervals

```cpp
// Display update interval (milliseconds)
if (millis() - lastDisplayUpdate >= 2000) { // 2 seconds

// Data transmission interval (milliseconds)
if (currentTime - lastSendTime < 5000) { // 5 seconds
```

## License

This project is open source and available for educational and research purposes.

## Contributors

- ESP32 Firmware Development
- Flask Backend Development
- Hardware Integration
- Testing and Documentation

## Version History

- **v1.0.0** (2026-02-18): Initial release
  - Complete ESP32 firmware with all sensor integrations
  - TFT display with real-time updates
  - Risk assessment engine
  - WiFi connectivity and data transmission
  - Flask backend with API endpoints
  - Comprehensive documentation and testing guides

## Support

For issues or questions:
1. Check the documentation in the `firmware/` directory
2. Review the testing guide (TESTING.md)
3. Verify hardware connections (WIRING.md)
4. Check Serial Monitor output for error messages

## Future Enhancements

- Database integration for historical data storage
- Advanced data visualization and graphing
- Email/SMS alerts for critical conditions
- Mobile app integration
- Multi-device support
- Machine learning for predictive analytics
