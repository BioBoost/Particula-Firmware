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
        "wakeUpSuccessful": 0,
        "readSuccessful":0
      },
      "TphSensor": {
        "wakeUpSuccessful": 0,
        "readSuccessful": 0
      },
      "BatteryIndicator": {
        "chargeStatus": 0,
        "chargeComplete": 0,
        "lowBattery": 0,
        "timerTemperatureFault": 0
      }
    };

    // Check particle sensor wake-up code
    if (1 & binary_coded_status) {
      decoded.hardwareStatus.ParticleSensor.wakeUpSuccessful = 1;
    }

    // Check particle sensor read code
    if ((1 << 1) & binary_coded_status) {
      decoded.hardwareStatus.ParticleSensor.readSuccessful = 1;
    }

    // Check tph sensor wake-up code
    if ((1 << 5) & binary_coded_status) {
      decoded.hardwareStatus.TphSensor.wakeUpSuccessful = 1;
    }

    // Check tph sensor read code
    if ((1 << 6) & binary_coded_status) {
      decoded.hardwareStatus.TphSensor.readSuccessful = 1;
    }

    // // Check battery charge output STAT 1/-LBO code
    // if ((1 << 10) & binary_coded_status) {
    //   decoded.hardwareStatus["Battery-STAT1-LBO"] = 1;
    // }

    // // Check battery charge output STAT2 code
    // if ((1 << 11) & binary_coded_status) {
    //   decoded.hardwareStatus["Battery-STAT2"] = 1;
    // }

    // // Check battery charge output -PG code
    // if ((1 << 12) & binary_coded_status) {
    //   decoded.hardwareStatus["Battery-PG"] = 1;
    // }

  }
  return decoded;
}