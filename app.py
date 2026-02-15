from flask import Flask, jsonify, render_template, request
from flask_cors import CORS
import sqlite3
import os
from datetime import datetime
import json

app = Flask(__name__)
CORS(app)

# Database configuration
DB_PATH = 'slope_data.db'

# Risk thresholds
THRESHOLDS = {
    'moisture': {'green': 20, 'yellow': 35, 'orange': 45},
    'vibration': {'green': 0.02, 'yellow': 0.08, 'orange': 0.20},
    'tilt': {'green': 2, 'yellow': 5, 'orange': 10}
}

# Alarm configuration storage
alarm_config = {
    'moisture_threshold': 45,
    'vibration_threshold': 0.20,
    'tilt_threshold': 10,
    'notification_enabled': True
}

def init_db():
    """Initialize the database with required tables"""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    # Create sensor_data table
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS sensor_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT NOT NULL,
            moisture REAL,
            accel_x REAL,
            accel_y REAL,
            accel_z REAL,
            vibration REAL,
            tilt REAL,
            risk_level TEXT,
            risk_score INTEGER
        )
    ''')
    
    # Create alarm_history table
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS alarm_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT NOT NULL,
            alarm_type TEXT,
            severity TEXT,
            message TEXT,
            acknowledged INTEGER DEFAULT 0
        )
    ''')
    
    conn.commit()
    conn.close()

def get_db_connection():
    """Get a database connection"""
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn

def calculate_risk_level(moisture, vibration, tilt):
    """Calculate overall risk level based on sensor data"""
    scores = []
    
    # Moisture risk
    if moisture <= THRESHOLDS['moisture']['green']:
        moisture_risk = 'GREEN'
        moisture_score = 0
    elif moisture <= THRESHOLDS['moisture']['yellow']:
        moisture_risk = 'YELLOW'
        moisture_score = 25
    elif moisture <= THRESHOLDS['moisture']['orange']:
        moisture_risk = 'ORANGE'
        moisture_score = 50
    else:
        moisture_risk = 'RED'
        moisture_score = 100
    
    # Vibration risk
    if vibration <= THRESHOLDS['vibration']['green']:
        vibration_risk = 'GREEN'
        vibration_score = 0
    elif vibration <= THRESHOLDS['vibration']['yellow']:
        vibration_risk = 'YELLOW'
        vibration_score = 25
    elif vibration <= THRESHOLDS['vibration']['orange']:
        vibration_risk = 'ORANGE'
        vibration_score = 50
    else:
        vibration_risk = 'RED'
        vibration_score = 100
    
    # Tilt risk
    if tilt <= THRESHOLDS['tilt']['green']:
        tilt_risk = 'GREEN'
        tilt_score = 0
    elif tilt <= THRESHOLDS['tilt']['yellow']:
        tilt_risk = 'YELLOW'
        tilt_score = 25
    elif tilt <= THRESHOLDS['tilt']['orange']:
        tilt_risk = 'ORANGE'
        tilt_score = 50
    else:
        tilt_risk = 'RED'
        tilt_score = 100
    
    # Overall risk is the highest risk level
    overall_score = max(moisture_score, vibration_score, tilt_score)
    
    if overall_score >= 100:
        overall_risk = 'RED'
    elif overall_score >= 50:
        overall_risk = 'ORANGE'
    elif overall_score >= 25:
        overall_risk = 'YELLOW'
    else:
        overall_risk = 'GREEN'
    
    return {
        'overall': overall_risk,
        'score': overall_score,
        'moisture': moisture_risk,
        'vibration': vibration_risk,
        'tilt': tilt_risk
    }

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/sensor-data', methods=['POST'])
def receive_sensor_data():
    """Receive sensor data from ESP8266"""
    try:
        data = request.json
        
        # Extract and validate sensor data
        moisture = float(data.get('moisture', 0))
        accel_x = float(data.get('accel_x', 0))
        accel_y = float(data.get('accel_y', 0))
        accel_z = float(data.get('accel_z', 0))
        vibration = float(data.get('vibration', 0))
        tilt = float(data.get('tilt', 0))
        
        # Validate data ranges
        if not (0 <= moisture <= 100):
            return jsonify({'status': 'error', 'message': 'Moisture must be between 0 and 100'}), 400
        if not (-2 <= accel_x <= 2 and -2 <= accel_y <= 2 and -2 <= accel_z <= 2):
            return jsonify({'status': 'error', 'message': 'Acceleration values must be between -2g and 2g'}), 400
        if not (0 <= vibration <= 2):
            return jsonify({'status': 'error', 'message': 'Vibration must be between 0 and 2g'}), 400
        if not (0 <= tilt <= 90):
            return jsonify({'status': 'error', 'message': 'Tilt must be between 0 and 90 degrees'}), 400
        
        # Calculate risk
        risk = calculate_risk_level(moisture, vibration, tilt)
        
        # Store in database
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute('''
            INSERT INTO sensor_data (timestamp, moisture, accel_x, accel_y, accel_z, vibration, tilt, risk_level, risk_score)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
        ''', (datetime.now().isoformat(), moisture, accel_x, accel_y, accel_z, vibration, tilt, risk['overall'], risk['score']))
        
        # Check for alarms
        if risk['overall'] in ['ORANGE', 'RED']:
            cursor.execute('''
                INSERT INTO alarm_history (timestamp, alarm_type, severity, message)
                VALUES (?, ?, ?, ?)
            ''', (datetime.now().isoformat(), 'HIGH_RISK', risk['overall'], f"Risk level {risk['overall']} detected"))
        
        conn.commit()
        conn.close()
        
        return jsonify({'status': 'success', 'risk': risk}), 200
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 400

@app.route('/api/data')
def get_data():
    """Get last 100 sensor readings with statistics"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        # Get last 100 readings
        cursor.execute('''
            SELECT * FROM sensor_data ORDER BY id DESC LIMIT 100
        ''')
        rows = cursor.fetchall()
        
        data = []
        for row in rows:
            data.append({
                'id': row['id'],
                'timestamp': row['timestamp'],
                'moisture': row['moisture'],
                'accel_x': row['accel_x'],
                'accel_y': row['accel_y'],
                'accel_z': row['accel_z'],
                'vibration': row['vibration'],
                'tilt': row['tilt'],
                'risk_level': row['risk_level'],
                'risk_score': row['risk_score']
            })
        
        # Calculate statistics
        if data:
            moistures = [d['moisture'] for d in data if d['moisture'] is not None]
            vibrations = [d['vibration'] for d in data if d['vibration'] is not None]
            tilts = [d['tilt'] for d in data if d['tilt'] is not None]
            
            stats = {
                'moisture': {
                    'avg': sum(moistures) / len(moistures) if moistures else 0,
                    'min': min(moistures) if moistures else 0,
                    'max': max(moistures) if moistures else 0
                },
                'vibration': {
                    'avg': sum(vibrations) / len(vibrations) if vibrations else 0,
                    'min': min(vibrations) if vibrations else 0,
                    'max': max(vibrations) if vibrations else 0
                },
                'tilt': {
                    'avg': sum(tilts) / len(tilts) if tilts else 0,
                    'min': min(tilts) if tilts else 0,
                    'max': max(tilts) if tilts else 0
                }
            }
        else:
            stats = {}
        
        conn.close()
        
        return jsonify({
            'data': list(reversed(data)),  # Return in chronological order
            'statistics': stats,
            'count': len(data)
        })
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/risk-level')
def get_risk_level():
    """Get current risk assessment"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        # Get latest reading
        cursor.execute('''
            SELECT * FROM sensor_data ORDER BY id DESC LIMIT 1
        ''')
        row = cursor.fetchone()
        
        if row:
            result = {
                'risk_level': row['risk_level'],
                'risk_score': row['risk_score'],
                'timestamp': row['timestamp']
            }
        else:
            result = {
                'risk_level': 'GREEN',
                'risk_score': 0,
                'timestamp': datetime.now().isoformat()
            }
        
        conn.close()
        return jsonify(result)
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/sensor-status')
def get_sensor_status():
    """Get individual sensor risk levels"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        # Get latest reading
        cursor.execute('''
            SELECT * FROM sensor_data ORDER BY id DESC LIMIT 1
        ''')
        row = cursor.fetchone()
        
        if row:
            risk = calculate_risk_level(row['moisture'], row['vibration'], row['tilt'])
            result = {
                'moisture': {
                    'value': row['moisture'],
                    'risk': risk['moisture']
                },
                'vibration': {
                    'value': row['vibration'],
                    'risk': risk['vibration']
                },
                'tilt': {
                    'value': row['tilt'],
                    'risk': risk['tilt']
                },
                'acceleration': {
                    'x': row['accel_x'],
                    'y': row['accel_y'],
                    'z': row['accel_z']
                },
                'timestamp': row['timestamp']
            }
        else:
            result = {
                'moisture': {'value': 0, 'risk': 'GREEN'},
                'vibration': {'value': 0, 'risk': 'GREEN'},
                'tilt': {'value': 0, 'risk': 'GREEN'},
                'acceleration': {'x': 0, 'y': 0, 'z': 0},
                'timestamp': datetime.now().isoformat()
            }
        
        conn.close()
        return jsonify(result)
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/risk-history')
def get_risk_history():
    """Get historical risk data"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        # Get last 50 readings
        cursor.execute('''
            SELECT timestamp, risk_level, risk_score FROM sensor_data 
            ORDER BY id DESC LIMIT 50
        ''')
        rows = cursor.fetchall()
        
        history = []
        for row in rows:
            history.append({
                'timestamp': row['timestamp'],
                'risk_level': row['risk_level'],
                'risk_score': row['risk_score']
            })
        
        conn.close()
        return jsonify(list(reversed(history)))
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/alarms/config', methods=['GET', 'POST'])
def alarm_config_endpoint():
    """Get or update alarm configuration"""
    global alarm_config
    
    if request.method == 'POST':
        try:
            data = request.json
            alarm_config.update(data)
            return jsonify({'status': 'success', 'config': alarm_config})
        except Exception as e:
            return jsonify({'status': 'error', 'message': str(e)}), 400
    else:
        return jsonify(alarm_config)

@app.route('/api/alarms/acknowledge', methods=['POST'])
def acknowledge_alarm():
    """Acknowledge an alarm"""
    try:
        data = request.json
        alarm_id = data.get('alarm_id')
        
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute('''
            UPDATE alarm_history SET acknowledged = 1 WHERE id = ?
        ''', (alarm_id,))
        conn.commit()
        conn.close()
        
        return jsonify({'status': 'success'})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 400

@app.route('/api/alarms/active')
def get_active_alarms():
    """Get active (unacknowledged) alarms"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT * FROM alarm_history WHERE acknowledged = 0 
            ORDER BY id DESC LIMIT 20
        ''')
        rows = cursor.fetchall()
        
        alarms = []
        for row in rows:
            alarms.append({
                'id': row['id'],
                'timestamp': row['timestamp'],
                'alarm_type': row['alarm_type'],
                'severity': row['severity'],
                'message': row['message']
            })
        
        conn.close()
        return jsonify(alarms)
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/alarms/history')
def get_alarm_history():
    """Get alarm history"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT * FROM alarm_history 
            ORDER BY id DESC LIMIT 50
        ''')
        rows = cursor.fetchall()
        
        alarms = []
        for row in rows:
            alarms.append({
                'id': row['id'],
                'timestamp': row['timestamp'],
                'alarm_type': row['alarm_type'],
                'severity': row['severity'],
                'message': row['message'],
                'acknowledged': bool(row['acknowledged'])
            })
        
        conn.close()
        return jsonify(alarms)
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 500

# Initialize database on startup
init_db()

if __name__ == '__main__':
    # Note: Set debug=False in production for security
    # Use environment variable to control debug mode
    import os
    debug_mode = os.getenv('FLASK_DEBUG', 'False').lower() == 'true'
    app.run(debug=debug_mode, host='0.0.0.0', port=5000)