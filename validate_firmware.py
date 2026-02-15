#!/usr/bin/env python3
"""
S.L.O.P.E System Validation Script
Checks firmware code for common issues and validates structure
"""

import os
import re
import json
from pathlib import Path

def check_firmware_file():
    """Check if firmware file exists and is valid"""
    firmware_path = Path("firmware/SLOPE_ESP8266/SLOPE_ESP8266.ino")
    
    if not firmware_path.exists():
        print("❌ Firmware file not found")
        return False
    
    print("✅ Firmware file found")
    
    # Read firmware content
    with open(firmware_path, 'r') as f:
        content = f.read()
    
    # Check for required components
    checks = {
        "Pin definitions": r"#define PIN_LED_GREEN",
        "MPU6050 initialization": r"bool initMPU6050\(\)",
        "WiFi configuration": r"const char\* ssid =",
        "Risk level enum": r"enum RiskLevel",
        "Sensor reading": r"void readAllSensors\(\)",
        "Risk calculation": r"void calculateRiskLevels\(\)",
        "LED control": r"void updateLEDs\(\)",
        "Buzzer control": r"void updateBuzzer\(\)",
        "Data transmission": r"void sendDataToServer\(\)",
        "JSON creation": r"String createJsonPayload\(\)",
        "Time-sustained analysis": r"void checkTimeSustainedAnalysis\(\)",
        "Rate of change": r"void checkRateOfChange\(\)",
        "Hysteresis": r"bool shouldApplyHysteresis\(",
    }
    
    print("\n🔍 Checking firmware components:")
    all_passed = True
    for name, pattern in checks.items():
        if re.search(pattern, content):
            print(f"  ✅ {name}")
        else:
            print(f"  ❌ {name} - NOT FOUND")
            all_passed = False
    
    return all_passed

def check_pin_definitions():
    """Verify all required pins are defined"""
    firmware_path = Path("firmware/SLOPE_ESP8266/SLOPE_ESP8266.ino")
    
    with open(firmware_path, 'r') as f:
        content = f.read()
    
    required_pins = [
        "PIN_LED_GREEN", "PIN_LED_YELLOW", "PIN_LED_ORANGE", "PIN_LED_RED",
        "PIN_BUZZER", "PIN_SCL", "PIN_SDA", "PIN_VIBRATION",
        "PIN_SOIL_DIGITAL", "PIN_SOIL_ADC"
    ]
    
    print("\n📍 Checking pin definitions:")
    all_defined = True
    for pin in required_pins:
        if f"#define {pin}" in content:
            print(f"  ✅ {pin}")
        else:
            print(f"  ❌ {pin} - NOT DEFINED")
            all_defined = False
    
    return all_defined

def check_documentation():
    """Check if all documentation files exist"""
    docs = {
        "Main README": "firmware/README.md",
        "Quick Start": "firmware/QUICKSTART.md",
        "Wiring Guide": "firmware/WIRING.md",
        "Config Example": "firmware/config.h.example",
    }
    
    print("\n📚 Checking documentation:")
    all_exist = True
    for name, path in docs.items():
        if Path(path).exists():
            size = Path(path).stat().st_size
            print(f"  ✅ {name} ({size} bytes)")
        else:
            print(f"  ❌ {name} - NOT FOUND")
            all_exist = False
    
    return all_exist

def check_server_integration():
    """Check Flask server has required endpoints"""
    app_path = Path("app.py")
    
    if not app_path.exists():
        print("❌ app.py not found")
        return False
    
    with open(app_path, 'r') as f:
        content = f.read()
    
    print("\n🌐 Checking server integration:")
    checks = {
        "POST endpoint": "@app.route('/api/data', methods=",
        "JSON handling": "request.json",
        "CORS enabled": "CORS(app)",
    }
    
    all_passed = True
    for name, pattern in checks.items():
        if pattern in content:
            print(f"  ✅ {name}")
        else:
            print(f"  ❌ {name} - NOT FOUND")
            all_passed = False
    
    return all_passed

def check_risk_thresholds():
    """Verify risk level thresholds are properly defined"""
    firmware_path = Path("firmware/SLOPE_ESP8266/SLOPE_ESP8266.ino")
    
    with open(firmware_path, 'r') as f:
        content = f.read()
    
    print("\n⚠️  Checking risk thresholds:")
    thresholds = [
        "SOIL_GREEN_MAX", "SOIL_YELLOW_MAX", "SOIL_ORANGE_MAX",
        "VIB_GREEN_MAX", "VIB_YELLOW_MAX", "VIB_ORANGE_MAX",
        "TILT_GREEN_MAX", "TILT_YELLOW_MAX", "TILT_ORANGE_MAX"
    ]
    
    all_defined = True
    for threshold in thresholds:
        if f"#define {threshold}" in content:
            # Extract the value
            match = re.search(rf"#define {threshold}\s+([\d.]+)", content)
            if match:
                value = match.group(1)
                print(f"  ✅ {threshold} = {value}")
            else:
                print(f"  ✅ {threshold}")
        else:
            print(f"  ❌ {threshold} - NOT DEFINED")
            all_defined = False
    
    return all_defined

def validate_json_structure():
    """Test JSON payload structure"""
    print("\n📦 Validating JSON structure:")
    
    expected_fields = [
        "soilMoisture", "soilDigital", "accelX", "accelY", "accelZ",
        "tiltAngle", "vibration", "riskLevel", "riskScore", "timestamp"
    ]
    
    firmware_path = Path("firmware/SLOPE_ESP8266/SLOPE_ESP8266.ino")
    with open(firmware_path, 'r') as f:
        content = f.read()
    
    all_found = True
    for field in expected_fields:
        if f'doc["{field}"]' in content:
            print(f"  ✅ {field}")
        else:
            print(f"  ❌ {field} - NOT IN JSON")
            all_found = False
    
    return all_found

def main():
    print("=" * 60)
    print("S.L.O.P.E System Validation")
    print("=" * 60)
    
    results = {
        "Firmware file": check_firmware_file(),
        "Pin definitions": check_pin_definitions(),
        "Documentation": check_documentation(),
        "Server integration": check_server_integration(),
        "Risk thresholds": check_risk_thresholds(),
        "JSON structure": validate_json_structure(),
    }
    
    print("\n" + "=" * 60)
    print("VALIDATION RESULTS")
    print("=" * 60)
    
    for test, passed in results.items():
        status = "✅ PASSED" if passed else "❌ FAILED"
        print(f"{test:.<40} {status}")
    
    all_passed = all(results.values())
    
    print("=" * 60)
    if all_passed:
        print("✅ ALL CHECKS PASSED!")
        print("The firmware is ready to be uploaded to ESP8266")
    else:
        print("❌ SOME CHECKS FAILED")
        print("Please review the issues above")
    print("=" * 60)
    
    return 0 if all_passed else 1

if __name__ == "__main__":
    exit(main())
