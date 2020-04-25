function intToDouble(value, places) {
    // converts int to a double with n decimal places
    position = 1;
    for (i = 0; i < places; i++) {
      position *= 10;
    }
    return (1.00 * value) / position;
}
  
function Decoder(bytes, port) {
    // Decode an uplink message from a buffer
    // (array) of bytes to an object of fields.
    var decoded = {};
  
    if (port === 1) {
      decoded.temperature = intToDouble(bytes[0] + bytes[1] * 256, 2);
      decoded.humidity = intToDouble(bytes[2] + bytes[3] * 256, 2);
      decoded.pressure = intToDouble(bytes[4] + bytes[5] * 256, 1);
      decoded.pm25 = intToDouble(bytes[6] + bytes[7] * 256, 1);
      decoded.pm10 = intToDouble(bytes[8] + bytes[9] * 256, 1);

      var binary_coded_status = bytes[10] + bytes[11];
      decoded.hardwareStatus = {
        "ParticleSensor": {
          "wakeUp": 0,
          "read":0
        },
        "TphSensor": {
          "wakeUp": 0,
          "read": 0
        },
        "BatteryIndicator": {
          "chargeStatus": 0,
          "chargeComplete": 0,
          "lowBattery": 0,
          "timerTemperatureFault": 0
        }
      };

      // Check particle sensor wake-up code
      if (binary_coded_status | 1 == binary_coded_status) {
        decoded.hardwareStatus.ParticleSensor.wakeUp = 1;
      }

      // Check particle sensor read code
      if (binary_coded_status | (1 << 1) == binary_coded_status) {
        decoded.hardwareStatus.ParticleSensor.read = 1;
      }

      // Check tph sensor wake-up code
      if (binary_coded_status | (1 << 5) == binary_coded_status) {
        decoded.hardwareStatus.TphSensor.wakeUp = 1;
      }

      // Check tph sensor read code
      if (binary_coded_status | (1 << 6) == binary_coded_status) {
        decoded.hardwareStatus.TphSensor.read = 1;
      }

      // // Check battery charge output STAT 1/-LBO code
      // if (binary_coded_errors | (1 << 10) == binary_coded_errors) {
      //   decoded.hardwareStatus["Battery-STAT1-LBO"] = 1;
      // }

      // // Check battery charge output STAT2 code
      // if (binary_coded_errors | (1 << 11) == binary_coded_errors) {
      //   decoded.hardwareStatus["Battery-STAT2"] = 1;
      // }

      // // Check battery charge output -PG code
      // if (binary_coded_errors | (1 << 12) == binary_coded_errors) {
      //   decoded.hardwareStatus["Battery-PG"] = 1;
      // }
  
    }
    return decoded;
}