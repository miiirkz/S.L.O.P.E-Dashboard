/*
 * S.L.O.P.E Dashboard - ESP32 Firmware
 * 
 * Hardware Components:
 * - TFT Display: ILI9341 2.4" 240x320 (SPI)
 * - MPU6050: Accelerometer/Gyroscope (I2C)
 * - Soil Moisture Sensor: Digital D5, Analog A0
 * - SW-420 Vibration Sensor: D4
 * - Status LEDs: D0 (GREEN), D3 (YELLOW), D7 (ORANGE), D8 (RED)
 * - Buzzer: GPIO3 (RX)
 * - WiFi: 5GHz capable
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ==================== PIN DEFINITIONS ====================
// TFT Display Pins (SPI)
#define TFT_CS    15    // Chip Select
#define TFT_RST   4     // Reset (can share with MCU reset)
#define TFT_DC    2     // Data/Command
#define TFT_MOSI  23    // SDI (MOSI)
#define TFT_CLK   18    // SCK
#define TFT_MISO  19    // SD0 (MISO)
#define TFT_LED   5     // Backlight control

// Sensor Pins
#define SOIL_DIGITAL_PIN  5   // D5
#define SOIL_ANALOG_PIN   36  // A0 (GPIO36 on ESP32)
#define VIBRATION_PIN     4   // D4 (SW-420)

// LED Pins
#define LED_GREEN   0   // D0
#define LED_YELLOW  15  // D3
#define LED_ORANGE  13  // D7
#define LED_RED     12  // D8

// Buzzer Pin
#define BUZZER_PIN  3   // RX/GPIO3

// I2C Pins for MPU6050
#define I2C_SDA     21
#define I2C_SCL     22

// ==================== RISK THRESHOLDS ====================
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

// ==================== GLOBAL OBJECTS ====================
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
MPU6050 mpu;

// ==================== WIFI CONFIGURATION ====================
const char* WIFI_SSID = "YOUR_WIFI_SSID";        // Replace with your WiFi SSID
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // Replace with your WiFi password
const char* SERVER_URL = "http://192.168.1.100:5000/api/sensor-data"; // Replace with your server URL

// ==================== GLOBAL VARIABLES ====================
struct SensorData {
  float soilMoisture;
  float accelX;
  float accelY;
  float accelZ;
  float accelMagnitude;
  float tiltAngle;
  int vibration;
  unsigned long timestamp;
};

SensorData currentData;
int currentRiskLevel = 0; // 0=GREEN, 1=YELLOW, 2=ORANGE, 3=RED
unsigned long lastDisplayUpdate = 0;
unsigned long lastBuzzerChange = 0;
bool buzzerState = false;
bool wifiConnected = false;

// ==================== SETUP FUNCTION ====================
void setup() {
  Serial.begin(115200);
  Serial.println(F("\n=== S.L.O.P.E ESP32 Firmware Starting ==="));
  
  // Initialize pins
  initializePins();
  
  // Initialize TFT Display
  initializeTFT();
  
  // Initialize sensors
  initializeMPU6050();
  initializeSoilMoisture();
  initializeVibrationSensor();
  
  // Initialize WiFi
  initializeWiFi();
  
  // Display startup complete
  displayStartupComplete();
  
  Serial.println(F("=== Initialization Complete ===\n"));
}

// ==================== MAIN LOOP ====================
void loop() {
  // Read all sensors
  readSensors();
  
  // Calculate risk level
  calculateRiskLevel();
  
  // Update LEDs
  updateLEDs();
  
  // Update buzzer
  updateBuzzer();
  
  // Update TFT display every 2 seconds
  if (millis() - lastDisplayUpdate >= 2000) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
  
  // Send data to server if WiFi is connected
  if (wifiConnected) {
    sendDataToServer();
  }
  
  // Print debug info to serial
  printDebugInfo();
  
  delay(100); // Small delay for stability
}

// ==================== INITIALIZATION FUNCTIONS ====================
void initializePins() {
  Serial.println(F("Initializing pins..."));
  
  // LED pins
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_ORANGE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  // Buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Sensor pins
  pinMode(SOIL_DIGITAL_PIN, INPUT);
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(SOIL_ANALOG_PIN, INPUT);
  
  // TFT LED backlight
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH); // Turn on backlight
  
  // Turn off all LEDs initially
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_ORANGE, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println(F("Pins initialized."));
}

void initializeTFT() {
  Serial.println(F("Initializing TFT display..."));
  
  tft.begin();
  tft.setRotation(3); // Landscape mode
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  
  // Display startup message
  tft.setCursor(10, 10);
  tft.println(F("S.L.O.P.E Dashboard"));
  tft.setCursor(10, 40);
  tft.setTextSize(1);
  tft.println(F("Initializing sensors..."));
  
  Serial.println(F("TFT display initialized."));
}

void initializeMPU6050() {
  Serial.println(F("Initializing MPU6050..."));
  
  Wire.begin(I2C_SDA, I2C_SCL);
  
  if (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println(F("ERROR: MPU6050 not found!"));
    tft.setCursor(10, 60);
    tft.setTextColor(ILI9341_RED);
    tft.println(F("MPU6050 ERROR!"));
  } else {
    Serial.println(F("MPU6050 initialized successfully."));
    mpu.calibrateGyro();
    mpu.setThreshold(3);
    tft.setCursor(10, 60);
    tft.setTextColor(ILI9341_GREEN);
    tft.println(F("MPU6050 OK"));
  }
}

void initializeSoilMoisture() {
  Serial.println(F("Initializing Soil Moisture Sensor..."));
  
  // Test read
  int analogValue = analogRead(SOIL_ANALOG_PIN);
  Serial.print(F("Soil moisture test read: "));
  Serial.println(analogValue);
  
  tft.setCursor(10, 80);
  tft.setTextColor(ILI9341_GREEN);
  tft.println(F("Soil Sensor OK"));
  
  Serial.println(F("Soil moisture sensor initialized."));
}

void initializeVibrationSensor() {
  Serial.println(F("Initializing Vibration Sensor..."));
  
  // Test read
  int vibValue = digitalRead(VIBRATION_PIN);
  Serial.print(F("Vibration test read: "));
  Serial.println(vibValue);
  
  tft.setCursor(10, 100);
  tft.setTextColor(ILI9341_GREEN);
  tft.println(F("Vibration Sensor OK"));
  
  Serial.println(F("Vibration sensor initialized."));
}

void initializeWiFi() {
  Serial.println(F("Initializing WiFi..."));
  
  tft.setCursor(10, 120);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(F("Connecting to WiFi..."));
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println(F("\nWiFi connected!"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
    
    tft.setCursor(10, 120);
    tft.fillRect(10, 120, 300, 20, ILI9341_BLACK);
    tft.setTextColor(ILI9341_GREEN);
    tft.print(F("WiFi: "));
    tft.println(WiFi.localIP());
  } else {
    wifiConnected = false;
    Serial.println(F("\nWiFi connection failed. Running in offline mode."));
    
    tft.setCursor(10, 120);
    tft.fillRect(10, 120, 300, 20, ILI9341_BLACK);
    tft.setTextColor(ILI9341_ORANGE);
    tft.println(F("WiFi: OFFLINE MODE"));
  }
}

void displayStartupComplete() {
  delay(2000);
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_GREEN);
  tft.println(F("System Ready!"));
  delay(1000);
}

// ==================== SENSOR READING FUNCTIONS ====================
void readSensors() {
  // Read soil moisture
  int soilAnalog = analogRead(SOIL_ANALOG_PIN);
  currentData.soilMoisture = map(soilAnalog, 4095, 0, 0, 100); // ESP32 ADC is 12-bit (0-4095)
  currentData.soilMoisture = constrain(currentData.soilMoisture, 0, 100);
  
  // Read MPU6050
  Vector rawAccel = mpu.readRawAccel();
  Vector normAccel = mpu.readNormalizeAccel();
  
  currentData.accelX = normAccel.XAxis;
  currentData.accelY = normAccel.YAxis;
  currentData.accelZ = normAccel.ZAxis;
  
  // Calculate acceleration magnitude
  currentData.accelMagnitude = sqrt(
    currentData.accelX * currentData.accelX +
    currentData.accelY * currentData.accelY +
    currentData.accelZ * currentData.accelZ
  ) - 1.0; // Subtract 1g for gravity
  currentData.accelMagnitude = abs(currentData.accelMagnitude);
  
  // Calculate tilt angle (pitch)
  currentData.tiltAngle = atan2(currentData.accelX, 
                                sqrt(currentData.accelY * currentData.accelY + 
                                     currentData.accelZ * currentData.accelZ)) * 180.0 / PI;
  currentData.tiltAngle = abs(currentData.tiltAngle);
  
  // Read vibration sensor (digital)
  currentData.vibration = digitalRead(VIBRATION_PIN);
  
  currentData.timestamp = millis();
}

// ==================== RISK CALCULATION ====================
void calculateRiskLevel() {
  int soilRisk = 0;
  int vibRisk = 0;
  int tiltRisk = 0;
  
  // Calculate soil moisture risk
  if (currentData.soilMoisture <= SOIL_GREEN_MAX) {
    soilRisk = 0; // GREEN
  } else if (currentData.soilMoisture <= SOIL_YELLOW_MAX) {
    soilRisk = 1; // YELLOW
  } else if (currentData.soilMoisture <= SOIL_ORANGE_MAX) {
    soilRisk = 2; // ORANGE
  } else {
    soilRisk = 3; // RED
  }
  
  // Calculate vibration risk
  if (currentData.accelMagnitude <= VIB_GREEN_MAX) {
    vibRisk = 0; // GREEN
  } else if (currentData.accelMagnitude <= VIB_YELLOW_MAX) {
    vibRisk = 1; // YELLOW
  } else if (currentData.accelMagnitude <= VIB_ORANGE_MAX) {
    vibRisk = 2; // ORANGE
  } else {
    vibRisk = 3; // RED
  }
  
  // Calculate tilt risk
  if (currentData.tiltAngle <= TILT_GREEN_MAX) {
    tiltRisk = 0; // GREEN
  } else if (currentData.tiltAngle <= TILT_YELLOW_MAX) {
    tiltRisk = 1; // YELLOW
  } else if (currentData.tiltAngle <= TILT_ORANGE_MAX) {
    tiltRisk = 2; // ORANGE
  } else {
    tiltRisk = 3; // RED
  }
  
  // Overall risk is the highest of the three
  currentRiskLevel = max(max(soilRisk, vibRisk), tiltRisk);
}

// ==================== LED CONTROL ====================
void updateLEDs() {
  // Turn off all LEDs first
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_ORANGE, LOW);
  digitalWrite(LED_RED, LOW);
  
  // Turn on the LED corresponding to current risk level
  switch (currentRiskLevel) {
    case 0:
      digitalWrite(LED_GREEN, HIGH);
      break;
    case 1:
      digitalWrite(LED_YELLOW, HIGH);
      break;
    case 2:
      digitalWrite(LED_ORANGE, HIGH);
      break;
    case 3:
      digitalWrite(LED_RED, HIGH);
      break;
  }
}

// ==================== BUZZER CONTROL ====================
void updateBuzzer() {
  unsigned long currentTime = millis();
  
  switch (currentRiskLevel) {
    case 0: // GREEN - Silent
    case 1: // YELLOW - Silent
      digitalWrite(BUZZER_PIN, LOW);
      buzzerState = false;
      break;
      
    case 2: // ORANGE - 500ms beep, 1500ms pause
      if (buzzerState) {
        if (currentTime - lastBuzzerChange >= 500) {
          digitalWrite(BUZZER_PIN, LOW);
          buzzerState = false;
          lastBuzzerChange = currentTime;
        }
      } else {
        if (currentTime - lastBuzzerChange >= 1500) {
          digitalWrite(BUZZER_PIN, HIGH);
          buzzerState = true;
          lastBuzzerChange = currentTime;
        }
      }
      break;
      
    case 3: // RED - 200ms rapid beep
      if (currentTime - lastBuzzerChange >= 200) {
        buzzerState = !buzzerState;
        digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
        lastBuzzerChange = currentTime;
      }
      break;
  }
}

// ==================== DISPLAY UPDATE ====================
void updateDisplay() {
  tft.fillScreen(ILI9341_BLACK);
  
  // Title
  tft.setCursor(5, 5);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_CYAN);
  tft.println(F("S.L.O.P.E Monitor"));
  
  // Draw separator line
  tft.drawLine(0, 30, 320, 30, ILI9341_WHITE);
  
  // Sensor readings
  tft.setTextSize(1);
  int y = 40;
  
  // Soil Moisture
  tft.setCursor(5, y);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(F("Soil Moisture: "));
  tft.setTextColor(getSoilColor());
  tft.print(currentData.soilMoisture, 1);
  tft.println(F(" %"));
  y += 20;
  
  // Acceleration
  tft.setCursor(5, y);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(F("Acceleration: "));
  tft.setTextColor(getVibrationColor());
  tft.print(currentData.accelMagnitude, 3);
  tft.println(F(" g"));
  y += 20;
  
  // Tilt Angle
  tft.setCursor(5, y);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(F("Tilt Angle: "));
  tft.setTextColor(getTiltColor());
  tft.print(currentData.tiltAngle, 1);
  tft.println(F(" deg"));
  y += 20;
  
  // Vibration Status
  tft.setCursor(5, y);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(F("Vibration: "));
  tft.setTextColor(currentData.vibration ? ILI9341_RED : ILI9341_GREEN);
  tft.println(currentData.vibration ? F("DETECTED") : F("NONE"));
  y += 20;
  
  // Draw separator line
  tft.drawLine(0, y + 5, 320, y + 5, ILI9341_WHITE);
  y += 15;
  
  // Risk Level Display
  tft.setCursor(5, y);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(F("Risk: "));
  tft.setTextColor(getRiskColor());
  tft.println(getRiskText());
  y += 25;
  
  // WiFi Status
  tft.setTextSize(1);
  tft.setCursor(5, y);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(F("WiFi: "));
  tft.setTextColor(wifiConnected ? ILI9341_GREEN : ILI9341_RED);
  tft.println(wifiConnected ? F("CONNECTED") : F("OFFLINE"));
  
  // Timestamp
  tft.setCursor(5, 220);
  tft.setTextColor(ILI9341_DARKGREY);
  tft.print(F("Uptime: "));
  tft.print(millis() / 1000);
  tft.println(F(" sec"));
}

uint16_t getSoilColor() {
  if (currentData.soilMoisture <= SOIL_GREEN_MAX) return ILI9341_GREEN;
  if (currentData.soilMoisture <= SOIL_YELLOW_MAX) return ILI9341_YELLOW;
  if (currentData.soilMoisture <= SOIL_ORANGE_MAX) return ILI9341_ORANGE;
  return ILI9341_RED;
}

uint16_t getVibrationColor() {
  if (currentData.accelMagnitude <= VIB_GREEN_MAX) return ILI9341_GREEN;
  if (currentData.accelMagnitude <= VIB_YELLOW_MAX) return ILI9341_YELLOW;
  if (currentData.accelMagnitude <= VIB_ORANGE_MAX) return ILI9341_ORANGE;
  return ILI9341_RED;
}

uint16_t getTiltColor() {
  if (currentData.tiltAngle <= TILT_GREEN_MAX) return ILI9341_GREEN;
  if (currentData.tiltAngle <= TILT_YELLOW_MAX) return ILI9341_YELLOW;
  if (currentData.tiltAngle <= TILT_ORANGE_MAX) return ILI9341_ORANGE;
  return ILI9341_RED;
}

uint16_t getRiskColor() {
  switch (currentRiskLevel) {
    case 0: return ILI9341_GREEN;
    case 1: return ILI9341_YELLOW;
    case 2: return ILI9341_ORANGE;
    case 3: return ILI9341_RED;
    default: return ILI9341_WHITE;
  }
}

const char* getRiskText() {
  switch (currentRiskLevel) {
    case 0: return "LOW";
    case 1: return "MODERATE";
    case 2: return "HIGH";
    case 3: return "CRITICAL";
    default: return "UNKNOWN";
  }
}

// ==================== WIFI DATA TRANSMISSION ====================
void sendDataToServer() {
  static unsigned long lastSendTime = 0;
  unsigned long currentTime = millis();
  
  // Send data every 5 seconds
  if (currentTime - lastSendTime < 5000) {
    return;
  }
  lastSendTime = currentTime;
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    Serial.println(F("WiFi disconnected. Attempting to reconnect..."));
    WiFi.reconnect();
    return;
  }
  
  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");
  
  // Create JSON payload
  StaticJsonDocument<512> doc;
  doc["soil_moisture"] = currentData.soilMoisture;
  doc["accel_x"] = currentData.accelX;
  doc["accel_y"] = currentData.accelY;
  doc["accel_z"] = currentData.accelZ;
  doc["accel_magnitude"] = currentData.accelMagnitude;
  doc["tilt_angle"] = currentData.tiltAngle;
  doc["vibration"] = currentData.vibration;
  doc["risk_level"] = currentRiskLevel;
  doc["timestamp"] = currentData.timestamp;
  
  String jsonPayload;
  serializeJson(doc, jsonPayload);
  
  // Send POST request
  int httpResponseCode = http.POST(jsonPayload);
  
  if (httpResponseCode > 0) {
    Serial.print(F("Data sent successfully. Response code: "));
    Serial.println(httpResponseCode);
  } else {
    Serial.print(F("Error sending data. Error code: "));
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

// ==================== DEBUG OUTPUT ====================
void printDebugInfo() {
  static unsigned long lastPrintTime = 0;
  unsigned long currentTime = millis();
  
  // Print every 3 seconds
  if (currentTime - lastPrintTime < 3000) {
    return;
  }
  lastPrintTime = currentTime;
  
  Serial.println(F("\n======== Sensor Data ========"));
  Serial.print(F("Soil Moisture: "));
  Serial.print(currentData.soilMoisture, 1);
  Serial.println(F(" %"));
  
  Serial.print(F("Acceleration (X,Y,Z): "));
  Serial.print(currentData.accelX, 3);
  Serial.print(F(", "));
  Serial.print(currentData.accelY, 3);
  Serial.print(F(", "));
  Serial.println(currentData.accelZ, 3);
  
  Serial.print(F("Accel Magnitude: "));
  Serial.print(currentData.accelMagnitude, 3);
  Serial.println(F(" g"));
  
  Serial.print(F("Tilt Angle: "));
  Serial.print(currentData.tiltAngle, 1);
  Serial.println(F(" degrees"));
  
  Serial.print(F("Vibration: "));
  Serial.println(currentData.vibration ? F("DETECTED") : F("NONE"));
  
  Serial.print(F("Risk Level: "));
  Serial.print(currentRiskLevel);
  Serial.print(F(" ("));
  Serial.print(getRiskText());
  Serial.println(F(")"));
  
  Serial.print(F("WiFi Status: "));
  Serial.println(wifiConnected ? F("CONNECTED") : F("OFFLINE"));
  
  Serial.println(F("============================\n"));
}
