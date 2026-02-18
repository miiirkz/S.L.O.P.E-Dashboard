from flask import Flask, jsonify, render_template, request
from flask_cors import CORS
from datetime import datetime
import json

app = Flask(__name__)
CORS(app)

# Store latest sensor data in memory (for simple implementation)
# In production, use a database
latest_sensor_data = {
    "soil_moisture": 0,
    "accel_x": 0,
    "accel_y": 0,
    "accel_z": 0,
    "accel_magnitude": 0,
    "tilt_angle": 0,
    "vibration": 0,
    "risk_level": 0,
    "timestamp": 0,
    "last_updated": None
}

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/data')
def get_data():
    """Get latest sensor data"""
    return jsonify(latest_sensor_data)

@app.route('/api/sensor-data', methods=['POST'])
def receive_sensor_data():
    """Receive sensor data from ESP32"""
    try:
        data = request.get_json()
        
        # Validate required fields
        required_fields = ['soil_moisture', 'accel_x', 'accel_y', 'accel_z', 
                          'accel_magnitude', 'tilt_angle', 'vibration', 'risk_level', 'timestamp']
        
        for field in required_fields:
            if field not in data:
                return jsonify({"error": f"Missing field: {field}"}), 400
        
        # Update latest sensor data
        latest_sensor_data.update(data)
        latest_sensor_data['last_updated'] = datetime.now().isoformat()
        
        # Log received data
        print(f"[{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}] Received sensor data:")
        print(f"  Soil Moisture: {data['soil_moisture']:.1f}%")
        print(f"  Acceleration: {data['accel_magnitude']:.3f}g")
        print(f"  Tilt Angle: {data['tilt_angle']:.1f}°")
        print(f"  Vibration: {'DETECTED' if data['vibration'] else 'NONE'}")
        print(f"  Risk Level: {data['risk_level']}")
        print()
        
        return jsonify({"status": "success", "message": "Data received"}), 200
        
    except Exception as e:
        print(f"Error receiving sensor data: {str(e)}")
        return jsonify({"error": str(e)}), 500

@app.route('/api/sensor-data', methods=['GET'])
def get_sensor_data():
    """Get latest sensor data (alternative endpoint)"""
    return jsonify(latest_sensor_data)

if __name__ == '__main__':
    # Run on all network interfaces to accept ESP32 connections
    app.run(host='0.0.0.0', port=5000, debug=True)