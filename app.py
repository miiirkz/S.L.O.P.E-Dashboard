from flask import Flask, jsonify, render_template, request
from flask_cors import CORS
from datetime import datetime

app = Flask(__name__)
CORS(app)

# Store latest sensor data
latest_data = {
    "soilMoisture": 0,
    "soilDigital": 0,
    "accelX": 0,
    "accelY": 0,
    "accelZ": 0,
    "tiltAngle": 0,
    "vibration": False,
    "riskLevel": "GREEN",
    "riskScore": 0,
    "timestamp": 0,
    "receivedAt": None
}

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/data', methods=['GET', 'POST'])
def handle_data():
    global latest_data
    
    if request.method == 'POST':
        # Receive data from ESP8266
        try:
            data = request.json
            latest_data = data
            latest_data['receivedAt'] = datetime.now().isoformat()
            
            print(f"Received data from ESP8266:")
            print(f"  Risk Level: {data.get('riskLevel', 'N/A')}")
            print(f"  Risk Score: {data.get('riskScore', 'N/A')}")
            print(f"  Soil Moisture: {data.get('soilMoisture', 'N/A')}%")
            print(f"  Tilt Angle: {data.get('tiltAngle', 'N/A')}°")
            print(f"  Vibration: {data.get('vibration', 'N/A')}")
            
            return jsonify({"status": "success"}), 200
        except Exception as e:
            print(f"Error processing data: {e}")
            return jsonify({"status": "error", "message": str(e)}), 400
    else:
        # GET request - return latest data for dashboard
        return jsonify(latest_data)

if __name__ == '__main__':
    # Use environment variable to control debug mode
    # For production: export FLASK_DEBUG=0 or FLASK_ENV=production
    # For development: export FLASK_DEBUG=1 or omit (defaults to True)
    import os
    debug_mode = os.environ.get('FLASK_DEBUG', '1') == '1'
    host = os.environ.get('FLASK_HOST', '0.0.0.0')
    port = int(os.environ.get('FLASK_PORT', '5000'))
    
    app.run(debug=debug_mode, host=host, port=port)