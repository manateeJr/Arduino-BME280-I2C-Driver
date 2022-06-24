#include <BME280.h>

oversamplingRateHumidity oversamplingHumidity = X2_H;
oversamplingRateTemperature oversamplingTemp = X2_T;
//  NOTE: to change the mode of the settings call the ctrl_meas_write function with any of the modes as an arguement
registerSettingsMode settingsMode = NORMAL; 

// Test setup, rework before delivering
void setup() {
  Wire.begin();
  Serial.begin(9600);

  BME280_reset();

  uint8_t BMEID = BME280_GetID();

  Serial.print("BME id number: ");
  Serial.println(BMEID);
  
  // Set humidity oversampling to X1
  BME280_ctrl_hum_write(X16_H);

  // Set temperature oversampling to X1 and set mode to normal
  BME280_ctrl_meas_write(X16_T | NORMAL);

  Serial.print("Humidity sampling rate is: ");
  Serial.println(BME280_ctrl_hum_read());

  Serial.print("Temperature sampling rate is: ");
  Serial.println(BME280_ctrl_meas_read());

  ReadTemperatureCoefficients();
  ReadHumidityCoefficients();
}

void loop() {
  
  float temperature = BME280_ReadTemperature(); //WORKS!!
  Serial.print("Current air temperature: ");
  Serial.print(temperature);
  Serial.println("C");
  



  double humidity = BME280_ReadHumidity();

  Serial.print("Current humidity: ");
  Serial.println(humidity);  
  Serial.print("\n\n");
  delay(2500);
}