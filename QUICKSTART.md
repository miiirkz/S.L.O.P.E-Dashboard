# Quick Start Guide - S.L.O.P.E Dashboard

Get your landslide monitoring system up and running in 30 minutes!

## What You'll Need

### Hardware
- [ ] ESP32 Development Board
- [ ] ILI9341 TFT Display (2.4" SPI)
- [ ] MPU6050 Accelerometer
- [ ] Soil Moisture Sensor
- [ ] SW-420 Vibration Sensor
- [ ] 4 LEDs (Green, Yellow, Orange, Red)
- [ ] Active Buzzer
- [ ] 4x 220Ω resistors (for LEDs)
- [ ] Breadboard and jumper wires
- [ ] USB cable for ESP32
- [ ] 5V 2A power supply (recommended)

### Software
- [ ] Arduino IDE (1.8.19+ or 2.x)
- [ ] Python 3.7+
- [ ] Computer with WiFi

## Step-by-Step Setup

### Phase 1: Software Preparation (10 minutes)

#### 1. Install Arduino IDE
1. Download from https://www.arduino.cc/en/software
2. Install and launch

#### 2. Add ESP32 Board Support
1. Open **File > Preferences**
2. Add to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to **Tools > Board > Boards Manager**
4. Search "ESP32" and install "esp32 by Espressif Systems"

#### 3. Install Arduino Libraries
Open **Sketch > Include Library > Manage Libraries** and install:
- Adafruit GFX Library
- Adafruit ILI9341
- MPU6050 (by Electronic Cats or Adafruit)
- ArduinoJson

#### 4. Install Python Requirements
```bash
cd S.L.O.P.E-Dashboard
pip install -r requirements.txt
```

### Phase 2: Hardware Assembly (15 minutes)

Follow the wiring guide in [WIRING.md](WIRING.md). Key connections:

**Quick Reference:**
- TFT: CS→15, DC→2, MOSI→23, CLK→18, RST→4, MISO→19, LED→5
- MPU6050: SDA→21, SCL→22
- Soil: Digital→5, Analog→36
- Vibration: →4
- LEDs: Green→0, Yellow→15, Orange→13, Red→12 (with 220Ω resistors)
- Buzzer: →3
- Power: 5V and GND to all components

⚠️ **Important**: Double-check all connections before powering on!

### Phase 3: Firmware Configuration (5 minutes)

1. Open `firmware/slope_esp32/slope_esp32.ino` in Arduino IDE

2. Configure WiFi (lines 65-67):
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";        // Your WiFi name
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // Your WiFi password
   const char* SERVER_URL = "http://192.168.1.100:5000/api/sensor-data"; // Your server IP
   ```

3. Find your computer's IP address:
   - **Windows**: Open CMD, type `ipconfig`, look for IPv4
   - **Mac/Linux**: Open Terminal, type `ifconfig` or `ip addr`
   - Update SERVER_URL with your IP address

4. Select board and port:
   - **Tools > Board > ESP32 Arduino > ESP32 Dev Module**
   - **Tools > Port > [Your ESP32 Port]**

5. Click **Upload** (arrow button)

6. Wait for "Done uploading" message

### Phase 4: Backend Setup (2 minutes)

1. Open new terminal in project directory:
   ```bash
   cd S.L.O.P.E-Dashboard
   ```

2. For development (with debug):
   ```bash
   export FLASK_DEBUG=true  # Linux/Mac
   set FLASK_DEBUG=true     # Windows
   python app.py
   ```

3. For production (secure):
   ```bash
   python app.py
   ```

4. Look for message:
   ```
   * Running on http://0.0.0.0:5000
   ```

### Phase 5: First Test (3 minutes)

#### 1. Check Serial Monitor
1. Open **Tools > Serial Monitor** (set to 115200 baud)
2. Press RESET on ESP32
3. Verify you see:
   ```
   === S.L.O.P.E ESP32 Firmware Starting ===
   ...
   WiFi connected!
   IP address: [ESP32 IP]
   === Initialization Complete ===
   ```

#### 2. Check TFT Display
- Should show "S.L.O.P.E Monitor" title
- Real-time sensor readings
- Risk level indicator
- WiFi status "CONNECTED"

#### 3. Check LED
- One LED should be lit (probably GREEN if sensors are stable)

#### 4. Check Flask Server
In the Flask terminal, you should see:
```
[2026-02-18 13:14:24] Received sensor data:
  Soil Moisture: XX.X%
  Acceleration: X.XXXg
  Tilt Angle: X.X°
  Vibration: NONE
  Risk Level: X
```

#### 5. Test Sensors
- **Soil**: Touch sensor probes → moisture % increases
- **Tilt**: Tilt ESP32 → angle increases, color changes
- **Vibration**: Tap near sensor → "DETECTED" appears
- Watch LED change color based on risk level!

## Troubleshooting Quick Fixes

### "Upload Failed"
- Try different USB cable
- Press and hold BOOT button during upload
- Check correct port selected

### "WiFi connection failed"
- Verify SSID/password are correct
- Ensure WiFi is 2.4GHz (not 5GHz)
- Move ESP32 closer to router

### "MPU6050 not found"
- Check I2C wiring (SDA=21, SCL=22)
- Verify sensor has power (3.3V)
- Try I2C scanner sketch

### Display is blank
- Check backlight connection (LED pin)
- Verify SPI connections
- Try adjusting contrast if possible

### No data on Flask server
- Verify ESP32 shows "WiFi connected"
- Check server IP in firmware matches computer IP
- Ensure ESP32 and computer on same network
- Check firewall isn't blocking port 5000

## What's Next?

### Calibration
1. Test all sensors individually (see [TESTING.md](TESTING.md))
2. Adjust risk thresholds if needed
3. Fine-tune vibration sensor sensitivity

### Deployment
1. Use proper enclosure (weatherproof if outdoor)
2. Secure 5V power supply
3. Mount sensors appropriately:
   - Soil sensor: In ground
   - MPU6050: On stable surface
   - Vibration sensor: On monitoring point
4. Position display for easy viewing

### Monitoring
- Open web browser: `http://YOUR_SERVER_IP:5000`
- Watch Serial Monitor for detailed logs
- Observe LED and buzzer alerts

## Understanding the System

### Risk Levels
| Color | Level | Buzzer | Meaning |
|-------|-------|--------|---------|
| 🟢 GREEN | 0 | Silent | Safe - All normal |
| 🟡 YELLOW | 1 | Silent | Moderate - Monitor closely |
| 🟠 ORANGE | 2 | Slow beep | High - Take precautions |
| 🔴 RED | 3 | Rapid beep | Critical - Evacuate! |

### Data Flow
```
Sensors → ESP32 → WiFi → Flask Server → Dashboard
         ↓
    TFT Display
         ↓
    LED + Buzzer
```

## Getting Help

1. **Check documentation:**
   - [README.md](README.md) - Overview
   - [WIRING.md](firmware/WIRING.md) - Connection details
   - [TESTING.md](firmware/TESTING.md) - Troubleshooting
   - [LIBRARIES.md](firmware/LIBRARIES.md) - Library info

2. **Common issues:**
   - Serial Monitor shows errors → Check specific error message
   - Sensors reading wrong → Verify connections
   - WiFi issues → Check network settings
   - Display problems → Verify SPI connections

3. **Debug tools:**
   - Serial Monitor (115200 baud)
   - Multimeter for checking connections
   - Simple test sketches for individual components

## Success Checklist

- [ ] All libraries installed
- [ ] Hardware connected correctly
- [ ] WiFi configured
- [ ] Firmware uploaded successfully
- [ ] Serial Monitor shows initialization messages
- [ ] TFT display shows sensor data
- [ ] One LED is lit
- [ ] Flask server receives data
- [ ] Sensors respond to changes
- [ ] Risk level changes appropriately

**Congratulations! Your S.L.O.P.E Dashboard is operational! 🎉**

## Next Steps

- Read [TESTING.md](firmware/TESTING.md) for comprehensive testing procedures
- Customize risk thresholds in firmware
- Deploy to your monitoring site
- Set up long-term data logging
- Consider adding battery backup

## Safety Notes

⚠️ **This is a monitoring system, not a prediction system.**
- Always follow official landslide warnings
- Use multiple monitoring methods
- Have evacuation plans ready
- Regular maintenance and calibration required
- Test system regularly

## Support

For questions or issues:
1. Check the documentation
2. Review error messages in Serial Monitor
3. Verify all connections
4. Test components individually

**Happy Monitoring! Stay Safe!**
