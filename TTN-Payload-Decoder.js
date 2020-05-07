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
    
    if (bytes.length > 1) {
      decoded.temperature = intToDouble(bytes[0] + bytes[1] * 256, 2);
      decoded.humidity = intToDouble(bytes[2] + bytes[3] * 256, 2);
      decoded.pressure = intToDouble(bytes[4] + bytes[5] * 256, 1);
      decoded.pm25 = intToDouble(bytes[6] + bytes[7] * 256, 1);
      decoded.pm10 = intToDouble(bytes[8] + bytes[9] * 256, 1);
      decoded.hardwareStatus = {
        "ParticleSensor": {
          "wakeUpSuccessful": true,
          "sleepSuccessful": true,
          "readSuccessful": true
        },
        "TphSensor": {
          "wakeUpSuccessful": true,
          "readSuccessful": true
        },
        "BatteryIndicator": {
          "chargeStatus": true,
          "chargeComplete": true,
          "lowBattery": true,
          "timerTemperatureFault": true
        }
      };
      if (bytes.length > 10) {
        var binary_coded_status = bytes[10] + (bytes[11] * 256);

        /**
         * Particle sensor status codes
         */
        // Check particle sensor wake-up code
        if (!(1 & binary_coded_status)) {
          decoded.hardwareStatus.ParticleSensor.wakeUpSuccessful = false;
        }
    
        // Check particle sensor read code
        if (!((1 << 1) & binary_coded_status)) {
          decoded.hardwareStatus.ParticleSensor.readSuccessful = false;
        }
    
        // Check particle sensor sleep code
        if (!((1 << 2) & binary_coded_status)) {
          decoded.hardwareStatus.ParticleSensor.sleepSuccessful = false;
        }
    
        /**
         * TPH sensor status codes
         */
        // Check tph sensor wake-up code
        if (!((1 << 5) & binary_coded_status)) {
          decoded.hardwareStatus.TphSensor.wakeUpSuccessful = false;
        }
    
        // Check tph sensor read code
        if (!((1 << 6) & binary_coded_status)) {
          decoded.hardwareStatus.TphSensor.readSuccessful = false;
        }
    
        /**
         * Battery indicator status codes
         */
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
  
    } else {
      decoded.version = intToDouble(bytes[0], 1);
    }
  }
  return decoded;
}