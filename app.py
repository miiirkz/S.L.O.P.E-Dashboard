from flask import Flask, request, jsonify
import json

app = Flask(__name__)

@app.route('/soil-moisture', methods=['GET'])
def get_soil_moisture():
    # Logic to fetch soil moisture data
    data = {'moisture': 35}  # Sample data
    return jsonify(data)

if __name__ == '__main__':
    app.run(debug=True)