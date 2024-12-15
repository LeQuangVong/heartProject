#### Lưu đồ 
![1](/anh/1.png)

#### Khởi tạo MAX30100
- Cài đặt thanh ghi Mode Configuration:
    - Bật chế độ SpO2 (giám sát nhịp tim và nồng độ oxi trong máu):

    ```
    data = 0x03;
	HAL_I2C_Mem_Write(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_MODE_CONFIG, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    ```

    - Khởi động lại thiết bị:

    ```
    reset_command = 0x40;
    HAL_I2C_Mem_Write(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_MODE_CONFIG, I2C_MEMADD_SIZE_8BIT, &reset_command, 1, HAL_MAX_DELAY);
    ```

![2](/anh/2.png)
![3](/anh/3.png)

- Cài đặt thanh ghi SpO2 Configuration:
    ```
    data = 0x27;
	HAL_I2C_Mem_Write(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_SPO2_CONFIG, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    ```
    - SPO2_ADC_RGE = 01 (RANGE = 4096).
    - SPO2_SR = 100 (Sample Rate = 100Hz).
    - LED_PW = 11 (Led Pulse Width = 1600us).

![4](/anh/4.png)
![5](/anh/5.png)
![6](/anh/6.png)

- Cài đặt thanh ghi Led Configuration

    ```
    data = 0x24;
    HAL_I2C_Mem_Write(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_LED_CONFIG, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    ```

    - Led đỏ: Dòng 6mA
    - Led hồng ngoại (IR): Dòng 12mA

![7](/anh/7.png)
![8](/anh/8.png)

- Cài đặt thanh ghi Interrupt Enable:
    ```
    int_enable = 0xF0;
	HAL_I2C_Mem_Write(pMAX30100->hi2c, MAX30100_ADDR, MAX30100_IT_ENABLE, I2C_MEMADD_SIZE_8BIT, &int_enable, 1, HAL_MAX_DELAY);
    ```
    - Bit 7: kích hoạt ngắt khi FIFO đầy.
    - Bit 6: Kích hoạt ngắt đo xong nhiệt độ.
    - Bit 5: Kích hoạt ngắt phát hiện SpO2.
    - Bit 4: Kích hoạt ngắt phát hiện nhịp tim.

![9](/anh/9.png)
#### Khởi tạo SSD1306
- Reset màn hình:
    ```
    void SSD1306_Reset(void) {
        //Đặt CS ở mức cao để màn không được chọn để giao tiếp.
        HAL_GPIO_WritePin(SSD1306_CS_GPIO, SSD1306_CS_PIN, GPIO_PIN_SET);

        //Chân RESET hạ xuống thấp để khởi tạo lại màn
        HAL_GPIO_WritePin(SSD1306_RESET_GPIO, SSD1306_RESET_PIN, GPIO_PIN_RESET);
        HAL_Delay(10);
        //Chân RESET đưa lên cao kết thúc quá trình reset
        HAL_GPIO_WritePin(SSD1306_RESET_GPIO, SSD1306_RESET_PIN, GPIO_PIN_SET);
        HAL_Delay(10);
    }
    ```
- Set Memory Addressing Mode (20h):
    - Page addressing mode (A[1:0]=10xb): Bộ nhớ hiển thị được chia thành các trang.
    ![11](/anh/11.png)
    ![12](/anh/12.png)
    - Horizontal addressing mode (A[1:0]=00b): Truy cập tuần tự theo hàng ngang.
    ![13](/anh/13.png)
    - Vertical addressing mode: (A[1:0]=01b): Truy cập tuần tự theo hàng dọc.
    ![14](/anh/14.png)
    ```
    SSD1306_WriteCommand(0x20); // Set memory addressing mode
    SSD1306_WriteCommand(0x00); // Horizontal addressing mode
    ```
    - Tương thích với cách tổ chức của GDDRAM, tối ưu hiệu suất ghi dữ liệu.
- Set Display Start Line (40h~7Fh):
    Chỉ định dòng trong GDDRAM làm dòng đầu tiên hiển thị trên màn hình.
    ```
    SSD1306_WriteCommand(0x40 | 0x0); // Set start line address
    ```
    Dòng đầu tiên của màn hình hiển thị sẽ là dòng 0 của GDDRAM.
- Set Contrast Control for BANK0 (81h):
    Điều chỉnh mức điện áp hoặc dòng điện điều khiển qua đó thay đổi độ sáng tương đối.
    ```
    SSD1306_WriteCommand(0x81); // Set contrast control
    SSD1306_WriteCommand(0x7F);
    ```
    Điều chỉnh độ tương phản ở mức trung bình 127/255.
- Set Segment Re-map (A0h/A1h):
    - A0h: Ánh xạ cột GDDRAM theo hướng bình thường (column 0 -> segment 0).
    - A1h: Ánh xạ cột GDDRAM theo hướng đảo ngược (column 0 -> segment 127).
    ```
    SSD1306_WriteCommand(0xA1); // Set segment re-map 0 to 127
    ```
- Entire Display ON (A4h/A5h):
    - A4h: Màn hình hiển thị theo dữ liệu trong GDDRAM.
    - A5h: Tất cả các pixel trên màn hình sẽ luôn sáng không phụ thuộc vào dữ liệu trong GDDRAM.
    ```
    SSD1306_WriteCommand(0xA4); // Output follows RAM content
    ```
-  Set Normal/Inverse Display (A6h/A7h):
    - A6h: Hiển thị bình thường.
    - A7h: Hiển thị ngược.
    ```
    SSD1306_WriteCommand(0xA6); // Set normal display
    ```
- Set Multiplex Ratio (A8h):
    - Hiển thị các dòng theo chu kì thay vì hiển thị cùng 1 lúc, giúp tiết kiệm tài nguyên và nguồn điện.
    ```
    SSD1306_WriteCommand(0xA8); // Set multiplex ratio(1 to 64)
    SSD1306_WriteCommand(0x3F); // 1/64 duty
    ```
- Set Display Offset (D3h):
    - Điều chỉnh vị trí hiển thị trong khoảng 0x00 -> 0x3F (0 - 63).
    ```
    SSD1306_WriteCommand(0xD3); // Set display offset
    SSD1306_WriteCommand(0x00); // No offset
    ```
- Set Display Clock Divide Ratio/ Oscillator Frequency (D5h):
    Thiết lập tần số dao động và tỷ lệ chia. 
    - Clock Divide Ratio: 4 bit thấp, quyết định tốc độ làm mới của màn hình, tỷ lệ càng cao thì tốc độ quét càng chậm.
    - Oscillator Frequency: 4 bit cao.
    ```
    SSD1306_WriteCommand(0xD5); // Set display clock divide ratio/oscillator frequency
    SSD1306_WriteCommand(0x80); // Suggested ratio 0x80
    ```
    - Oscillator Frequency: 0x08.
    - Clock Divide Ratio: 0x00: không phân chia, tỉ lệ chia 1:1.
- Set COM Pins Hardware Configuration (DAh):
    Thiết lập chân COM phù hợp với độ phân giải của màn hình (128x64).
    ```
    SSD1306_WriteCommand(0xDA); // Set COM pins hardware configuration
    SSD1306_WriteCommand(0x12);
    ```
- Set VCOMH  Deselect Level (DBh):
    - Xác định mức điện áp mà các dòng tín hiệu sẽ được điều khiển.
    ```
    SSD1306_WriteCommand(0xDB); // Set VCOMH deselect level
    SSD1306_WriteCommand(0x40);
    ```
- Set Pre-charge Period (D9h):
    - Cấu hình thời gian sạc trước của các tụ điện vì màn hình sử dụng các tụ điện để lưu trữ điện áp cho mỗi pixel, ảnh hưởng đến độ tương phản và làm mới màn hình.
    ```
    SSD1306_WriteCommand(0xD9); // Set pre-charge period
    SSD1306_WriteCommand(0xF1);
    ```
    - 0xF1: giai đoạn 1 là 0xF (15 cycles) và giai đoạn 2 là 0x1 (1 cycle).
- Set Display ON/OFF (AEh/AFh):
    - AEh: ngừng hiển thị nội dung trên màn hình nhưng không tắt nguồn, màn hình vẫn giữ lại dữ liệu trong RAM. Bật lại màn hình thì dữ liệu vẫn được hiển thị.
    - AFh: bật lại màn hình, làm cho màn hình hiển thị dữ liệu đã được lưu trong RAM.
    ```
    SSD1306_WriteCommand(0xAE); // Tắt màn hình
    SSD1306_WriteCommand(0xAF); // Bật màn hình
    ```
#### Hàm xử lí ngắt
- Chân INT là đầu ra của cảm biến, thông báo các sự kiện ngắt đến vi điều khiển
- Chân INT sẽ được kéo xuống thấp khi có ngắt xảy ra, sau khi xử lí xong và đọc thanh ghi Status thì INT trở lại mức cao.
- Đọc và kiểm tra các ngắt thông qua thanh ghi Interrupt status:
    - Nếu đọc thanh ghi xảy ra lỗi, sẽ đặt cờ báo lỗi va thoát khỏi ngắt.
    ```
    if (HAL_I2C_Mem_Read(&hi2c1, MAX30100_ADDR, MAX30100_IT_STATUS, I2C_MEMADD_SIZE_8BIT, &int_status, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        error_flag = 1;
        return;
    }
    ```
    - Kiểm tra ngắt FIFO full, nếu bit này được set thì FIFO đã đầy.
    ```
    if (int_status & (1 << 7)) // FIFO Full
    {
        fifo_full_flag = 1;
    }
    ```
    - Kiểm tra ngắt Data Ready, nếu bit này được set thì dữ liệu SpO2 và nhịp tim đã sẵn sàng đọc.
    ```
    if (int_status & (1 << 5)) // Data Ready
    {
        data_ready_flag = 1;
    }
    ```
#### Thanh ghi trạng thái
![10](/anh/10.png)
#### Xử lí dữ liệu
- Xử lí tính hiệu để tính nhịp tim:
    - Lọc tín hiệu IR thông qua bộ lọc tần số thấp:
        - chỉ giữ lại các thành phần tần số thấp trong tín hiệu và loại bỏ nhiễu ở tần số cao. Thành phần tần số thấp biểu thị nhịp tim (giao động tuần hoàn), tần số cao do nhiễu từ môi trường

        - Công thức:
        y[n]=α⋅x[n]+(1−α)⋅y[n−1]
        
            - y[n]: Tín hiệu đã lọc tại thời điểm hiện tại.
            - x[n]: Tín hiệu đầu vào tại thời điểm hiện tại.
            - y[n−1]: Tín hiệu đã lọc tại thời điểm trước đó.
            - α: Hệ số lọc, điều chỉnh mức độ làm mượt tín hiệu:
                - α gần 1: Phản ứng nhanh với thay đổi, ít làm mượt.
                - α gần 0: Làm mượt mạnh, nhưng có độ trễ lớn.
        ```
        ir_filtered = low_pass_filter(max30100.ir, ir_filtered, 0.9);
        ```
        ```
        float low_pass_filter(float input, float prev_output, float alpha) {
            return (alpha * input) + ((1 - alpha) * prev_output);
        }
        ```
    - Phát hiện đỉnh:
        - là điểm cao nhất trong một chu kì giao động
        - mục đích là xác định số lượng đỉnh và khoảng cách giữa các đỉnh để tính nhịp tim.
        - điều kiện để gọi là một đỉnh là điểm đó phải cao hơn điểm trước nó và sau nó.

        ```
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
        ```
        length được tính bằng tần số lấy mẫu nhân với thời gian quan sát.
            -  tần số lấy mẫu ở đây bằng 100Hz (100 mẫu trên giây).
            - thời gian quan sát bằng 5s.
    - Chu kì giữa 2 đỉnh liên tiếp:
        ```
        peak_interval = 1.0f / sample_rate;
        ```
    - Tính nhịp tim:
        được tính bằng cách lấy 60 chia cho tổng thời gian (chu kì giữa 2 đỉnh nhân với số đỉnh).
        ```
        heart_rate = (60.0f / (peak_interval * peak_count));
        ```
- Xử lí tín hiệu để tính SpO2:
    - Lọc tín hiệu ánh sáng đỏ được đo từ cảm biến thông qua bộ lọc thấp.
    - Tách thành phần AC và DC:
        - DC là đại diện cho ánh sáng được hấp thụ bởi mô và máu tĩnh.
        - AC là đại diện cho sự thay đổi lượng máu trong mạch máu khi bơm máu.
    - Tính SpO2 bằng cách tính theo công thức:
        ```
        110 - 25*ratio
        ```
        ratio là tỉ lệ giữa AC và DC của tín hiệu đỏ và tỉ lệ của tín hiệu hồng ngoại. 
    ```
    float calculate_spo2(float ac_red, float dc_red, float ac_ir, float dc_ir)
    {
        float ratio = (ac_red/dc_red) / (ac_ir/dc_ir);
        return 110 - 25*ratio;
    }
    ```
#### Xử lí lỗi
- Khi cờ báo lỗi được set thì sẽ tiến hành thông báo và Khởi tạo lại cảm biến.
```
if (error_flag)
{
    SSD1306_SetCursor(0, 0);
    SSD1306_WriteString("Error detected", Font_7x10, White);
    SSD1306_UpdateScreen();
    HAL_Delay(1000);
    MAX30100_Init(&max30100, &hi2c1);
    error_flag = 0;
    SSD1306_SetCursor(0, 20);
    SSD1306_WriteString("Restarting...", Font_7x10, White);
    SSD1306_UpdateScreen();
    HAL_Delay(2000); // Khởi động lại cảm biến
}
```
- Khi nhịp tim nhỏ hơn 40 bpm hoặc lớn hơn 180 bpm, hay SpO2 nhỏ hơn 85% hoặc lớn hơn 100% thì sẽ thông báo cảnh báo.
#### Hiển thị SSD1306
- Vẽ 1 pixel:
    - Có 128 cột, mỗi cột chứa 8 bytes, mỗi byte chưa 8 pixel.
    Ex: pixel ở vị trí (x = 10, y = 5)
    - Chỉ số byte = x + (y/8)*SSD1306_WIDTH(128)
    => byte chứa pixel là byte thứ 10.
    - Chỉ số bit = y % 8
    => pixel sẽ được đặt tại bit thứ 5 trong byte.
    ```
    void SSD1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
        if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
            return;
        }

        if(color == White) {
            SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
        } else {
            SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
        }
    }
    ```
- Vẽ kí tự.
- Vẽ chuỗi kí tự.