# S.L.O.P.E Dashboard

**Soil Landslide Observation & Prediction Engine**

A comprehensive IoT-based landslide monitoring system with real-time sensor data collection, risk assessment, and web-based visualization dashboard.

## 🌟 Features

### Real-time Monitoring
- **Soil Moisture**: Continuous moisture level tracking with percentage display
- **Acceleration**: 3-axis (X, Y, Z) acceleration monitoring using MPU6050
- **Vibration Detection**: Ground vibration monitoring with SW-420 sensor
- **Tilt Angle**: Real-time slope angle calculation and tracking

### Risk Assessment
- **Multi-parameter Analysis**: Combined risk calculation from all sensors
- **Color-coded Risk Levels**:
  - 🟢 **GREEN**: Safe conditions (Score: 0-24)
  - 🟡 **YELLOW**: Caution required (Score: 25-49)
  - 🟠 **ORANGE**: Warning - potential hazard (Score: 50-99)
  - 🔴 **RED**: Critical - immediate attention required (Score: 100)

### Advanced Analytics
- **Time-sustained Analysis**: Detects prolonged high moisture conditions
- **Rate of Change Detection**: Identifies sudden tilt variations
- **Historical Tracking**: Stores up to 100 sensor readings with statistics
- **Alarm System**: Configurable thresholds with acknowledgment

### Professional Dashboard
- **Real-time Data Cards**: Live sensor readings with status indicators
- **Interactive Charts**: Chart.js visualizations for trend analysis
- **Risk Gauge**: Visual risk score display (0-100)
- **Mobile Responsive**: Optimized for desktop, tablet, and mobile devices
- **Auto-refresh**: Updates every 5 seconds automatically

## 🏗️ System Architecture

```
┌─────────────────┐
│   ESP8266       │
│   + Sensors     │
│   (Hardware)    │
└────────┬────────┘
         │ WiFi/HTTP
         │ JSON Data
         ▼
┌─────────────────┐
│  Flask Backend  │
│  + SQLite DB    │
│  (app.py)       │
└────────┬────────┘
         │ REST API
         │ JSON
         ▼
┌─────────────────┐
│  Web Dashboard  │
│  HTML/CSS/JS    │
│  (Frontend)     │
└─────────────────┘
```

## 📋 Prerequisites

### For Backend (Flask Server)
- Python 3.7 or higher
- pip (Python package manager)

### For Hardware (ESP8266)
- Arduino IDE 1.8.x or 2.x
- ESP8266 board package
- Required Arduino libraries (see arduino_firmware/README.md)

## 🚀 Quick Start

### 1. Setup Flask Backend

```bash
# Clone the repository
git clone https://github.com/miiirkz/S.L.O.P.E-Dashboard.git
cd S.L.O.P.E-Dashboard

# Install dependencies
pip install -r requirements.txt

# Run the server
python app.py
```

The server will start on `http://localhost:5000`

### 2. Access Dashboard

Open your web browser and navigate to:
```
http://localhost:5000
```

**Note**: The dashboard uses Chart.js for data visualization. If you see "Charts require Chart.js library" messages, the CDN may be blocked by your network. The dashboard will still function fully with all data displayed in the sensor cards. For full chart functionality, you can:
- Serve the page from a network that allows CDN access
- Download Chart.js locally and update the script source in `templates/index.html`

### 3. Setup ESP8266 (Optional)

See detailed instructions in [`arduino_firmware/README.md`](arduino_firmware/README.md)

## 📊 API Endpoints

### Data Collection
- `POST /api/sensor-data` - Receive sensor data from ESP8266

### Data Retrieval
- `GET /api/data` - Get last 100 readings with statistics
- `GET /api/risk-level` - Get current risk assessment
- `GET /api/sensor-status` - Get individual sensor risk levels
- `GET /api/risk-history` - Get historical risk data

### Alarm Management
- `GET /api/alarms/config` - Get alarm configuration
- `POST /api/alarms/config` - Update alarm thresholds
- `POST /api/alarms/acknowledge` - Acknowledge an alarm
- `GET /api/alarms/active` - Get active (unacknowledged) alarms
- `GET /api/alarms/history` - Get alarm history

## 🎯 Risk Thresholds

### Soil Moisture
| Level  | Range    | Description |
|--------|----------|-------------|
| GREEN  | 0-20%    | Normal/Dry |
| YELLOW | 21-35%   | Moderate moisture |
| ORANGE | 36-45%   | High moisture |
| RED    | 46-100%  | Critical saturation |

### Vibration
| Level  | Range      | Description |
|--------|------------|-------------|
| GREEN  | 0-0.02g    | Minimal/None |
| YELLOW | 0.03-0.08g | Slight vibration |
| ORANGE | 0.09-0.20g | Significant vibration |
| RED    | >0.20g     | Severe vibration |

### Tilt Angle
| Level  | Range  | Description |
|--------|--------|-------------|
| GREEN  | 0-2°   | Stable |
| YELLOW | 3-5°   | Slight tilt |
| ORANGE | 6-10°  | Moderate tilt |
| RED    | >10°   | Critical tilt |

## 🔧 Configuration

### Alarm Thresholds

Configure thresholds through the web dashboard:
1. Navigate to the "Alarm Configuration" section
2. Adjust sliders for each parameter
3. Enable/disable notifications
4. Click "Save Configuration"

### WiFi Configuration (ESP8266)

Edit `arduino_firmware/slope_monitor/slope_monitor.ino`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://YOUR_SERVER_IP:5000/api/sensor-data";
```

## 🎨 Dashboard Features

### Sensor Data Cards
- Live readings for all sensors
- Color-coded status indicators
- Unit labels for clarity

### Charts
1. **Soil Moisture Over Time**: Line chart with threshold lines
2. **Acceleration Trends**: Multi-line chart (X, Y, Z axes)
3. **Vibration Timeline**: Bar chart for vibration events
4. **Tilt Angle Visualization**: Line chart with fill

### Risk Indicator
- Large visual badge with current risk level
- Animated pulsing effect based on severity
- Semi-circular gauge showing risk score (0-100)

### Alarm System
- Active alarms list with acknowledgment buttons
- Alarm history with timestamps
- Severity-based color coding

## 🛠️ Hardware Setup

### Components Required
- ESP8266 NodeMCU or equivalent
- MPU6050 (Accelerometer/Gyroscope)
- Soil Moisture Sensor (Analog)
- SW-420 Vibration Sensor
- 4x LEDs (Green, Yellow, Orange, Red)
- 1x Buzzer
- Resistors (220Ω for LEDs)
- Jumper wires
- Breadboard or custom PCB

### Wiring Diagram

See [`arduino_firmware/README.md`](arduino_firmware/README.md) for detailed pin configuration.

## 📱 Mobile Support

The dashboard is fully responsive and optimized for:
- Desktop (1920x1080 and above)
- Tablet (768x1024)
- Mobile (375x667 and above)

## 🗄️ Database Schema

### sensor_data table
```sql
- id: INTEGER PRIMARY KEY
- timestamp: TEXT
- moisture: REAL
- accel_x: REAL
- accel_y: REAL
- accel_z: REAL
- vibration: REAL
- tilt: REAL
- risk_level: TEXT
- risk_score: INTEGER
```

### alarm_history table
```sql
- id: INTEGER PRIMARY KEY
- timestamp: TEXT
- alarm_type: TEXT
- severity: TEXT
- message: TEXT
- acknowledged: INTEGER (0 or 1)
```

## 🔒 Security Considerations

- Run Flask with proper firewall rules in production
- Use HTTPS for secure communication
- Implement authentication for API endpoints
- Sanitize all user inputs
- Regular database backups

## 📈 Future Enhancements

- [ ] User authentication system
- [ ] Email/SMS notifications
- [ ] Multiple deployment site support
- [ ] Machine learning prediction models
- [ ] Weather data integration
- [ ] Export data to CSV/Excel
- [ ] Mobile app (iOS/Android)

## 🐛 Troubleshooting

### Server won't start
- Check if port 5000 is available
- Verify Python dependencies are installed
- Check for syntax errors in app.py

### No data in dashboard
- Ensure ESP8266 is connected and sending data
- Check server logs for errors
- Verify database is created (slope_data.db)

### Charts not displaying
- Check browser console for JavaScript errors
- Verify Chart.js CDN is accessible
- Clear browser cache

## 📄 License

This project is open-source and available for educational and research purposes.

## 👥 Contributors

Developed as part of the S.L.O.P.E Dashboard project for landslide monitoring and early warning systems.

## 📞 Support

For issues and questions, please open an issue on the GitHub repository.

---

**⚠️ Important**: This system is designed for research and educational purposes. For production deployment in critical environments, additional testing, validation, and safety measures are required.

