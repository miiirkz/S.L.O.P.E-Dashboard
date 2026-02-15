# S.L.O.P.E - Smart Landslide Observation and Prevention Electronics

A complete IoT landslide monitoring system featuring ESP8266-based hardware sensors and a Flask web dashboard for real-time data visualization and risk assessment.

## 🌟 Features

### Hardware (ESP8266 Firmware)
- **Multi-Sensor Monitoring**: MPU6050 accelerometer, soil moisture, and vibration detection
- **Real-time Risk Assessment**: GREEN/YELLOW/ORANGE/RED/CRITICAL levels based on sensor data
- **Advanced Analytics**: Time-sustained analysis, rate of change detection, hysteresis
- **Visual & Audio Alerts**: Color-coded LEDs and buzzer patterns
- **WiFi Data Transmission**: Sends JSON data to server every 2 seconds
- **Offline Operation**: Functions independently without network connection

### Dashboard (Flask Server)
- Web-based monitoring interface
- Real-time data reception from ESP8266
- RESTful API endpoints
- CORS-enabled for cross-origin requests

## 📁 Project Structure

```
S.L.O.P.E-Dashboard/
├── firmware/               # ESP8266 Arduino firmware
│   ├── SLOPE_ESP8266/
│   │   └── SLOPE_ESP8266.ino    # Main firmware code
│   ├── README.md          # Detailed firmware documentation
│   ├── QUICKSTART.md      # Quick setup guide
│   ├── WIRING.md          # Hardware wiring instructions
│   └── config.h.example   # Configuration template
├── app.py                 # Flask server application
├── static/                # Static assets (CSS, JS)
├── templates/             # HTML templates
└── requirements.txt       # Python dependencies
```

## 🚀 Quick Start

### 1. Hardware Setup
See [`firmware/QUICKSTART.md`](firmware/QUICKSTART.md) for detailed step-by-step instructions.

**Quick Reference:**
- Connect MPU6050 to I2C pins (D1, D2)
- Connect soil sensor to A0
- Connect LEDs to D0, D3, D7, D8
- Connect buzzer to RX pin
- Full wiring guide: [`firmware/WIRING.md`](firmware/WIRING.md)

### 2. Arduino Firmware Upload

1. Install Arduino IDE and ESP8266 board support
2. Install ArduinoJson library
3. Open `firmware/SLOPE_ESP8266/SLOPE_ESP8266.ino`
4. Update WiFi credentials and server URL
5. Upload to ESP8266

Detailed instructions: [`firmware/README.md`](firmware/README.md)

### 3. Server Setup

```bash
# Install Python dependencies
pip install -r requirements.txt

# (Optional) Configure environment for production
cp .env.example .env
# Edit .env and set FLASK_DEBUG=0, FLASK_HOST=127.0.0.1

# Run Flask server
python app.py
```

Server will start on `http://0.0.0.0:5000` (development) or configured host/port

### 4. Test the System

1. Open Serial Monitor (115200 baud) to view ESP8266 output
2. Check server terminal for incoming data
3. LEDs should indicate current risk level
4. Tilt device to test accelerometer

## 📊 Data Format

ESP8266 sends JSON data every 2 seconds:

```json
{
  "soilMoisture": 23.5,
  "soilDigital": 0,
  "accelX": 0.023,
  "accelY": -0.015,
  "accelZ": 0.982,
  "tiltAngle": 2.3,
  "vibration": false,
  "riskLevel": "GREEN",
  "riskScore": 34,
  "timestamp": 123456
}
```

## 🎯 Risk Levels

| Level | Soil Moisture | Vibration | Tilt Angle | LED | Buzzer |
|-------|--------------|-----------|------------|-----|--------|
| **GREEN** | 0-20% | 0-0.02g | 0-2° | Green | Silent |
| **YELLOW** | 21-35% | 0.03-0.08g | 3-5° | Yellow | Silent |
| **ORANGE** | 36-45% | 0.09-0.20g | 6-10° | Orange | Beep pattern |
| **RED** | 46%+ | >0.20g | >10° | Red | Continuous |
| **CRITICAL** | Time-sustained or rapid change | - | - | Orange | Continuous |

## 🔧 API Endpoints

### POST /api/data
Receives sensor data from ESP8266
```bash
curl -X POST http://localhost:5000/api/data \
  -H "Content-Type: application/json" \
  -d '{"soilMoisture": 25, "riskLevel": "YELLOW"}'
```

### GET /api/data
Returns latest sensor data
```bash
curl http://localhost:5000/api/data
```

## 📖 Documentation

- **[Firmware README](firmware/README.md)** - Complete firmware documentation
- **[Quick Start Guide](firmware/QUICKSTART.md)** - Get running in 30 minutes
- **[Wiring Guide](firmware/WIRING.md)** - Detailed hardware connections
- **[Configuration](firmware/config.h.example)** - Customization options

## ⚠️ Safety Notice

This system is a **monitoring and early warning tool** and should not be used as the sole safety measure. Always:
- Follow official evacuation orders
- Maintain regular system checks
- Use battery backup for reliability
- Test emergency alerts regularly

## 🛠️ Requirements

### Hardware
- ESP8266 (NodeMCU, Wemos D1 Mini, etc.)
- MPU6050 accelerometer module
- Soil moisture sensor
- SW-420 vibration sensor
- 4x LEDs (Green, Yellow, Orange, Red)
- Active buzzer
- Resistors, wires, breadboard

### Software
- Arduino IDE 1.8.19+
- Python 3.7+
- Flask 2.0+

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## 📝 License

This project is open source and available for landslide monitoring and prevention applications.

## 🔗 Resources

- [ESP8266 Documentation](https://arduino-esp8266.readthedocs.io/)
- [MPU6050 Datasheet](https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/)
- [Flask Documentation](https://flask.palletsprojects.com/)

---

**Built for landslide early warning and community safety** 🏔️🛡️