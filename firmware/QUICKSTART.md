# S.L.O.P.E System Quick Start Guide

Get your landslide monitoring system up and running in 30 minutes!

## What You'll Need

### Hardware
- [ ] ESP8266 NodeMCU or Wemos D1 Mini
- [ ] MPU6050 accelerometer module
- [ ] Soil moisture sensor with analog output
- [ ] SW-420 vibration sensor module
- [ ] 4x LEDs (Green, Yellow, Orange, Red)
- [ ] 4x 220Ω resistors (for LEDs)
- [ ] 1x Active buzzer (or passive buzzer with driver)
- [ ] Breadboard and jumper wires
- [ ] USB cable for programming
- [ ] 5V power supply (500mA or higher)

### Software
- [ ] Arduino IDE 1.8.19 or later
- [ ] USB drivers for ESP8266 (CH340 or CP2102)
- [ ] Python 3.7+ with Flask (for server)

## Step-by-Step Setup

### Step 1: Prepare Arduino IDE (5 minutes)

1. **Install Arduino IDE**
   - Download from: https://www.arduino.cc/en/software
   - Install and run

2. **Add ESP8266 Board Support**
   ```
   File → Preferences
   Additional Boards Manager URLs:
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   
   Tools → Board → Boards Manager
   Search: "esp8266"
   Install: "esp8266 by ESP8266 Community"
   ```

3. **Install Required Libraries**
   ```
   Sketch → Include Library → Manage Libraries
   Search and install:
   - ArduinoJson by Benoit Blanchon (version 6.x)
   ```

4. **Select Board**
   ```
   Tools → Board → ESP8266 Boards → NodeMCU 1.0 (ESP-12E Module)
   Tools → Upload Speed → 115200
   Tools → CPU Frequency → 80 MHz
   Tools → Flash Size → 4MB (FS:2MB OTA:~1019KB)
   ```

### Step 2: Wire the Hardware (10 minutes)

Follow the wiring diagram in `firmware/WIRING.md`. Quick reference:

**Power & Ground:**
```
All components VCC → 3.3V (except soil sensor can use 5V)
All components GND → GND (common ground!)
```

**Sensors:**
```
MPU6050: SCL→D1, SDA→D2
Soil Sensor: A0→A0, DO→D5
SW-420: DO→D4
```

**Indicators:**
```
GREEN LED:  D0 ─[220Ω]─┤>├─ GND
YELLOW LED: D3 ─[220Ω]─┤>├─ GND
ORANGE LED: D7 ─[220Ω]─┤>├─ GND
RED LED:    D8 ─[220Ω]─┤>├─ GND
Buzzer:     RX─(+), GND─(-)
```

### Step 3: Configure Firmware (3 minutes)

1. **Open the firmware**
   ```
   File → Open → firmware/SLOPE_ESP8266/SLOPE_ESP8266.ino
   ```

2. **Update WiFi credentials** (lines 48-50)
   ```cpp
   const char* ssid = "YourWiFiName";
   const char* password = "YourWiFiPassword";
   const char* serverUrl = "http://192.168.1.100:5000/api/data";
   ```
   
   Replace:
   - `YourWiFiName` with your WiFi network name
   - `YourWiFiPassword` with your WiFi password
   - `192.168.1.100` with your computer's IP address

3. **Find your computer's IP**
   - Windows: `ipconfig` in Command Prompt
   - Mac/Linux: `ifconfig` or `ip addr` in Terminal
   - Look for IPv4 address (e.g., 192.168.1.100)

### Step 4: Upload Firmware (5 minutes)

1. **Connect ESP8266**
   - Plug USB cable into ESP8266
   - Connect to computer

2. **Select Port**
   ```
   Tools → Port → (select your ESP8266 port)
   Windows: COM3, COM4, etc.
   Mac: /dev/cu.usbserial-xxx
   Linux: /dev/ttyUSB0, etc.
   ```

3. **Upload**
   - Click Upload button (→)
   - Wait for "Done uploading" message
   - You'll see blue LED flash on ESP8266

4. **Open Serial Monitor**
   ```
   Tools → Serial Monitor
   Set baud rate: 115200
   ```
   
   You should see:
   ```
   === S.L.O.P.E System Starting ===
   Pins initialized
   MPU6050 initialized successfully
   Connecting to WiFi...
   WiFi connected!
   IP address: 192.168.1.XXX
   === System Ready ===
   ```

### Step 5: Start Server (3 minutes)

1. **Open terminal in project directory**
   ```bash
   cd /path/to/S.L.O.P.E-Dashboard
   ```

2. **Install Python dependencies**
   ```bash
   pip install flask flask-cors
   ```

3. **Run the server**
   ```bash
   python app.py
   ```
   
   You should see:
   ```
   * Running on http://0.0.0.0:5000
   * Restarting with stat
   * Debugger is active!
   ```

4. **Watch for incoming data**
   
   Every 2 seconds you'll see:
   ```
   Received data from ESP8266:
     Risk Level: GREEN
     Risk Score: 15
     Soil Moisture: 23.5%
     Tilt Angle: 2.1°
     Vibration: False
   ```

### Step 6: Test the System (5 minutes)

1. **GREEN LED Test** (Normal state)
   - System should boot with GREEN LED on
   - Buzzer should be silent
   - Serial shows: Risk Level: GREEN

2. **Tilt Test** (Test MPU6050)
   - Tilt the device beyond 5°
   - LED should change to YELLOW or ORANGE
   - Serial shows tilt angle increasing

3. **Vibration Test** (Test SW-420)
   - Tap the board sharply
   - Should detect vibration spike
   - Risk level may elevate

4. **Moisture Test** (Test soil sensor)
   - Touch sensor probes together (simulates wet)
   - Moisture reading should increase
   - LED may change color

5. **Buzzer Test**
   - Create ORANGE condition (moderate tilt + some moisture)
   - Listen for beep-beep-beep pattern
   - Create RED condition (high tilt)
   - Listen for continuous alarm

## Troubleshooting

### ❌ ESP8266 won't upload
**Solutions:**
- Install CH340 or CP2102 drivers
- Try different USB cable (must have data pins)
- Press and hold FLASH button during upload
- Select correct board and port

### ❌ MPU6050 initialization failed
**Solutions:**
- Check wiring: SDA→D2, SCL→D1
- Verify 3.3V power to MPU6050
- Try different I2C address (change 0x68 to 0x69 in code)
- Add 4.7kΩ pull-up resistors on SDA/SCL

### ❌ WiFi won't connect
**Solutions:**
- Double-check SSID and password (case-sensitive!)
- Ensure 2.4GHz WiFi (ESP8266 doesn't support 5GHz)
- Move closer to router
- System still works offline with local LEDs/buzzer

### ❌ Server doesn't receive data
**Solutions:**
- Check computer's IP address matches serverUrl
- Disable firewall temporarily to test
- Verify both devices on same WiFi network
- Try `http://192.168.1.XXX:5000/api/data` in browser

### ❌ All sensor readings are zero
**Solutions:**
- Check power connections (3.3V and GND)
- Verify sensor modules are powered on
- Test each sensor individually
- Check for loose wires

### ❌ LEDs don't light up
**Solutions:**
- Check LED polarity (long leg = positive)
- Verify 220Ω resistors in series
- Test LED with 3V battery
- Measure GPIO voltage with multimeter

## Next Steps

✅ **System is running!** Now you can:

1. **Monitor in Real-Time**
   - Watch Serial Monitor for sensor readings
   - Check server terminal for received data
   - LEDs show current risk level

2. **Deploy in Field**
   - Place in weatherproof enclosure
   - Position on slope to monitor
   - Bury soil sensor probes in ground
   - Ensure stable mounting for tilt detection

3. **Customize Settings**
   - Adjust risk thresholds in firmware
   - Modify buzzer patterns
   - Change sensor read intervals
   - Fine-tune for your environment

4. **Build Dashboard**
   - Create web interface to view data
   - Add data logging and graphs
   - Set up SMS/email alerts
   - Implement historical analysis

## Safety Reminders

⚠️ **Important:**
- This is a monitoring tool, not a life-safety device
- Follow official evacuation orders
- Regular maintenance required
- Test system weekly
- Battery backup recommended

## Getting Help

📚 **Documentation:**
- Full README: `firmware/README.md`
- Wiring Guide: `firmware/WIRING.md`
- Configuration: `firmware/config.h.example`

🐛 **Issues:**
- Check GitHub Issues
- Include Serial Monitor output
- Describe what you expected vs what happened

## Success Indicators

You'll know everything is working when:
- ✅ Serial Monitor shows sensor readings every 2 seconds
- ✅ Server receives and prints data
- ✅ LED lights based on risk level
- ✅ Tilting device changes LED color
- ✅ Buzzer activates at ORANGE/RED levels
- ✅ System recovers from WiFi disconnection

**Congratulations!** Your S.L.O.P.E landslide monitoring system is operational! 🎉
