#include "my_SSD1306.h"
#include "string.h"

static SSD1306_t SSD1306;

static uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];

// Hàm khởi tạo SSD1306
void SSD1306_Init(void) {
    // Reset màn hình
	SSD1306_Reset();

	HAL_Delay(100);

    // Cấu hình các lệnh khởi tạo của SSD1306
    SSD1306_WriteCommand(0xAE); // Tắt màn hình
    SSD1306_WriteCommand(0xD5); // Set display clock divide ratio/oscillator frequency
    SSD1306_WriteCommand(0x80); // Suggested ratio 0x80
    SSD1306_WriteCommand(0xA8); // Set multiplex ratio(1 to 64)
    SSD1306_WriteCommand(0x3F); // 1/64 duty
    SSD1306_WriteCommand(0xD3); // Set display offset
    SSD1306_WriteCommand(0x00); // No offset
    SSD1306_WriteCommand(0x40 | 0x0); // Set start line address
    SSD1306_WriteCommand(0x8D); // Enable charge pump regulator
    SSD1306_WriteCommand(0x14);
    SSD1306_WriteCommand(0x20); // Set memory addressing mode
    SSD1306_WriteCommand(0x00); // Horizontal addressing mode
    SSD1306_WriteCommand(0xA1); // Set segment re-map 0 to 127
    SSD1306_WriteCommand(0xC8); // Set COM output scan direction, remapped mode
    SSD1306_WriteCommand(0xDA); // Set COM pins hardware configuration
    SSD1306_WriteCommand(0x12);
    SSD1306_WriteCommand(0x81); // Set contrast control
    SSD1306_WriteCommand(0x7F);
    SSD1306_WriteCommand(0xD9); // Set pre-charge period
    SSD1306_WriteCommand(0xF1);
    SSD1306_WriteCommand(0xDB); // Set VCOMH deselect level
    SSD1306_WriteCommand(0x40);
    SSD1306_WriteCommand(0xA4); // Output follows RAM content
    SSD1306_WriteCommand(0xA6); // Set normal display
    SSD1306_WriteCommand(0xAF); // Bật màn hình

    SSD1306_Fill(Black);
    SSD1306_UpdateScreen();
}

// Vẽ một điểm ảnh
void SSD1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        // Don't write outside the buffer
        return;
    }

    // Draw in the right color
    if(color == White) {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

void SSD1306_Reset(void) {
    // CS = High (not selected)
    HAL_GPIO_WritePin(SSD1306_CS_GPIO, SSD1306_CS_PIN, GPIO_PIN_SET);

    // Reset the OLED
    HAL_GPIO_WritePin(SSD1306_RESET_GPIO, SSD1306_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(SSD1306_RESET_GPIO, SSD1306_RESET_PIN, GPIO_PIN_SET);
    HAL_Delay(10);
}

// Send a byte to the command register
void SSD1306_WriteCommand(uint8_t byte) {
    HAL_GPIO_WritePin(SSD1306_CS_GPIO, SSD1306_CS_PIN, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SSD1306_DC_GPIO, SSD1306_DC_PIN, GPIO_PIN_RESET); // command
    HAL_SPI_Transmit(&SSD1306_SPI_PORT, (uint8_t *) &byte, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SSD1306_CS_GPIO, SSD1306_CS_PIN, GPIO_PIN_SET); // un-select OLED
}

// Send data
void SSD1306_WriteData(uint8_t* buffer, size_t buff_size) {
    HAL_GPIO_WritePin(SSD1306_CS_GPIO, SSD1306_CS_PIN, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SSD1306_DC_GPIO, SSD1306_DC_PIN, GPIO_PIN_SET); // data
    HAL_SPI_Transmit(&SSD1306_SPI_PORT, buffer, buff_size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SSD1306_CS_GPIO, SSD1306_CS_PIN, GPIO_PIN_SET); // un-select OLED
}

void SSD1306_UpdateScreen(void) {
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
    for(uint8_t i = 0; i < SSD1306_HEIGHT/8; i++) {
        SSD1306_WriteCommand(0xB0 + i); // Set the current RAM page address.
        SSD1306_WriteCommand(0x00 + SSD1306_X_OFFSET_LOWER);
        SSD1306_WriteCommand(0x10 + SSD1306_X_OFFSET_UPPER);
        SSD1306_WriteData(&SSD1306_Buffer[SSD1306_WIDTH*i],SSD1306_WIDTH);
    }
}

char SSD1306_WriteChar(char ch, SSD1306_Font_t Font, SSD1306_COLOR color) {
    uint32_t i, b, j;

    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (SSD1306.CurrentX + Font.width) ||
        SSD1306_HEIGHT < (SSD1306.CurrentY + Font.height))
    {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for(i = 0; i < Font.height; i++) {
        b = Font.data[(ch - 32) * Font.height + i];
        for(j = 0; j < Font.width; j++) {
            if((b << j) & 0x8000)  {
                SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
            } else {
                SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
            }
        }
    }

    // The current space is now taken
    SSD1306.CurrentX += Font.char_width ? Font.char_width[ch - 32] : Font.width;

    // Return written char for validation
    return ch;
}

char SSD1306_WriteString(char* str, SSD1306_Font_t Font, SSD1306_COLOR color) {
    while (*str) {
        if (SSD1306_WriteChar(*str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }
        str++;
    }

    // Everything ok
    return *str;
}

void SSD1306_SetCursor(uint8_t x, uint8_t y) {
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

void SSD1306_Fill(SSD1306_COLOR color) {
    memset(SSD1306_Buffer, (color == Black) ? 0x00 : 0xFF, sizeof(SSD1306_Buffer));
}

void SSD1306_SetDisplayOn(const uint8_t on) {
    uint8_t value;
    if (on) {
        value = 0xAF;   // Display on
        SSD1306.DisplayOn = 1;
    } else {
        value = 0xAE;   // Display off
        SSD1306.DisplayOn = 0;
    }
    SSD1306_WriteCommand(value);
}
