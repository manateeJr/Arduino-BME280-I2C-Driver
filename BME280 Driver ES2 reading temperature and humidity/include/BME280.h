#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <Wire.h>

#define BME280_ADDR 0x76
#define BME280_ID 0XD0

#define BME280_RESET 0xE0
#define BME280_RESET_COMMAND 0xB6

#define BME280_CTRLHUM 0xF2

#define BME280_CTRLMEAS 0xF4

#define BME280_TEMP_MSB 0xFA
#define BME280_TEMP_LSB 0xFB
#define BME280_TEMP_XLSB 0xFC

#define BME280_HUM_MSB 0xFD
#define BME280_HUM_LSB 0xFE


#define BME280_READINGS_RESET 0x80

#define DIG_T1 0x88

#define DIG_H1 0xA1
#define DIG_H2 0xE1
#define DIG_H3 0xE3
#define DIG_H4 0xE4
#define DIG_H5 0xE5
#define DIG_H6 0xE7

typedef int32_t BME280_S32_t;
typedef uint32_t BME280_U32_t;


enum oversamplingRateHumidity {
    SKIPPED_H, X1_H, X2_H, X4_H, X8_H, X16_H
};

enum oversamplingRateTemperature {
    SKIPPED_T = (0b000 << 5),
    X1_T = (0b001 << 5),
    X2_T = (0b010 << 5),    
    X4_T = (0b011 << 5),
    X8_T = (0b100 << 5),
    X16_T = (0b101 << 5)
};

enum registerSettingsMode {
    SLEEP, FORCED, FORCEDMODE, NORMAL
};


uint8_t BME280_GetID(); 
void BME280_reset(); 
uint8_t BME280_ctrl_hum_read(); 
void BME280_ctrl_hum_write(uint8_t); 
uint8_t BME280_ctrl_meas_read(); 
void BME280_ctrl_meas_write(uint8_t); 

float BME280_ReadTemperature(); 
double BME280_ReadHumidity(); 

BME280_S32_t BME280_compensate_T_int32(BME280_S32_t);
double bme280_compensate_H_double(BME280_S32_t);
BME280_U32_t bme280_compensate_H_int32(BME280_S32_t);

void ReadTemperatureCoefficients();
void ReadHumidityCoefficients();


