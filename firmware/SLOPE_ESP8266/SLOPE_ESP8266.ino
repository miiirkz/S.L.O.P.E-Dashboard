/*
 * S.L.O.P.E - Smart Landslide Observation and Prevention Electronics
 * ESP8266 Firmware for Real-time Landslide Monitoring System
 * 
 * Features:
 * - MPU6050 accelerometer for tilt detection
 * - Soil moisture monitoring
 * - SW-420 vibration detection
 * - Multi-level risk assessment with LED indicators
 * - Buzzer alarm system
 * - WiFi data transmission to Flask server
 * - Advanced analytics (time-sustained, rate of change, hysteresis)
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

// ============================================================================
// PIN DEFINITIONS
// ============================================================================
#define PIN_LED_GREEN    16  // D0 (GPIO16)
#define PIN_LED_YELLOW   0   // D3 (GPIO0)
#define PIN_LED_ORANGE   13  // D7 (GPIO13)
#define PIN_LED_RED      15  // D8 (GPIO15)
#define PIN_BUZZER       3   // RX (GPIO3)
#define PIN_SCL          5   // D1 (GPIO5) - I2C Clock
#define PIN_SDA          4   // D2 (GPIO4) - I2C Data
#define PIN_VIBRATION    2   // D4 (GPIO2) - SW-420
#define PIN_SOIL_DIGITAL 14  // D5 (GPIO14)
#define PIN_SOIL_ANALOG  12  // D6 (GPIO12) - Not used, using A0
#define PIN_SOIL_ADC     A0  // A0 - Analog input for soil moisture

// ============================================================================
// MPU6050 DEFINITIONS
// ============================================================================
#define MPU6050_ADDR     0x68
#define MPU6050_PWR_MGMT 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B

// ============================================================================
// WIFI CONFIGURATION
// ============================================================================
const char* ssid = "YOUR_WIFI_SSID";          // Change this
const char* password = "YOUR_WIFI_PASSWORD";   // Change this
const char* serverUrl = "http://YOUR_SERVER_IP:5000/api/data";  // Change this

// ============================================================================
// TIMING CONSTANTS
// ============================================================================
const unsigned long SENSOR_READ_INTERVAL = 2000;  // 2 seconds
const unsigned long TIME_SUSTAINED_THRESHOLD = 1800000;  // 30 minutes in milliseconds
const unsigned long RATE_CHECK_INTERVAL = 10000;  // 10 seconds for rate of change

// ============================================================================
// RISK LEVEL THRESHOLDS
// ============================================================================
// Soil Moisture (%)
#define SOIL_GREEN_MAX    20.0
#define SOIL_YELLOW_MAX   35.0
#define SOIL_ORANGE_MAX   45.0
#define SOIL_CRITICAL     40.0  // For time-sustained check

// Vibration (g)
#define VIB_GREEN_MAX     0.02
#define VIB_YELLOW_MAX    0.08
#define VIB_ORANGE_MAX    0.20

// Tilt (degrees)
#define TILT_GREEN_MAX    2.0
#define TILT_YELLOW_MAX   5.0
#define TILT_ORANGE_MAX   10.0
#define TILT_CRITICAL_CHANGE 5.0  // For rate of change

// Hysteresis margin (2%)
#define HYSTERESIS_MARGIN 0.02

// ============================================================================
// RISK LEVELS
// ============================================================================
enum RiskLevel {
  RISK_GREEN = 0,
  RISK_YELLOW = 1,
  RISK_ORANGE = 2,
  RISK_RED = 3,
  RISK_CRITICAL = 4
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
// Sensor readings
float accelX = 0.0, accelY = 0.0, accelZ = 0.0;
float tiltAngle = 0.0;
float soilMoisture = 0.0;
int soilDigital = 0;
bool vibrationDetected = false;

// Risk levels
RiskLevel currentRiskLevel = RISK_GREEN;
RiskLevel previousRiskLevel = RISK_GREEN;
int riskScore = 0;

// Timing variables
unsigned long lastSensorRead = 0;
unsigned long lastDataSend = 0;
unsigned long highMoistureStartTime = 0;
bool highMoistureActive = false;

// Rate of change tracking
float previousTilt = 0.0;
unsigned long lastRateCheck = 0;
bool rapidTiltChange = false;

// Buzzer timing
unsigned long lastBuzzerToggle = 0;
bool buzzerState = false;
int buzzerBeepCount = 0;

// WiFi client
WiFiClient wifiClient;

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  delay(100);
  Serial.println("\n\n=== S.L.O.P.E System Starting ===");
  
  // Initialize pins
  initializePins();
  
  // Initialize I2C
  Wire.begin(PIN_SDA, PIN_SCL);
  
  // Initialize MPU6050
  if (!initMPU6050()) {
    Serial.println("ERROR: MPU6050 initialization failed!");
    // Flash red LED to indicate error
    for (int i = 0; i < 10; i++) {
      digitalWrite(PIN_LED_RED, HIGH);
      delay(100);
      digitalWrite(PIN_LED_RED, LOW);
      delay(100);
    }
  }
  
  // Connect to WiFi
  connectWiFi();
  
  // Initial sensor readings
  readAllSensors();
  
  Serial.println("=== System Ready ===\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  unsigned long currentTime = millis();
  
  // Read sensors every 2 seconds
  if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = currentTime;
    
    // Read all sensors
    readAllSensors();
    
    // Calculate risk levels
    calculateRiskLevels();
    
    // Update hardware (LEDs and buzzer)
    updateHardware();
    
    // Send data to server
    sendDataToServer();
    
    // Print debug info
    printDebugInfo();
  }
  
  // Update buzzer pattern (needs frequent updates)
  updateBuzzer();
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, attempting to reconnect...");
    connectWiFi();
  }
  
  delay(10);  // Small delay to prevent watchdog reset
}

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================
void initializePins() {
  // LED pins as outputs
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_ORANGE, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  
  // Buzzer pin as output
  pinMode(PIN_BUZZER, OUTPUT);
  
  // Sensor pins
  pinMode(PIN_VIBRATION, INPUT);
  pinMode(PIN_SOIL_DIGITAL, INPUT);
  
  // Turn off all LEDs initially
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_ORANGE, LOW);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  
  Serial.println("Pins initialized");
}

bool initMPU6050() {
  // Wake up MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(MPU6050_PWR_MGMT);
  Wire.write(0);  // Wake up
  byte error = Wire.endTransmission();
  
  if (error != 0) {
    Serial.print("MPU6050 communication error: ");
    Serial.println(error);
    return false;
  }
  
  delay(100);
  
  // Verify communication
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x75);  // WHO_AM_I register
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 1, true);
  
  if (Wire.available()) {
    byte whoami = Wire.read();
    if (whoami == 0x68) {
      Serial.println("MPU6050 initialized successfully");
      return true;
    } else {
      Serial.print("MPU6050 WHO_AM_I mismatch: 0x");
      Serial.println(whoami, HEX);
      return false;
    }
  }
  
  Serial.println("MPU6050 no response");
  return false;
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed - operating in offline mode");
  }
}

// ============================================================================
// SENSOR READING FUNCTIONS
// ============================================================================
void readAllSensors() {
  readMPU6050();
  readSoilMoisture();
  readVibration();
}

void readMPU6050() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(MPU6050_ACCEL_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 6, true);
  
  if (Wire.available() >= 6) {
    int16_t rawX = Wire.read() << 8 | Wire.read();
    int16_t rawY = Wire.read() << 8 | Wire.read();
    int16_t rawZ = Wire.read() << 8 | Wire.read();
    
    // Convert to G units (assuming ±2g sensitivity, LSB = 16384)
    accelX = rawX / 16384.0;
    accelY = rawY / 16384.0;
    accelZ = rawZ / 16384.0;
    
    // Calculate tilt angle from acceleration
    // Using pitch calculation (rotation around Y axis)
    // tilt = atan2(X, sqrt(Y^2 + Z^2)) * 180/PI
    float magnitude = sqrt(accelY * accelY + accelZ * accelZ);
    if (magnitude > 0.01) {  // Avoid division by zero
      tiltAngle = atan2(accelX, magnitude) * 180.0 / PI;
      tiltAngle = abs(tiltAngle);  // Use absolute value
    }
  }
}

void readSoilMoisture() {
  // Read analog value (0-1023)
  int rawValue = analogRead(PIN_SOIL_ADC);
  
  // Convert to percentage (0-100%)
  // Assuming dry = 1023 (0%), wet = 0 (100%)
  soilMoisture = map(rawValue, 1023, 0, 0, 100);
  soilMoisture = constrain(soilMoisture, 0, 100);
  
  // Read digital pin
  soilDigital = digitalRead(PIN_SOIL_DIGITAL);
}

void readVibration() {
  vibrationDetected = digitalRead(PIN_VIBRATION);
}

// ============================================================================
// RISK CALCULATION FUNCTIONS
// ============================================================================
void calculateRiskLevels() {
  // Calculate individual risk levels
  RiskLevel soilRisk = calculateSoilRisk();
  RiskLevel vibRisk = calculateVibrationRisk();
  RiskLevel tiltRisk = calculateTiltRisk();
  
  // Check for critical conditions
  checkTimeSustainedAnalysis();
  checkRateOfChange();
  
  // Determine combined risk level with hysteresis
  RiskLevel newRisk = determineCombinedRisk(soilRisk, vibRisk, tiltRisk);
  
  // Apply hysteresis to prevent fluttering
  if (shouldApplyHysteresis(newRisk)) {
    newRisk = previousRiskLevel;
  }
  
  previousRiskLevel = currentRiskLevel;
  currentRiskLevel = newRisk;
  
  // Calculate risk score (0-100)
  calculateRiskScore(soilRisk, vibRisk, tiltRisk);
}

RiskLevel calculateSoilRisk() {
  if (soilMoisture <= SOIL_GREEN_MAX) return RISK_GREEN;
  if (soilMoisture <= SOIL_YELLOW_MAX) return RISK_YELLOW;
  if (soilMoisture <= SOIL_ORANGE_MAX) return RISK_ORANGE;
  return RISK_RED;
}

RiskLevel calculateVibrationRisk() {
  // Calculate vibration magnitude in G
  float vibMagnitude = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ) - 1.0;
  vibMagnitude = abs(vibMagnitude);
  
  if (vibMagnitude <= VIB_GREEN_MAX) return RISK_GREEN;
  if (vibMagnitude <= VIB_YELLOW_MAX) return RISK_YELLOW;
  if (vibMagnitude <= VIB_ORANGE_MAX) return RISK_ORANGE;
  return RISK_RED;
}

RiskLevel calculateTiltRisk() {
  if (tiltAngle <= TILT_GREEN_MAX) return RISK_GREEN;
  if (tiltAngle <= TILT_YELLOW_MAX) return RISK_YELLOW;
  if (tiltAngle <= TILT_ORANGE_MAX) return RISK_ORANGE;
  return RISK_RED;
}

void checkTimeSustainedAnalysis() {
  unsigned long currentTime = millis();
  
  // Check if moisture is high
  if (soilMoisture > SOIL_CRITICAL) {
    if (!highMoistureActive) {
      highMoistureActive = true;
      highMoistureStartTime = currentTime;
    } else {
      // Check if it's been high for 30+ minutes
      if (currentTime - highMoistureStartTime >= TIME_SUSTAINED_THRESHOLD) {
        currentRiskLevel = RISK_CRITICAL;
        Serial.println("CRITICAL: High moisture sustained for 30+ minutes!");
      }
    }
  } else {
    highMoistureActive = false;
  }
}

void checkRateOfChange() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastRateCheck >= RATE_CHECK_INTERVAL) {
    float tiltChange = abs(tiltAngle - previousTilt);
    
    if (tiltChange > TILT_CRITICAL_CHANGE) {
      rapidTiltChange = true;
      currentRiskLevel = RISK_CRITICAL;
      Serial.print("CRITICAL: Rapid tilt change detected: ");
      Serial.print(tiltChange);
      Serial.println(" degrees in 10 seconds!");
    } else {
      rapidTiltChange = false;
    }
    
    previousTilt = tiltAngle;
    lastRateCheck = currentTime;
  }
}

RiskLevel determineCombinedRisk(RiskLevel soil, RiskLevel vib, RiskLevel tilt) {
  // RED if any sensor is at RED level
  if (soil == RISK_RED || vib == RISK_RED || tilt == RISK_RED) {
    return RISK_RED;
  }
  
  // Count elevated sensors (YELLOW or ORANGE)
  int elevatedCount = 0;
  if (soil >= RISK_YELLOW) elevatedCount++;
  if (vib >= RISK_YELLOW) elevatedCount++;
  if (tilt >= RISK_YELLOW) elevatedCount++;
  
  // ORANGE if 2+ sensors elevated
  if (elevatedCount >= 2) {
    return RISK_ORANGE;
  }
  
  // YELLOW if any sensor is YELLOW or ORANGE
  if (soil >= RISK_YELLOW || vib >= RISK_YELLOW || tilt >= RISK_YELLOW) {
    return RISK_YELLOW;
  }
  
  // GREEN if all sensors are GREEN
  return RISK_GREEN;
}

bool shouldApplyHysteresis(RiskLevel newRisk) {
  // Only apply hysteresis when transitioning down
  if (newRisk >= currentRiskLevel) {
    return false;
  }
  
  // Calculate thresholds with hysteresis margin
  float soilWithMargin = soilMoisture - (soilMoisture * HYSTERESIS_MARGIN);
  float tiltWithMargin = tiltAngle - (tiltAngle * HYSTERESIS_MARGIN);
  
  // If values are close to threshold, maintain current level
  if (currentRiskLevel == RISK_YELLOW && newRisk == RISK_GREEN) {
    if (soilWithMargin > SOIL_GREEN_MAX || tiltWithMargin > TILT_GREEN_MAX) {
      return true;
    }
  }
  
  if (currentRiskLevel == RISK_ORANGE && newRisk == RISK_YELLOW) {
    if (soilWithMargin > SOIL_YELLOW_MAX || tiltWithMargin > TILT_YELLOW_MAX) {
      return true;
    }
  }
  
  return false;
}

void calculateRiskScore(RiskLevel soil, RiskLevel vib, RiskLevel tilt) {
  // Calculate weighted score (0-100)
  float soilScore = (soilMoisture / 100.0) * 40;  // 40% weight
  float tiltScore = (tiltAngle / 20.0) * 35;       // 35% weight (assuming max 20 degrees)
  float vibScore = (vib * 25.0 / 3.0);             // 25% weight
  
  riskScore = constrain((int)(soilScore + tiltScore + vibScore), 0, 100);
  
  // Boost score if critical
  if (currentRiskLevel == RISK_CRITICAL) {
    riskScore = 100;
  }
}

// ============================================================================
// HARDWARE CONTROL FUNCTIONS
// ============================================================================
void updateHardware() {
  updateLEDs();
  // Buzzer is updated in updateBuzzer() called from main loop
}

void updateLEDs() {
  // Turn off all LEDs first
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_ORANGE, LOW);
  digitalWrite(PIN_LED_RED, LOW);
  
  // Light only ONE LED based on current risk level
  switch (currentRiskLevel) {
    case RISK_GREEN:
      digitalWrite(PIN_LED_GREEN, HIGH);
      break;
    case RISK_YELLOW:
      digitalWrite(PIN_LED_YELLOW, HIGH);
      break;
    case RISK_ORANGE:
    case RISK_CRITICAL:  // CRITICAL uses red LED but with continuous alarm
      digitalWrite(PIN_LED_ORANGE, HIGH);
      break;
    case RISK_RED:
      digitalWrite(PIN_LED_RED, HIGH);
      break;
  }
}

void updateBuzzer() {
  unsigned long currentTime = millis();
  
  switch (currentRiskLevel) {
    case RISK_GREEN:
    case RISK_YELLOW:
      // Silent
      digitalWrite(PIN_BUZZER, LOW);
      buzzerState = false;
      buzzerBeepCount = 0;
      break;
      
    case RISK_ORANGE:
      // Beep-beep-beep pattern (1 second intervals)
      if (currentTime - lastBuzzerToggle >= 1000) {
        if (buzzerBeepCount < 3) {
          // Short beep
          digitalWrite(PIN_BUZZER, HIGH);
          delay(100);
          digitalWrite(PIN_BUZZER, LOW);
          buzzerBeepCount++;
        } else {
          buzzerBeepCount = 0;
        }
        lastBuzzerToggle = currentTime;
      }
      break;
      
    case RISK_RED:
    case RISK_CRITICAL:
      // Continuous alarm buzz
      digitalWrite(PIN_BUZZER, HIGH);
      break;
  }
}

// ============================================================================
// DATA TRANSMISSION FUNCTIONS
// ============================================================================
void sendDataToServer() {
  if (WiFi.status() != WL_CONNECTED) {
    return;  // Skip if not connected
  }
  
  HTTPClient http;
  http.begin(wifiClient, serverUrl);
  http.addHeader("Content-Type", "application/json");
  
  // Create JSON payload
  String jsonPayload = createJsonPayload();
  
  // Send POST request
  int httpResponseCode = http.POST(jsonPayload);
  
  if (httpResponseCode > 0) {
    Serial.print("Data sent successfully. Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error sending data. Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

String createJsonPayload() {
  StaticJsonDocument<512> doc;
  
  // Add sensor readings
  doc["soilMoisture"] = round(soilMoisture * 10) / 10.0;
  doc["soilDigital"] = soilDigital;
  doc["accelX"] = round(accelX * 1000) / 1000.0;
  doc["accelY"] = round(accelY * 1000) / 1000.0;
  doc["accelZ"] = round(accelZ * 1000) / 1000.0;
  doc["tiltAngle"] = round(tiltAngle * 10) / 10.0;
  doc["vibration"] = vibrationDetected;
  
  // Add risk information
  String riskLevelStr;
  switch (currentRiskLevel) {
    case RISK_GREEN: riskLevelStr = "GREEN"; break;
    case RISK_YELLOW: riskLevelStr = "YELLOW"; break;
    case RISK_ORANGE: riskLevelStr = "ORANGE"; break;
    case RISK_RED: riskLevelStr = "RED"; break;
    case RISK_CRITICAL: riskLevelStr = "CRITICAL"; break;
  }
  doc["riskLevel"] = riskLevelStr;
  doc["riskScore"] = riskScore;
  
  // Add timestamp (milliseconds since boot)
  doc["timestamp"] = millis();
  
  // Serialize to string
  String output;
  serializeJson(doc, output);
  
  return output;
}

// ============================================================================
// DEBUG FUNCTIONS
// ============================================================================
void printDebugInfo() {
  Serial.println("===== SENSOR READINGS =====");
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.print("% | Digital: ");
  Serial.println(soilDigital);
  
  Serial.print("Acceleration (G): X=");
  Serial.print(accelX, 3);
  Serial.print(" Y=");
  Serial.print(accelY, 3);
  Serial.print(" Z=");
  Serial.println(accelZ, 3);
  
  Serial.print("Tilt Angle: ");
  Serial.print(tiltAngle);
  Serial.println(" degrees");
  
  Serial.print("Vibration: ");
  Serial.println(vibrationDetected ? "DETECTED" : "None");
  
  Serial.println("\n===== RISK ASSESSMENT =====");
  Serial.print("Risk Level: ");
  switch (currentRiskLevel) {
    case RISK_GREEN: Serial.println("GREEN"); break;
    case RISK_YELLOW: Serial.println("YELLOW"); break;
    case RISK_ORANGE: Serial.println("ORANGE"); break;
    case RISK_RED: Serial.println("RED"); break;
    case RISK_CRITICAL: Serial.println("CRITICAL"); break;
  }
  Serial.print("Risk Score: ");
  Serial.println(riskScore);
  
  Serial.println("===========================\n");
}
