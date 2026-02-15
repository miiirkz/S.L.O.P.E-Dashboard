/*
 * S.L.O.P.E Dashboard - ESP8266 Firmware
 * Soil Landslide Observation & Prediction Engine
 * 
 * Hardware Configuration:
 * - MPU6050: D1 (SCL), D2 (SDA)
 * - Soil Digital: D5
 * - Soil Analog: D6
 * - SW-420 Vibration: D4
 * - GREEN LED: D0
 * - YELLOW LED: D3
 * - ORANGE LED: D7
 * - RED LED: D8
 * - BUZZER: RX (GPIO3)
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <MPU6050.h>
#include <ArduinoJson.h>

// WiFi Configuration
// WARNING: Do not commit actual credentials to version control!
// Consider using a separate config.h file that is gitignored
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://YOUR_SERVER_IP:5000/api/sensor-data";

// Pin Definitions
#define LED_GREEN   D0  // GPIO16
#define LED_YELLOW  D3  // GPIO0
#define LED_ORANGE  D7  // GPIO13
#define LED_RED     D8  // GPIO15
#define BUZZER      3   // RX (GPIO3)

#define SOIL_DIGITAL D5 // GPIO14
#define SOIL_ANALOG  A0 // Analog pin (D6 externally)
#define VIBRATION    D4 // GPIO2

#define SDA_PIN D2      // GPIO4
#define SCL_PIN D1      // GPIO5

// MPU6050 Sensor
MPU6050 mpu;

// Risk Thresholds
#define MOISTURE_GREEN 20
#define MOISTURE_YELLOW 35
#define MOISTURE_ORANGE 45

#define VIBRATION_GREEN 0.02
#define VIBRATION_YELLOW 0.08
#define VIBRATION_ORANGE 0.20

#define TILT_GREEN 2
#define TILT_YELLOW 5
#define TILT_ORANGE 10

// Time-sustained analysis
#define HIGH_MOISTURE_DURATION 30000  // 30 seconds
unsigned long highMoistureStartTime = 0;
bool highMoistureDetected = false;

// Rate of change detection
float previousTilt = 0;
unsigned long previousTiltTime = 0;
#define TILT_RATE_THRESHOLD 2.0  // degrees per second

// Data collection interval
#define READING_INTERVAL 5000  // 5 seconds
unsigned long lastReadingTime = 0;

// Sensor Data Structure
struct SensorData {
    float moisture;
    float accel_x;
    float accel_y;
    float accel_z;
    float vibration;
    float tilt;
    String risk_level;
    int risk_score;
};

SensorData currentData;

// Function prototypes
void setupWiFi();
void setupSensors();
void readSensors();
float calculateTilt(float ax, float ay, float az);
String calculateRiskLevel();
void controlLEDs(String riskLevel);
void controlBuzzer(String riskLevel);
void sendDataToServer();

void setup() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println("\n\n=================================");
    Serial.println("S.L.O.P.E Dashboard - ESP8266");
    Serial.println("=================================\n");
    
    // Initialize pins
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_ORANGE, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(SOIL_DIGITAL, INPUT);
    pinMode(VIBRATION, INPUT);
    
    // Turn off all LEDs and buzzer initially
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_ORANGE, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(BUZZER, LOW);
    
    // Setup WiFi and sensors
    setupWiFi();
    setupSensors();
    
    Serial.println("System Ready!\n");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Read sensors at specified interval
    if (currentTime - lastReadingTime >= READING_INTERVAL) {
        lastReadingTime = currentTime;
        
        // Read all sensors
        readSensors();
        
        // Calculate risk level
        currentData.risk_level = calculateRiskLevel();
        
        // Control LEDs based on risk
        controlLEDs(currentData.risk_level);
        
        // Control buzzer based on risk
        controlBuzzer(currentData.risk_level);
        
        // Send data to server
        sendDataToServer();
        
        // Debug output
        Serial.println("\n--- Sensor Reading ---");
        Serial.print("Moisture: "); Serial.print(currentData.moisture); Serial.println("%");
        Serial.print("Accel X: "); Serial.print(currentData.accel_x); Serial.println("g");
        Serial.print("Accel Y: "); Serial.print(currentData.accel_y); Serial.println("g");
        Serial.print("Accel Z: "); Serial.print(currentData.accel_z); Serial.println("g");
        Serial.print("Vibration: "); Serial.print(currentData.vibration); Serial.println("g");
        Serial.print("Tilt: "); Serial.print(currentData.tilt); Serial.println("°");
        Serial.print("Risk Level: "); Serial.println(currentData.risk_level);
        Serial.print("Risk Score: "); Serial.println(currentData.risk_score);
        Serial.println("---------------------\n");
    }
}

void setupWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi Connection Failed!");
        Serial.println("Continuing in offline mode...");
    }
}

void setupSensors() {
    Serial.println("Initializing sensors...");
    
    // Initialize I2C
    Wire.begin(SDA_PIN, SCL_PIN);
    
    // Initialize MPU6050
    mpu.initialize();
    
    if (mpu.testConnection()) {
        Serial.println("MPU6050 connected successfully!");
        // Calibrate accelerometer
        mpu.setXAccelOffset(0);
        mpu.setYAccelOffset(0);
        mpu.setZAccelOffset(0);
    } else {
        Serial.println("MPU6050 connection failed!");
    }
}

void readSensors() {
    // Read soil moisture (analog)
    int soilAnalog = analogRead(SOIL_ANALOG);
    currentData.moisture = map(soilAnalog, 1023, 0, 0, 100);  // Convert to percentage
    currentData.moisture = constrain(currentData.moisture, 0, 100);
    
    // Read MPU6050 accelerometer
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    
    // Convert to g (gravity units)
    currentData.accel_x = ax / 16384.0;
    currentData.accel_y = ay / 16384.0;
    currentData.accel_z = az / 16384.0;
    
    // Calculate tilt angle
    currentData.tilt = calculateTilt(currentData.accel_x, currentData.accel_y, currentData.accel_z);
    
    // Detect sudden tilt change (rate of change)
    unsigned long currentTime = millis();
    if (previousTiltTime > 0) {
        float timeDiff = (currentTime - previousTiltTime) / 1000.0;  // seconds
        float tiltRate = abs(currentData.tilt - previousTilt) / timeDiff;
        
        if (tiltRate > TILT_RATE_THRESHOLD) {
            Serial.println("WARNING: Sudden tilt change detected!");
        }
    }
    previousTilt = currentData.tilt;
    previousTiltTime = currentTime;
    
    // Read vibration sensor (SW-420)
    int vibrationReading = digitalRead(VIBRATION);
    
    // Calculate vibration magnitude from accelerometer
    float accelMagnitude = sqrt(
        currentData.accel_x * currentData.accel_x + 
        currentData.accel_y * currentData.accel_y + 
        currentData.accel_z * currentData.accel_z
    ) - 1.0;  // Subtract 1g (gravity)
    
    currentData.vibration = abs(accelMagnitude);
    
    // Enhanced vibration from digital sensor
    if (vibrationReading == HIGH) {
        currentData.vibration = max(currentData.vibration, 0.25);  // Boost vibration reading
    }
    
    // Time-sustained moisture analysis
    if (currentData.moisture >= MOISTURE_ORANGE) {
        if (!highMoistureDetected) {
            highMoistureStartTime = millis();
            highMoistureDetected = true;
        } else if (millis() - highMoistureStartTime >= HIGH_MOISTURE_DURATION) {
            Serial.println("WARNING: Sustained high moisture detected!");
        }
    } else {
        highMoistureDetected = false;
    }
}

float calculateTilt(float ax, float ay, float az) {
    // Calculate tilt angle from vertical (Z-axis)
    // Using atan2 for better accuracy
    float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
    float roll = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / PI;
    
    // Return the maximum tilt from either axis
    return max(abs(pitch), abs(roll));
}

String calculateRiskLevel() {
    int moistureScore = 0;
    int vibrationScore = 0;
    int tiltScore = 0;
    
    // Moisture risk calculation
    if (currentData.moisture <= MOISTURE_GREEN) {
        moistureScore = 0;
    } else if (currentData.moisture <= MOISTURE_YELLOW) {
        moistureScore = 25;
    } else if (currentData.moisture <= MOISTURE_ORANGE) {
        moistureScore = 50;
    } else {
        moistureScore = 100;
    }
    
    // Vibration risk calculation
    if (currentData.vibration <= VIBRATION_GREEN) {
        vibrationScore = 0;
    } else if (currentData.vibration <= VIBRATION_YELLOW) {
        vibrationScore = 25;
    } else if (currentData.vibration <= VIBRATION_ORANGE) {
        vibrationScore = 50;
    } else {
        vibrationScore = 100;
    }
    
    // Tilt risk calculation
    if (currentData.tilt <= TILT_GREEN) {
        tiltScore = 0;
    } else if (currentData.tilt <= TILT_YELLOW) {
        tiltScore = 25;
    } else if (currentData.tilt <= TILT_ORANGE) {
        tiltScore = 50;
    } else {
        tiltScore = 100;
    }
    
    // Overall risk is the highest score
    currentData.risk_score = max(max(moistureScore, vibrationScore), tiltScore);
    
    // Determine risk level
    if (currentData.risk_score >= 100) {
        return "RED";
    } else if (currentData.risk_score >= 50) {
        return "ORANGE";
    } else if (currentData.risk_score >= 25) {
        return "YELLOW";
    } else {
        return "GREEN";
    }
}

void controlLEDs(String riskLevel) {
    // Turn off all LEDs first
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_ORANGE, LOW);
    digitalWrite(LED_RED, LOW);
    
    // Turn on appropriate LED
    if (riskLevel == "GREEN") {
        digitalWrite(LED_GREEN, HIGH);
    } else if (riskLevel == "YELLOW") {
        digitalWrite(LED_YELLOW, HIGH);
    } else if (riskLevel == "ORANGE") {
        digitalWrite(LED_ORANGE, HIGH);
    } else if (riskLevel == "RED") {
        digitalWrite(LED_RED, HIGH);
    }
}

void controlBuzzer(String riskLevel) {
    // Buzzer patterns based on risk level
    static unsigned long lastBuzzerTime = 0;
    static bool buzzerState = false;
    unsigned long currentTime = millis();
    
    if (riskLevel == "GREEN" || riskLevel == "YELLOW") {
        // Silent for GREEN and YELLOW
        digitalWrite(BUZZER, LOW);
        buzzerState = false;
    } else if (riskLevel == "ORANGE") {
        // Beep pattern: 500ms ON, 1500ms OFF
        if (currentTime - lastBuzzerTime >= 2000) {
            lastBuzzerTime = currentTime;
            buzzerState = true;
        }
        
        if (buzzerState && currentTime - lastBuzzerTime >= 500) {
            buzzerState = false;
        }
        
        digitalWrite(BUZZER, buzzerState ? HIGH : LOW);
    } else if (riskLevel == "RED") {
        // Continuous beeping: 200ms ON, 200ms OFF
        if (currentTime - lastBuzzerTime >= 200) {
            lastBuzzerTime = currentTime;
            buzzerState = !buzzerState;
            digitalWrite(BUZZER, buzzerState ? HIGH : LOW);
        }
    }
}

void sendDataToServer() {
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Skipping data transmission.");
        return;
    }
    
    WiFiClient client;
    HTTPClient http;
    
    // Create JSON payload
    StaticJsonDocument<256> doc;
    doc["moisture"] = currentData.moisture;
    doc["accel_x"] = currentData.accel_x;
    doc["accel_y"] = currentData.accel_y;
    doc["accel_z"] = currentData.accel_z;
    doc["vibration"] = currentData.vibration;
    doc["tilt"] = currentData.tilt;
    doc["risk_level"] = currentData.risk_level;
    doc["risk_score"] = currentData.risk_score;
    
    String jsonData;
    serializeJson(doc, jsonData);
    
    // Send HTTP POST request
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
        Serial.print("Data sent successfully! Response code: ");
        Serial.println(httpResponseCode);
        
        String response = http.getString();
        Serial.print("Server response: ");
        Serial.println(response);
    } else {
        Serial.print("Error sending data. Error code: ");
        Serial.println(httpResponseCode);
    }
    
    http.end();
}
