# S.L.O.P.E ESP8266 Wiring Guide

## Component Wiring Diagram

### ESP8266 NodeMCU Pinout Reference

```
                    ┌─────────────┐
                    │   ESP8266   │
                    │  NodeMCU    │
                    ├─────────────┤
              3.3V  │ 3V3     VIN │  5V
              GND   │ GND     GND │  GND
                    │ RST      D0 │  GPIO16 → GREEN LED
                    │ EN       D1 │  GPIO5  → SCL (MPU6050)
                    │ 3V3      D2 │  GPIO4  → SDA (MPU6050)
        Soil ADC ← │ A0       D3 │  GPIO0  → YELLOW LED
                    │ SK       D4 │  GPIO2  → SW-420 Vibration
                    │ SO       D5 │  GPIO14 → Soil Digital
                    │ SC       D6 │  GPIO12 → (Reserved)
                    │ S1       D7 │  GPIO13 → ORANGE LED
                    │ S2       D8 │  GPIO15 → RED LED
                    │ S3       RX │  GPIO3  → BUZZER
                    │ NC       TX │  GPIO1
                    └─────────────┘
```

## Detailed Component Connections

### 1. MPU6050 Accelerometer/Gyroscope

```
MPU6050          ESP8266
---------        --------
VCC    ────────  3.3V
GND    ────────  GND
SCL    ────────  D1 (GPIO5)
SDA    ────────  D2 (GPIO4)
XDA    ────────  (not connected)
XCL    ────────  (not connected)
AD0    ────────  GND (for 0x68 address)
INT    ────────  (optional, not used)
```

**Notes:**
- MPU6050 operates at 3.3V
- AD0 pin determines I2C address (LOW=0x68, HIGH=0x69)
- Pull-up resistors on SDA/SCL may be needed (usually included on module)

### 2. Soil Moisture Sensor

```
Soil Sensor      ESP8266
-----------      --------
VCC    ────────  3.3V or 5V (check sensor specs)
GND    ────────  GND
AO     ────────  A0 (Analog Out)
DO     ────────  D5 (GPIO14) (Digital Out)
```

**Notes:**
- Analog pin A0 reads 0-1023 (0-3.3V)
- Digital pin provides threshold detection
- Sensor probes should be inserted into soil
- Keep electronics module away from moisture

### 3. SW-420 Vibration Sensor

```
SW-420           ESP8266
---------        --------
VCC    ────────  3.3V or 5V
GND    ────────  GND
DO     ────────  D4 (GPIO2)
```

**Notes:**
- Digital output (HIGH when vibration detected)
- Sensitivity can be adjusted via onboard potentiometer
- Module typically has LED indicator

### 4. LED Indicators

Each LED requires a current-limiting resistor (220Ω - 330Ω recommended).

```
         220Ω
D0  ─────[===]─────┤>├───── GND  (GREEN LED)

         220Ω
D3  ─────[===]─────┤>├───── GND  (YELLOW LED)

         220Ω
D7  ─────[===]─────┤>├───── GND  (ORANGE LED)

         220Ω
D8  ─────[===]─────┤>├───── GND  (RED LED)
```

**Notes:**
- Long leg (anode) connects to resistor → GPIO pin
- Short leg (cathode) connects to GND
- Use common cathode configuration
- LED forward voltage typically 2-3V
- Current: ~10-20mA per LED

### 5. Buzzer (Active Buzzer)

```
Buzzer           ESP8266
---------        --------
+      ────────  RX (GPIO3)
-      ────────  GND
```

**For Passive Buzzer (with transistor driver):**

```
             ┌─────┐
RX (GPIO3) ──┤1K   ├─── Base (NPN Transistor)
             └─────┘
                      Collector ─── Buzzer+ ─── 3.3V/5V
                      Emitter   ─── GND
                                    Buzzer- ─── GND
```

**Notes:**
- Active buzzer: Direct connection (with optional resistor)
- Passive buzzer: Needs transistor driver circuit
- RX pin usage: May interfere with Serial debugging (consider alternative pin)
- Add flyback diode across buzzer for protection

## Complete System Wiring

### Power Distribution

```
ESP8266 Power Options:
1. USB Cable → 5V VIN pin
2. External 5V → VIN pin
3. Battery Pack (3.7V LiPo) → 3.3V pin (NOT VIN)

Power Requirements:
- ESP8266: ~80mA (WiFi active), ~20mA (sleep)
- MPU6050: ~3.6mA
- Soil Sensor: ~20mA
- LEDs: 10-20mA each
- Buzzer: 30-50mA
Total: ~200-250mA (recommend 500mA+ supply)
```

### Ground Connections

**Important:** All components must share common ground (GND).

```
ESP8266 GND ─┬─ MPU6050 GND
             ├─ Soil Sensor GND
             ├─ SW-420 GND
             ├─ All LED Cathodes
             └─ Buzzer GND
```

## Breadboard Layout Example

```
                    Power Rails
                    ──────────────
                    + (3.3V/5V)
                    - (GND)

┌──────────────────────────────────────────────┐
│                                              │
│  [MPU6050]     [ESP8266]      [Soil Sensor]│
│   SDA SCL      NodeMCU         AO  DO       │
│    │   │                        │   │       │
│    └───┼────────D2              │   │       │
│        └────────D1              │   │       │
│                                 └───┼───A0  │
│                                     └───D5  │
│                                              │
│  [SW-420]      LEDs           [Buzzer]     │
│    DO           ┤>├┤>├┤>├┤>├    + -        │
│    │            │ │ │ │ │ │     │ │        │
│    └────────D4  │ │ │ │ │ │     └─RX       │
│                 │ │ │ │ │ │                 │
│                D0 D3 D7 D8 └─────GND       │
│                                              │
└──────────────────────────────────────────────┘
```

## Assembly Checklist

- [ ] Connect MPU6050 to I2C pins (D1, D2)
- [ ] Connect soil sensor analog to A0
- [ ] Connect soil sensor digital to D5
- [ ] Connect SW-420 to D4
- [ ] Install GREEN LED on D0 (with resistor)
- [ ] Install YELLOW LED on D3 (with resistor)
- [ ] Install ORANGE LED on D7 (with resistor)
- [ ] Install RED LED on D8 (with resistor)
- [ ] Connect buzzer to RX pin
- [ ] Verify all GND connections
- [ ] Verify all VCC connections (3.3V or 5V as appropriate)
- [ ] Double-check polarity of all components
- [ ] Test with multimeter before powering on

## Testing Procedure

### 1. Power Test
- Connect ESP8266 to USB
- Verify blue LED on ESP8266 blinks briefly
- Measure voltages: 3.3V rail and GND

### 2. LED Test
- Upload simple LED blink sketch to each pin
- Verify each LED lights up correctly

### 3. Sensor Test
- Upload firmware
- Open Serial Monitor (115200 baud)
- Check MPU6050 initialization message
- Verify sensor readings appear

### 4. Complete System Test
- Tilt device to test MPU6050
- Touch soil sensor probes together to test moisture reading
- Tap SW-420 to test vibration detection
- Verify correct LED lights for each risk level
- Check buzzer patterns

## Troubleshooting

### No Serial Output
- Check USB cable (data pins required, not just power)
- Verify baud rate set to 115200
- Try pressing RST button on ESP8266

### MPU6050 Not Responding
- Check SDA/SCL connections (not swapped)
- Verify I2C pull-up resistors (4.7kΩ to 3.3V)
- Try different I2C address (0x69 instead of 0x68)

### LEDs Not Working
- Check polarity (long leg = anode = positive)
- Verify resistor values (220-330Ω)
- Test LED with coin cell battery (3V)
- Measure GPIO pin voltage with multimeter

### Unstable Readings
- Add decoupling capacitors (0.1µF) near sensors
- Use shorter wires for I2C connections
- Ensure solid GND connections
- Add pull-up resistors on I2C lines

### ESP8266 Resets Randomly
- Power supply insufficient (need 500mA+)
- Add bulk capacitor (100-470µF) near VIN
- Check for short circuits
- Disable buzzer temporarily to reduce current draw

## Safety Considerations

⚠️ **Electrical Safety:**
- Always disconnect power before wiring changes
- Double-check polarity before connecting power
- Use proper wire gauge for current requirements
- Insulate exposed connections

⚠️ **Component Protection:**
- ESP8266 is 3.3V logic - do not exceed voltage!
- Use level shifters for 5V sensors if needed
- Add protection diodes for inductive loads
- Keep electronics dry and protected from elements

⚠️ **Field Deployment:**
- Use weatherproof enclosure
- Protect sensor probes from corrosion
- Provide battery backup for power outages
- Regular maintenance and calibration
- Test emergency alerts regularly

## Additional Resources

- ESP8266 Datasheet: https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf
- MPU6050 Datasheet: https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/
- Arduino ESP8266 Core: https://arduino-esp8266.readthedocs.io/
- I2C Tutorial: https://learn.sparkfun.com/tutorials/i2c
