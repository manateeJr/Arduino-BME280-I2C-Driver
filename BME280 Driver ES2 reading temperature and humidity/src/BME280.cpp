#include <BME280.h>

BME280_S32_t t_fine; 


// Helper variable to extract data from 'dig' registers 
uint8_t buffer;

// Variables for compensation formula for temperature
unsigned short dig_T1;
signed short dig_T2;
signed short dig_T3;

// Variables for compensation formula for humidity
uint8_t dig_H1;
int16_t dig_H2;
uint8_t dig_H3;
int16_t dig_H4;
int16_t dig_H5;
int8_t dig_H6;


// Returns the ID of the sensor
uint8_t BME280_GetID() //get ID of device
{
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(BME280_ID);
    Wire.endTransmission();

    Wire.requestFrom(BME280_ADDR, 1);  //read 1 byte from buffer
    uint8_t id = Wire.read();
    
    return id;
}

// Resets BME280 to factory settings
void BME280_reset()
{
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(BME280_RESET);
    Wire.write(BME280_RESET_COMMAND);
    Wire.endTransmission();
}

// Read the current oversampling rate
uint8_t BME280_ctrl_hum_read() //reading oversampling rate
{
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(BME280_CTRLHUM);
    Wire.endTransmission();

    Wire.requestFrom(BME280_ADDR, 1);
    uint8_t ctrlHumidity = Wire.read();

    return ctrlHumidity;
}

// Configure current oversampling rate. Takes 'SKIPPED', 'X1', 'X2', 'X4', 'X8' or 'X16' as arguement
void BME280_ctrl_hum_write(uint8_t bitpattern) 
{
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(BME280_CTRLHUM);
    Wire.write(bitpattern);
    Wire.endTransmission();
}

// Returns the current oversampling rate of temperature (b7, b6, b5), oversampling rate of pressure (b4, b3, b2) and settings mode (b1, b0)
uint8_t BME280_ctrl_meas_read()
{
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(BME280_CTRLMEAS);
    Wire.endTransmission();

    Wire.requestFrom(BME280_ADDR, 1);
    uint8_t ctrlTemperature = Wire.read();

    return ctrlTemperature;
}

// Configures the oversampling rate of temperature (b7, b6, b5), oversampling rate of pressure (b4, b3, b2) and settings mode (b1, b0)
void BME280_ctrl_meas_write(uint8_t bittpattern) 
{
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(BME280_CTRLMEAS);
    Wire.write(bittpattern);
    Wire.endTransmission();
} 

// Reads the temperature from registers 0xFA, 0xFB and 0xFC
float BME280_ReadTemperature() 
{
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(BME280_TEMP_MSB);
    Wire.endTransmission();
    
    //BME280_S32_t temperature = 0;
    Wire.requestFrom(BME280_ADDR, 3);
    // Read MSB, LSB and XLSB
    BME280_S32_t temperature_msb = Wire.read(); // msb
    BME280_S32_t temperature_lsb = Wire.read(); // lsb
    BME280_S32_t temperature_xlsb = Wire.read(); // xlsb
    
    BME280_S32_t temperature;
    temperature = (((temperature_msb << 12) | temperature_lsb << 4)) | (temperature_xlsb);
    //call temp conversion function here
    // ReadTemperatureCoefficients();
    temperature = BME280_compensate_T_int32(temperature);
    float deg = (float)temperature;
    
    return deg / 100;
}

void ReadTemperatureCoefficients() {    
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(DIG_T1);
    Wire.endTransmission();

    Wire.requestFrom(BME280_ADDR, 6);
    buffer = Wire.read();
    dig_T1 = Wire.read();
    dig_T1 <<= 8;
    dig_T1 |= buffer;
    
    buffer = Wire.read();
    dig_T2 = Wire.read();
    dig_T2 <<= 8;
    dig_T2 |= buffer;

    buffer = Wire.read();
    dig_T3 = Wire.read();
    dig_T3 <<= 8;
    dig_T3 |= buffer;
}

BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T) 
{ 
 BME280_S32_t var1, var2, T; 
 var1  = ((((adc_T>>3) - ((BME280_S32_t)dig_T1<<1))) * ((BME280_S32_t)dig_T2)) >> 11; 
 var2  = (((((adc_T>>4) - ((BME280_S32_t)dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)dig_T1))) 
>> 12) *  
  ((BME280_S32_t)dig_T3)) >> 14; 
 t_fine = var1 + var2; 
 T  = (t_fine * 5 + 128) >> 8; 
 return T; 
} 




// 16 bit humidity value is being read properly
double BME280_ReadHumidity()
{
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(BME280_HUM_MSB);
    Wire.endTransmission();
    
    //BME280_S32_t humidity = 0;
    Wire.requestFrom(BME280_ADDR, 2);
    // Read MSB
    BME280_S32_t humidity_msb = Wire.read();
    // Read LSB
    BME280_S32_t humidity_lsb = Wire.read();
    BME280_S32_t humidity = (humidity_msb << 8) | humidity_lsb;

    double humidityFinal = bme280_compensate_H_double(humidity);

    return humidityFinal;
}

//reading dig registers not working correctly yet
void ReadHumidityCoefficients() {
    // dig_H1
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(DIG_H1);
    Wire.endTransmission();

    Wire.requestFrom(BME280_ADDR, 1);
    
    dig_H1 = Wire.read();

    // dig_H2
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(DIG_H2);
    Wire.endTransmission();

    Wire.requestFrom(BME280_ADDR, 2);

    buffer = Wire.read();
    dig_H2 = Wire.read();

    dig_H2 = (dig_H2 << 8) | buffer;
    
    // dig_H3
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(DIG_H3);
    Wire.endTransmission();

    Wire.requestFrom(BME280_ADDR, 1);

    dig_H3 = Wire.read();

    // dig_H4
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(DIG_H4);
    Wire.endTransmission();

    Wire.requestFrom(BME280_ADDR, 2);

    dig_H4 = Wire.read();
    dig_H4 <<= 4;   

    buffer = Wire.read();
    buffer &= 0b1111;
        
    dig_H4 |= buffer;

    // dig_H5
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(DIG_H5);
    Wire.endTransmission();

    Wire.requestFrom(BME280_ADDR, 2);

    buffer = Wire.read();
    buffer >>= 4;

    dig_H5 = Wire.read();
    dig_H5 <<= 4;
    dig_H5 |= buffer;

    // dig_H6
    Wire.beginTransmission(BME280_ADDR);
    Wire.write(DIG_H6);
    Wire.endTransmission();

    Wire.requestFrom(BME280_ADDR, 1);

    dig_H6 = Wire.read();
/*  DEBUGGING
    Serial.println(dig_H1);
    Serial.println(dig_H2);
    Serial.println(dig_H3);
    Serial.println(dig_H4);
    Serial.println(dig_H5);
    Serial.println(dig_H6);
*/
}


double bme280_compensate_H_double(BME280_S32_t adc_H) {
    double var_H; 
 
 var_H = (((double)t_fine) - 76800.0); 
 var_H = (adc_H - (((double)dig_H4) * 64.0 + ((double)dig_H5) / 16384.0 * 
var_H)) * (((double)dig_H2) / 65536.0 * (1.0 + ((double)dig_H6) / 
67108864.0 * var_H *  
(1.0 + ((double)dig_H3) / 67108864.0 * var_H))); 
 var_H = var_H * (1.0 - ((double)dig_H1) * var_H / 524288.0); 
  
 if (var_H > 100.0) 
  var_H = 100.0; 
 else if (var_H < 0.0) 
  var_H = 0.0; 
  
return var_H; 

}
