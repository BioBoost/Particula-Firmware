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
  
      var START_OFF_ERRORCODES = 10;  // Of type var because eror with const
      decoded.errors = {};
      
      for (i = START_OFF_ERRORCODES; i < bytes.length; i++) {
        decoded.errors[i - START_OFF_ERRORCODES] = bytes[i];
      }
    }
    return decoded;
  }