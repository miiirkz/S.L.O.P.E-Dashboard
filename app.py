from flask import Flask, jsonify, render_template
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

@app.route('/')
def index():
    return render_template('index.html')  # assuming you have an index.html file

@app.route('/api/data')
def get_data():
    data = {"key": "value"}  # replace with your actual data
    return jsonify(data)

if __name__ == '__main__':
    app.run(debug=True)