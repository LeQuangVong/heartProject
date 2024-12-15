#ifndef INC_MY_MAX30100_H_
#define INC_MY_MAX30100_H_

#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "math.h"

#define MAX30100_ADDR  				(0x57 << 1)
#define MAX30100_IT_STATUS			0x00
#define MAX30100_IT_ENABLE			0x01
#define MAX30100_FIFO_WRITE_PTR		0x02
#define MAX30100_OVER_FLOW_CONTROL	0x03
#define MAX30100_FIFO_READ_PTR		0x04
#define MAX30100_FIFO_DATA			0x05
#define MAX30100_MODE_CONFIG		0x06
#define MAX30100_SPO2_CONFIG		0x07
#define MAX30100_LED_CONFIG			0x09
#define MAX30100_TEMP_INT			0x16
#define MAX30100_TEMP_FRAC			0x17

typedef struct
{
	I2C_HandleTypeDef *hi2c;
	uint16_t ir;
	uint16_t red;
	float temperature;
}MAX30100_Handle_t;

void MAX30100_Init(MAX30100_Handle_t *pMAX30100, I2C_HandleTypeDef *hi2c);
void MAX30100_Read_FIFO(MAX30100_Handle_t *pMAX30100);
void MAX30100_ReadTemperature(MAX30100_Handle_t *pMAX30100);
void MAX30100_Reset(MAX30100_Handle_t *pMAX30100);

float low_pass_filter(float input, float previous_output, float alpha);
int detect_peak(float *data, int lenght);
float calculate_spo2(float ac_red, float dc_red, float ac_ir, float dc_ir);

#endif /* INC_MY_MAX30100_H_ */
