#ifndef INC_MY_SSD1306_H_
#define INC_MY_SSD1306_H_

#include "stm32f1xx_hal.h"

extern SPI_HandleTypeDef hspi1;

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_BUFFER_SIZE		SSD1306_WIDTH * SSD1306_HEIGHT/8

#ifdef SSD1306_X_OFFSET
#define SSD1306_X_OFFSET_LOWER (SSD1306_X_OFFSET & 0x0F)
#define SSD1306_X_OFFSET_UPPER ((SSD1306_X_OFFSET >> 4) & 0x07)
#else
#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0
#endif

#define SSD1306_SPI	&hspi1
#define SSD1306_SPI_PORT        hspi1

#define SSD1306_DC_GPIO GPIOA
#define SSD1306_DC_PIN GPIO_PIN_4

#define SSD1306_RESET_GPIO GPIOA
#define SSD1306_RESET_PIN GPIO_PIN_6

#define SSD1306_CS_GPIO GPIOA
#define SSD1306_CS_PIN GPIO_PIN_3

#define SSD1306_COMMAND 0
#define SSD1306_DATA 1

typedef enum {
	SSD1306_OK = 0x00,
	SSD1306_ERR = 0x01
} SSD1306_Error_t;

typedef enum {
	Black = 0x00, // Black color, no pixel
	White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Initialized;
	uint8_t DisplayOn;
} SSD1306_t;

typedef struct {
	uint8_t x;
	uint8_t y;
} SSD1306_VT;

typedef struct {
	const uint8_t width;                /**< Font width in pixels */
	const uint8_t height;               /**< Font height in pixels */
	const uint16_t *const data;         /**< Pointer to font data array */
    const uint8_t *const char_width;    /**< Proportional character width in pixels (NULL for monospaced) */
} SSD1306_Font_t;


void SSD1306_Init(void);
void SSD1306_WriteCommand(uint8_t byte);
void SSD1306_WriteData(uint8_t* buffer, size_t buff_size);
void SSD1306_Reset(void);
void SSD1306_UpdateScreen(void);
void SSD1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char SSD1306_WriteChar(char ch, SSD1306_Font_t Font, SSD1306_COLOR color);
char SSD1306_WriteString(char* str, SSD1306_Font_t Font, SSD1306_COLOR color);
void SSD1306_SetCursor(uint8_t x, uint8_t y);
void SSD1306_SetDisplayOn(const uint8_t on);
void SSD1306_Fill(SSD1306_COLOR color);
#endif /* INC_MY_SSD1306_H_ */
