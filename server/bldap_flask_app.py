"""
BLDAP Adruino Server
REFERENCE: https://www.bogotobogo.com/python/python-REST-API-Http-Requests-for-Humans-with-Flask.php
"""
import json
from io import StringIO
from flask import Flask
from flask import jsonify
from flask import request
from datetime import datetime

app = Flask("BLDAP Arduino")
DATA_FILE_PATH='bldap_temperature_data.txt'

@app.route('/', methods=['GET'])
def applicationInfo():
    return jsonify({'application' : 'BLDAP Adruino Server'})

@app.route('/temperature', methods=['GET'])
def returnAll():
    buff = StringIO()
    buff.write('[')
    try:
        start = True
        with open(DATA_FILE_PATH, 'r') as f:
            for d in f.readlines():
                if not start: buff.write(',')
                buff.write(d.strip())
                start = False
    except (FileNotFoundError) as ex:
        print("FileNotFoundError: %s" %(DATA_FILE_PATH))
        return jsonify([])
    buff.write(']')
    data = buff.getvalue()
    return data

@app.route('/temperature', methods=['POST'])
def addTemperature():
    datum = request.get_json()
    t1 = datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
    if type(datum) != dict:
        return jsonify({'error': 'datum must be JSON Object'}), 400
    if "temperature" not in datum:
        return jsonify({'error': 'temperature must be specified'}), 400
    if "humidity" not in datum:
        return jsonify({'error': 'humidity must be specified'}), 400
    if "deviceid" not in datum:
        return jsonify({'error': 'deviceid must be specified'}), 400

    datum['timestamp'] = t1
    with open(DATA_FILE_PATH, 'a') as f:
        f.writelines([json.dumps(datum),'\n'])
    return jsonify(datum)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000, debug=True)
