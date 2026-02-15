# S.L.O.P.E System Architecture

## System Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    S.L.O.P.E SYSTEM                         │
│         Smart Landslide Observation and Prevention          │
│                      Electronics                            │
└─────────────────────────────────────────────────────────────┘

┌──────────────────────────────┐         ┌──────────────────────┐
│      HARDWARE LAYER          │         │    SOFTWARE LAYER    │
│       (ESP8266)              │◄───────►│   (Flask Server)     │
└──────────────────────────────┘  WiFi   └──────────────────────┘
```

## Hardware Architecture

```
                    ┌──────────────────────┐
                    │      ESP8266         │
                    │  Microcontroller     │
                    └──────────────────────┘
                             │
        ┌────────────────────┼────────────────────┐
        │                    │                    │
   ┌────▼────┐         ┌────▼────┐        ┌─────▼─────┐
   │ SENSORS │         │  OUTPUTS│        │   COMMS   │
   └─────────┘         └─────────┘        └───────────┘
        │                    │                    │
   ┌────┼────┐          ┌────┼────┐              │
   │    │    │          │    │    │              │
   │    │    │          │    │    │              │
   ▼    ▼    ▼          ▼    ▼    ▼              ▼
 MPU  Soil  SW-420    LEDs Buzzer          WiFi Module
 6050 Moist                                      

```

## Data Flow Diagram

```
┌─────────────┐      ┌──────────────┐      ┌─────────────┐
│   SENSORS   │─────►│   ESP8266    │─────►│   SERVER    │
│             │ Read │              │ HTTP │             │
│ • MPU6050   │      │ • Process    │ POST │ • Store     │
│ • Soil      │      │ • Calculate  │      │ • Display   │
│ • Vibration │      │ • Control    │      │ • Alert     │
└─────────────┘      └──────────────┘      └─────────────┘
                            │
                            ▼
                     ┌─────────────┐
                     │  INDICATORS │
                     │             │
                     │ • LEDs      │
                     │ • Buzzer    │
                     └─────────────┘
```

## Risk Assessment Pipeline

```
┌─────────────────────────────────────────────────────────────┐
│                   SENSOR READINGS                           │
│  ┌───────────┐  ┌───────────┐  ┌───────────┐              │
│  │ Soil: 35% │  │ Tilt: 4°  │  │ Vib: 0.05g│              │
│  └───────────┘  └───────────┘  └───────────┘              │
└─────────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────┐
│              INDIVIDUAL RISK LEVELS                         │
│  ┌───────────┐  ┌───────────┐  ┌───────────┐              │
│  │   YELLOW  │  │   YELLOW  │  │   YELLOW  │              │
│  └───────────┘  └───────────┘  └───────────┘              │
└─────────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────┐
│               ADVANCED ANALYTICS                            │
│  • Time-sustained check  (30 min threshold)                │
│  • Rate of change check  (10 sec window)                   │
│  • Hysteresis filter     (2% margin)                       │
└─────────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────┐
│               COMBINED RISK LEVEL                           │
│                     ORANGE                                  │
│           (Multiple elevated sensors)                       │
└─────────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                  HARDWARE RESPONSE                          │
│  LED: ORANGE ON (others OFF)                               │
│  Buzzer: Beep-beep-beep pattern                            │
└─────────────────────────────────────────────────────────────┘
```

## Pin Mapping Diagram

```
          ESP8266 NodeMCU
          ┌────────────┐
          │            │
  GREEN ──┤ D0   3V3   ├── VCC (Sensors)
          │            │
MPU SCL ──┤ D1   GND   ├── GND (Common)
          │            │
MPU SDA ──┤ D2   RST   │
          │            │
 YELLOW ──┤ D3   EN    │
          │            │
  SW420 ──┤ D4   3V3   │
          │            │
Soil Dig──┤ D5   CLK   │
          │            │
          │ D6   MISO  │
          │            │
 ORANGE ──┤ D7   MOSI  │
          │            │
    RED ──┤ D8   CS    │
          │            │
  Soil ──┤ A0   RX    ├── BUZZER
   ADC    │            │
          │ GND  TX    │
          │            │
          └────────────┘
```

## Risk Level Matrix

```
┌──────────────┬─────────┬──────────┬─────────┬────────────┐
│ Parameter    │  GREEN  │  YELLOW  │ ORANGE  │    RED     │
├──────────────┼─────────┼──────────┼─────────┼────────────┤
│ Soil (%)     │  0-20   │  21-35   │  36-45  │    46+     │
│ Vibration(g) │ 0-0.02  │ 0.03-0.08│0.09-0.20│   >0.20    │
│ Tilt (°)     │  0-2    │   3-5    │  6-10   │    >10     │
├──────────────┼─────────┼──────────┼─────────┼────────────┤
│ LED          │  Green  │  Yellow  │ Orange  │    Red     │
│ Buzzer       │ Silent  │  Silent  │  Beep³  │ Continuous │
└──────────────┴─────────┴──────────┴─────────┴────────────┘

Special: CRITICAL - Time-sustained or rapid change
         LED: Orange | Buzzer: Continuous
```

## Communication Protocol

```
┌──────────────────────────────────────────────────────────┐
│                    JSON PAYLOAD                          │
│                    Every 2 seconds                       │
│                                                          │
│  {                                                       │
│    "soilMoisture": 35.2,        // 0-100%              │
│    "soilDigital": 0,            // 0 or 1              │
│    "accelX": 0.023,             // G units             │
│    "accelY": -0.015,            // G units             │
│    "accelZ": 0.982,             // G units             │
│    "tiltAngle": 2.3,            // degrees             │
│    "vibration": false,          // boolean             │
│    "riskLevel": "YELLOW",       // string              │
│    "riskScore": 45,             // 0-100               │
│    "timestamp": 123456          // milliseconds        │
│  }                                                       │
└──────────────────────────────────────────────────────────┘
                          │
                          ▼ HTTP POST
┌──────────────────────────────────────────────────────────┐
│              Flask Server Endpoint                       │
│              POST /api/data                             │
│              GET  /api/data (returns latest)            │
└──────────────────────────────────────────────────────────┘
```

## State Machine Diagram

```
┌─────────────────────────────────────────────────────────┐
│                    MAIN LOOP                            │
│                   (runs continuously)                   │
└─────────────────────────────────────────────────────────┘
         │
         ▼
  ┌─────────────┐
  │ Wait 2 sec? │───No──► Update Buzzer ──► Check WiFi
  └─────────────┘                                │
         │Yes                                    │
         ▼                                       │
  ┌─────────────┐                               │
  │ Read Sensors│                               │
  └─────────────┘                               │
         │                                       │
         ▼                                       │
  ┌─────────────┐                               │
  │ Calculate   │                               │
  │ Risk Levels │                               │
  └─────────────┘                               │
         │                                       │
         ▼                                       │
  ┌─────────────┐                               │
  │ Update LEDs │                               │
  └─────────────┘                               │
         │                                       │
         ▼                                       │
  ┌─────────────┐                               │
  │ Send to     │                               │
  │ Server      │                               │
  └─────────────┘                               │
         │                                       │
         ▼                                       │
  ┌─────────────┐                               │
  │ Print Debug │                               │
  └─────────────┘                               │
         │                                       │
         └───────────────────────────────────────┘
```

## Advanced Features Timeline

```
Time-Sustained Analysis:
┌────────────────────────────────────────────────────┐
│ Moisture > 40%                                     │
│ ────────────────────────────────────────────────   │
│ 0min      10min      20min      30min      40min   │
│                                  ▲                 │
│                           CRITICAL TRIGGERED       │
└────────────────────────────────────────────────────┘

Rate of Change Detection:
┌────────────────────────────────────────────────────┐
│ Tilt Angle                                         │
│   15° ┤                              ╱             │
│   10° ┤                            ╱               │
│    5° ┤                          ╱                 │
│    0° ├─────────────────────────                   │
│       0s     2s     4s     6s     8s     10s       │
│                                    ▲               │
│                             >5° change in 10s      │
│                             CRITICAL TRIGGERED     │
└────────────────────────────────────────────────────┘

Hysteresis (prevents fluttering):
┌────────────────────────────────────────────────────┐
│ Risk Level                                         │
│ YELLOW ─┬─────────────────┬────                    │
│         │                 │ ▲▼▲▼ Without          │
│ GREEN ──┴─────────────────┘    Hysteresis         │
│                                                    │
│ YELLOW ─┬───────────────────────                   │
│         │                       With 2%            │
│ GREEN ──┴───────────────────────                   │
│                               Hysteresis           │
└────────────────────────────────────────────────────┘
```

## Deployment Topology

```
                    ┌──────────────┐
                    │   Internet   │
                    └──────┬───────┘
                           │
                    ┌──────▼───────┐
                    │  WiFi Router │
                    └──────┬───────┘
                           │
              ┬────────────┼────────────┬
              │                         │
     ┌────────▼────────┐      ┌────────▼────────┐
     │   Flask Server  │      │    ESP8266      │
     │   (Computer)    │      │ (Monitoring     │
     │                 │      │  Device)        │
     │ • Store data    │      │                 │
     │ • Dashboard     │      │ ┌─────────────┐ │
     │ • Alerts        │      │ │  Sensors    │ │
     │                 │      │ └─────────────┘ │
     └─────────────────┘      │ ┌─────────────┐ │
                              │ │ Indicators  │ │
                              │ └─────────────┘ │
                              └─────────────────┘
                              
                              Deployed at
                              monitoring site
```

## Power Requirements

```
┌─────────────────────────────────────────────────────────┐
│                  POWER CONSUMPTION                      │
│                                                         │
│  Component           Current    Notes                  │
│  ─────────────────────────────────────────────────────│
│  ESP8266 (Active)    ~80mA     WiFi transmitting     │
│  ESP8266 (Idle)      ~20mA     Between transmissions  │
│  MPU6050            ~3.6mA     Continuous             │
│  Soil Sensor        ~20mA      Continuous             │
│  SW-420             ~3mA       Standby                │
│  LEDs (each)        ~15mA      One active at a time  │
│  Buzzer             ~40mA      When active            │
│  ─────────────────────────────────────────────────────│
│  Peak Total:        ~180mA     Normal operation       │
│  With Buzzer:       ~220mA     During alarm           │
│                                                         │
│  Recommended Supply: 500mA @ 5V (USB power)           │
│  Battery Option: 3x AA (4.5V) or 18650 Li-ion        │
└─────────────────────────────────────────────────────────┘
```

## File Structure Map

```
S.L.O.P.E-Dashboard/
│
├── firmware/                      # ESP8266 Arduino code
│   ├── SLOPE_ESP8266/
│   │   └── SLOPE_ESP8266.ino     # Main firmware (679 lines)
│   ├── README.md                  # Complete documentation
│   ├── QUICKSTART.md              # 30-min setup guide
│   ├── WIRING.md                  # Hardware connections
│   └── config.h.example           # Configuration template
│
├── app.py                         # Flask server
├── requirements.txt               # Python dependencies
├── .env.example                   # Environment config
│
├── static/                        # Web assets
│   └── style.css
│
├── templates/                     # HTML templates
│   └── index.html
│
├── validate_firmware.py           # Validation script
├── README.md                      # Project overview
├── IMPLEMENTATION_SUMMARY.md      # Requirements checklist
└── .gitignore                     # Git exclusions
```

---

**For detailed implementation, see:**
- Hardware: `firmware/WIRING.md`
- Software: `firmware/README.md`
- Quick Start: `firmware/QUICKSTART.md`
- Requirements: `IMPLEMENTATION_SUMMARY.md`
