# Testing Guide - S.L.O.P.E ESP32 Firmware

Complete testing procedures for the ESP32 landslide monitoring system.

## Pre-Testing Checklist

Before starting tests, ensure:
- [ ] All hardware components are connected correctly (see WIRING.md)
- [ ] All required libraries are installed (see LIBRARIES.md)
- [ ] WiFi credentials are configured in the firmware
- [ ] ESP32 is connected to computer via USB
- [ ] Serial Monitor is set to 115200 baud
- [ ] Flask backend is running (if testing WiFi connectivity)

## Test 1: Serial Monitor Initialization Test

**Objective**: Verify all components initialize correctly.

### Procedure:
1. Upload firmware to ESP32
2. Open Serial Monitor (115200 baud)
3. Press RESET button on ESP32
4. Observe initialization messages

### Expected Output:
```
=== S.L.O.P.E ESP32 Firmware Starting ===
Initializing pins...
Pins initialized.
Initializing TFT display...
TFT display initialized.
Initializing MPU6050...
MPU6050 initialized successfully.
Soil moisture test read: [value]
Soil moisture sensor initialized.
Vibration test read: [value]
Vibration sensor initialized.
Initializing WiFi...
WiFi connected!
IP address: [IP]
=== Initialization Complete ===
```

### Pass Criteria:
- ✓ All initialization messages appear
- ✓ No "ERROR" messages
- ✓ MPU6050 shows "OK"
- ✓ WiFi shows IP address or "OFFLINE MODE"

### Troubleshooting:
- **"MPU6050 not found"**: Check I2C wiring (SDA=21, SCL=22)
- **"WiFi connection failed"**: Verify SSID/password, check signal strength
- **No serial output**: Check baud rate (must be 115200)

---

## Test 2: TFT Display Test

**Objective**: Verify TFT display shows startup screen and updates correctly.

### Procedure:
1. Upload firmware and reset ESP32
2. Observe TFT display during startup
3. Wait for main display screen

### Expected Output:

**Startup Screen (first 2 seconds):**
- Title: "S.L.O.P.E Dashboard"
- Initialization status messages
- Green checkmarks for successful sensors
- Red errors for failed sensors
- WiFi status

**Main Screen (after startup):**
- Title: "S.L.O.P.E Monitor"
- Soil Moisture with % and color
- Acceleration with g value and color
- Tilt Angle with degrees and color
- Vibration status (NONE/DETECTED)
- Risk level (LOW/MODERATE/HIGH/CRITICAL) with color
- WiFi status
- Uptime counter

### Pass Criteria:
- ✓ Display shows clear text and values
- ✓ Display updates every 2 seconds
- ✓ Colors match risk levels
- ✓ All sensor readings visible
- ✓ No screen flickering or artifacts

### Troubleshooting:
- **Blank screen**: Check power, backlight, SPI connections
- **Garbled display**: Verify TFT pins match code
- **No updates**: Check main loop execution in Serial Monitor
- **Wrong colors**: Display may use different color codes

---

## Test 3: Individual Sensor Tests

### Test 3A: Soil Moisture Sensor

**Objective**: Verify soil moisture readings respond to moisture changes.

### Procedure:
1. Note dry sensor reading in Serial Monitor
2. Touch sensor probes with wet finger
3. Observe reading change
4. Remove moisture and observe return to baseline

### Expected Behavior:
- Dry sensor: Low moisture % (0-20%)
- Wet sensor: Higher moisture % (increases)
- Reading updates in real-time
- Color changes based on thresholds:
  - 0-20%: GREEN
  - 21-35%: YELLOW
  - 36-45%: ORANGE
  - 46%+: RED

### Pass Criteria:
- ✓ Readings change with moisture
- ✓ Values are between 0-100%
- ✓ Display updates on TFT
- ✓ Color coding matches thresholds

---

### Test 3B: MPU6050 Accelerometer

**Objective**: Verify acceleration and tilt detection.

### Procedure:
1. Keep ESP32 level and still
2. Note baseline acceleration (~0.0g) and tilt (~0°)
3. Tilt ESP32 to different angles
4. Gently shake ESP32
5. Observe readings in Serial Monitor and TFT

### Expected Behavior:
- **Level position**: 
  - Tilt: 0-2° (GREEN)
  - Acceleration: ~0.0g (GREEN)
- **Tilted position**: 
  - Tilt increases with angle
  - Color changes at thresholds (3°, 6°, 10°)
- **Shaking**: 
  - Acceleration increases
  - Values update in real-time

### Pass Criteria:
- ✓ Tilt angle responds to orientation
- ✓ Acceleration responds to movement
- ✓ Values are reasonable (not NaN or extreme)
- ✓ Color coding matches thresholds
- ✓ Serial Monitor shows X, Y, Z values

### Threshold Reference:
**Tilt:**
- 0-2°: GREEN
- 3-5°: YELLOW
- 6-10°: ORANGE
- >10°: RED

**Acceleration:**
- 0-0.02g: GREEN
- 0.03-0.08g: YELLOW
- 0.09-0.20g: ORANGE
- >0.20g: RED

---

### Test 3C: Vibration Sensor (SW-420)

**Objective**: Verify vibration detection.

### Procedure:
1. Keep ESP32 still
2. Note "NONE" status on display
3. Tap the surface near sensor
4. Observe "DETECTED" status
5. Wait for status to return to "NONE"

### Expected Behavior:
- Still: "NONE" (GREEN on display)
- Vibration: "DETECTED" (RED on display)
- Returns to "NONE" when vibration stops

### Pass Criteria:
- ✓ Responds to vibrations/taps
- ✓ Display shows status change
- ✓ Serial Monitor shows vibration value (0 or 1)
- ✓ Sensitivity is appropriate (adjust pot if needed)

---

## Test 4: LED Status Indicators

**Objective**: Verify only one LED is active and matches risk level.

### Procedure:
1. Create different risk scenarios:
   - **GREEN**: Keep sensors at safe levels
   - **YELLOW**: Increase one sensor to moderate level
   - **ORANGE**: Increase one sensor to high level
   - **RED**: Increase one sensor to critical level

2. For each scenario, verify:
   - Only one LED is lit
   - Correct LED color for risk level
   - Other LEDs are off

### Test Scenarios:

| Scenario | Condition | Expected LED | Risk Level |
|----------|-----------|--------------|------------|
| 1 | All sensors safe | GREEN | LOW |
| 2 | Soil 25% | YELLOW | MODERATE |
| 3 | Tilt 7° | ORANGE | HIGH |
| 4 | Soil 50% | RED | CRITICAL |
| 5 | Multiple moderate | YELLOW | MODERATE |
| 6 | One critical + others safe | RED | CRITICAL |

### Pass Criteria:
- ✓ Only ONE LED active at a time
- ✓ LED color matches risk level
- ✓ LED matches TFT display risk color
- ✓ LED changes when risk level changes
- ✓ Brightness is sufficient

### Troubleshooting:
- **Multiple LEDs on**: Check GPIO configuration
- **No LED on**: Check wiring, resistors, polarity
- **Wrong LED**: Verify GPIO pin assignments
- **Dim LED**: Check resistor value (220Ω recommended)

---

## Test 5: Buzzer Alert Patterns

**Objective**: Verify buzzer produces correct patterns for each risk level.

### Procedure:
1. Test each risk level and time the buzzer pattern

### Expected Patterns:

| Risk Level | Pattern | Timing |
|------------|---------|--------|
| GREEN (0) | Silent | No sound |
| YELLOW (1) | Silent | No sound |
| ORANGE (2) | Intermittent | 500ms ON, 1500ms OFF |
| RED (3) | Rapid | 200ms ON, 200ms OFF |

### Test Steps:
1. **GREEN/YELLOW**: Verify buzzer is silent
2. **ORANGE**: 
   - Count beep duration (~500ms)
   - Count pause duration (~1500ms)
   - Verify pattern repeats consistently
3. **RED**:
   - Count beep duration (~200ms)
   - Verify rapid beeping (~5 beeps per 2 seconds)

### Pass Criteria:
- ✓ Silent for GREEN and YELLOW
- ✓ ORANGE pattern: 500ms beep, 1500ms pause
- ✓ RED pattern: 200ms rapid beeps
- ✓ Pattern is consistent
- ✓ Sound is audible

### Troubleshooting:
- **No sound**: Check wiring, verify active vs passive buzzer
- **Wrong pattern**: Check timing values in code
- **Continuous sound**: Check buzzer state logic
- **Too quiet**: May need transistor amplifier

---

## Test 6: Display Update Timing

**Objective**: Verify display updates every 2 seconds.

### Procedure:
1. Watch the TFT display
2. Use stopwatch or count seconds
3. Observe uptime counter increment
4. Note sensor value refresh rate

### Expected Behavior:
- Display refreshes completely every 2 seconds
- Uptime increments by 2 each refresh
- No flickering or partial updates
- All values update simultaneously

### Pass Criteria:
- ✓ Update interval is 2 seconds (±0.1s)
- ✓ All values refresh together
- ✓ No screen tearing or artifacts
- ✓ Smooth, consistent updates

---

## Test 7: WiFi Connectivity and Data Transmission

**Objective**: Verify ESP32 connects to WiFi and sends data to Flask backend.

### Prerequisites:
1. Flask server running on local network
2. Server IP address configured in firmware
3. ESP32 and server on same network

### Procedure:

**Part A: WiFi Connection**
1. Upload firmware with correct WiFi credentials
2. Open Serial Monitor
3. Reset ESP32
4. Verify WiFi connection messages
5. Note IP address assigned to ESP32

**Part B: Data Transmission**
1. Start Flask server:
   ```bash
   cd /path/to/S.L.O.P.E-Dashboard
   python app.py
   ```
2. Observe Flask console for incoming data
3. Check Serial Monitor for "Data sent successfully" messages
4. Verify data appears every 5 seconds

**Part C: Data Format Verification**
1. Check Flask console output
2. Verify all fields are present:
   - soil_moisture
   - accel_x, accel_y, accel_z
   - accel_magnitude
   - tilt_angle
   - vibration
   - risk_level
   - timestamp

### Expected Flask Output:
```
[2026-02-18 13:14:24] Received sensor data:
  Soil Moisture: 25.5%
  Acceleration: 0.045g
  Tilt Angle: 3.2°
  Vibration: NONE
  Risk Level: 1
```

### Pass Criteria:
- ✓ ESP32 connects to WiFi automatically
- ✓ IP address displayed on Serial Monitor and TFT
- ✓ Data transmitted every 5 seconds
- ✓ Flask receives and logs data correctly
- ✓ JSON format is valid
- ✓ All sensor values are included

### Troubleshooting:
- **WiFi won't connect**: 
  - Check SSID/password
  - Verify 5GHz band or switch to 2.4GHz
  - Check router settings
- **Data not received**:
  - Verify server URL in firmware
  - Check Flask is running and accessible
  - Verify same network
  - Check firewall settings
- **Connection drops**:
  - Check WiFi signal strength
  - Enable auto-reconnect testing

---

## Test 8: Offline Mode (Graceful Degradation)

**Objective**: Verify system works without WiFi connection.

### Procedure:
1. Upload firmware with incorrect WiFi credentials OR turn off WiFi router
2. Reset ESP32
3. Observe system behavior

### Expected Behavior:
- Initialization proceeds normally
- WiFi shows "OFFLINE MODE" on TFT
- All sensors continue reading
- Risk calculation continues
- LEDs and buzzer function normally
- Serial Monitor shows sensor data
- TFT display updates normally

### Pass Criteria:
- ✓ System operates without WiFi
- ✓ No system crashes or freezes
- ✓ All local features work
- ✓ Clear indication of offline status
- ✓ Serial Monitor shows "Running in offline mode"

---

## Test 9: Risk Level Calculation

**Objective**: Verify risk level is calculated correctly as highest of three sensors.

### Test Scenarios:

| Soil | Vibration | Tilt | Expected Risk |
|------|-----------|------|---------------|
| GREEN | GREEN | GREEN | GREEN (0) |
| YELLOW | GREEN | GREEN | YELLOW (1) |
| GREEN | ORANGE | GREEN | ORANGE (2) |
| GREEN | GREEN | RED | RED (3) |
| YELLOW | ORANGE | GREEN | ORANGE (2) |
| YELLOW | YELLOW | YELLOW | YELLOW (1) |
| RED | GREEN | GREEN | RED (3) |

### Procedure:
1. For each scenario, set sensor conditions
2. Verify risk level on:
   - Serial Monitor (numeric value)
   - TFT Display (text: LOW/MODERATE/HIGH/CRITICAL)
   - LED indicator
   - Buzzer pattern

### Pass Criteria:
- ✓ Risk level = max(soil_risk, vib_risk, tilt_risk)
- ✓ All indicators match calculated risk
- ✓ Changes immediately when sensor threshold crossed
- ✓ Consistent across all output methods

---

## Test 10: Serial Debug Output

**Objective**: Verify comprehensive debug information in Serial Monitor.

### Procedure:
1. Open Serial Monitor (115200 baud)
2. Observe periodic debug output (every 3 seconds)
3. Verify all information is present and formatted correctly

### Expected Output Format:
```
======== Sensor Data ========
Soil Moisture: 25.5 %
Acceleration (X,Y,Z): 0.012, -0.023, 1.001
Accel Magnitude: 0.028 g
Tilt Angle: 3.2 degrees
Vibration: NONE
Risk Level: 1 (MODERATE)
WiFi Status: CONNECTED
============================
```

### Pass Criteria:
- ✓ Output appears every 3 seconds
- ✓ All sensor values are included
- ✓ Values are formatted correctly
- ✓ Risk level shows both number and text
- ✓ WiFi status is accurate

---

## Test 11: Long-Term Stability Test

**Objective**: Verify system runs reliably over extended period.

### Procedure:
1. Upload firmware
2. Let system run for 30 minutes minimum
3. Monitor for crashes, freezes, or errors
4. Check memory usage if available

### Observations to Monitor:
- Display continues updating
- WiFi connection remains stable
- LEDs respond to changes
- Buzzer patterns work correctly
- No memory leaks (uptime continues increasing)
- No system resets or crashes

### Pass Criteria:
- ✓ No crashes for 30+ minutes
- ✓ All features continue working
- ✓ No memory errors
- ✓ WiFi stays connected (or reconnects if dropped)
- ✓ Display updates consistently

---

## Test 12: Integration Test (Complete System)

**Objective**: Verify all components work together correctly.

### Procedure:
1. Set up complete system with all components
2. Start Flask backend server
3. Upload and run firmware
4. Simulate different risk scenarios
5. Verify all responses

### Test Flow:
1. **Startup**: All components initialize
2. **Safe condition**: GREEN LED, silent, low risk on display
3. **Moderate condition**: YELLOW LED, silent, moderate risk
4. **High condition**: ORANGE LED, slow beep, high risk
5. **Critical condition**: RED LED, rapid beep, critical risk
6. **Data logging**: Flask receives all data
7. **Display**: TFT shows all current values
8. **Recovery**: Return to safe, verify system resets properly

### Pass Criteria:
- ✓ All subsystems function correctly
- ✓ Real-time response to sensor changes
- ✓ Consistent behavior across all outputs
- ✓ Data successfully transmitted and logged
- ✓ No interference between components

---

## Troubleshooting Common Issues

### Serial Monitor Shows Garbage
- **Solution**: Set baud rate to 115200
- **Check**: USB cable quality

### Display Flickers
- **Solution**: Check power supply capacity
- **Try**: External 5V 2A power adapter
- **Check**: Decoupling capacitors

### Sensors Give Constant Max/Min Values
- **Solution**: Check sensor connections
- **Verify**: Correct analog/digital pins
- **Test**: Sensor with multimeter

### LEDs Don't Change
- **Check**: Risk calculation thresholds
- **Verify**: GPIO pin definitions
- **Test**: Manual LED control

### WiFi Won't Connect
- **Try**: 2.4GHz network instead of 5GHz
- **Check**: SSID/password for typos
- **Verify**: ESP32 WiFi capability
- **Test**: ESP32 WiFi example sketch

### System Crashes/Resets
- **Check**: Power supply adequacy
- **Verify**: No short circuits
- **Monitor**: Serial output for errors
- **Test**: Reduce components one by one

---

## Performance Benchmarks

Expected performance metrics:

| Metric | Target | Acceptable Range |
|--------|--------|------------------|
| Display update | 2.0s | 1.8s - 2.2s |
| Data transmission | 5.0s | 4.5s - 5.5s |
| Serial debug | 3.0s | 2.8s - 3.2s |
| Sensor read rate | 100ms | 80ms - 120ms |
| WiFi latency | <100ms | <500ms |
| Memory usage | <50% | <70% |

---

## Test Results Documentation Template

```
Test Date: _______________
Firmware Version: _________
Tester: __________________

Component Tests:
[ ] Serial Monitor Init    - Pass/Fail: _____
[ ] TFT Display           - Pass/Fail: _____
[ ] Soil Moisture Sensor  - Pass/Fail: _____
[ ] MPU6050              - Pass/Fail: _____
[ ] Vibration Sensor     - Pass/Fail: _____
[ ] LED Indicators       - Pass/Fail: _____
[ ] Buzzer Patterns      - Pass/Fail: _____
[ ] Display Updates      - Pass/Fail: _____
[ ] WiFi Connection      - Pass/Fail: _____
[ ] Data Transmission    - Pass/Fail: _____
[ ] Offline Mode         - Pass/Fail: _____
[ ] Risk Calculation     - Pass/Fail: _____
[ ] Serial Debug         - Pass/Fail: _____
[ ] Long-term Stability  - Pass/Fail: _____
[ ] Integration Test     - Pass/Fail: _____

Issues Found:
_________________________________
_________________________________

Notes:
_________________________________
_________________________________
```

---

## Next Steps After Testing

Once all tests pass:
1. ✓ Document any modifications made
2. ✓ Save test results
3. ✓ Deploy to final hardware
4. ✓ Perform field testing
5. ✓ Monitor real-world performance

## Support

For testing issues:
1. Review wiring diagram (WIRING.md)
2. Check library installation (LIBRARIES.md)
3. Verify firmware configuration
4. Check Serial Monitor for errors
5. Test components individually
