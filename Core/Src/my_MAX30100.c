#include "my_MAX30100.h"

void MAX30100_Init(MAX30100_Handle_t *pMAX30100, I2C_HandleTypeDef *hi2c)
{
	pMAX30100->hi2c = hi2c;

	MAX30100_Reset(pMAX30100);

	uint8_t data;

	data = 0x03;
	HAL_I2C_Mem_Write(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_MODE_CONFIG, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

	data = 0x27;
	HAL_I2C_Mem_Write(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_SPO2_CONFIG, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

	data = 0x24;
	HAL_I2C_Mem_Write(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_LED_CONFIG, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

	//uint8_t int_enable = (1 << 5) | (1 << 4);
	uint8_t int_enable = 0xF0;
	HAL_I2C_Mem_Write(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_IT_ENABLE, I2C_MEMADD_SIZE_8BIT, &int_enable, 1, HAL_MAX_DELAY);
}

void MAX30100_Read_FIFO(MAX30100_Handle_t *pMAX30100)
{
	uint8_t data[4];

	HAL_I2C_Mem_Read(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_FIFO_DATA, I2C_MEMADD_SIZE_8BIT, data, 4, HAL_MAX_DELAY);

	pMAX30100->ir = (data[0] << 8) | data[1];
	pMAX30100->red = (data[2] << 8) | data[3];
}

float low_pass_filter(float input, float previous_output, float alpha)
{
	return alpha * input + (1 - alpha) * previous_output;
}

int detect_peak(float *data, int lenght)
{
	int peak_count = 0;
	for(int i = 1; i < lenght - 1; i++)
	{
		if(data[i] > data[i-1] && data[i] > data[i+1])
		{
			peak_count++;
		}
	}
	return peak_count;
}

float calculate_spo2(float ac_red, float dc_red, float ac_ir, float dc_ir)
{
	float ratio = (ac_red/dc_red) / (ac_ir/dc_ir);
	return 110 - 25*ratio;
}

void MAX30100_Reset(MAX30100_Handle_t *pMAX30100)
{
    uint8_t reset_command = 0x40; // Giá trị reset trong thanh ghi MODE_CONFIG
    HAL_I2C_Mem_Write(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_MODE_CONFIG, I2C_MEMADD_SIZE_8BIT, &reset_command, 1, HAL_MAX_DELAY);
}
