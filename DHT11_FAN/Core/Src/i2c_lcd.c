/*
 * i2c_lcd.c
 *
 *  Created on: Feb 3, 2026
 *  Author: appletea
 *
 * [구조 설명]
 * STM32 ──I2C──> PCF8574 ──(4bit)──> HD44780 LCD
 *
 * PCF8574의 8bit 포트 구성 예:
 *  D7 D6 D5 D4 BL EN RW RS
 *  ↑  ↑  ↑  ↑  ↑  ↑  ↑  ↑
 *  LCD 데이터   제어 비트
 */

#include "i2c_lcd.h"
#include "i2c.h"

/*
 * lcd_command()
 *  - LCD에 "명령(command)" 전송
 *  - RS = 0
 *  - 8bit 명령을 4bit + 4bit로 나누어 전송 (LCD 4bit 모드)
 */
void lcd_command(uint8_t command)
{
    uint8_t high_nibble, low_nibble; // 상위 4bit / 하위 4bit
    uint8_t i2c_buffer[4];           // I2C로 한 번에 보낼 데이터 (EN 토글 포함)

    // 상위 4bit 추출 (D7~D4)
    high_nibble = command & 0xF0;

    // 하위 4bit를 상위로 이동 (D3~D0 → D7~D4)
    low_nibble = (command << 4) & 0xF0;

    /*
     * PCF8574 비트 매핑
     * bit 0 (0x01) = RS (0: Command, 1: Data)
     * bit 1 (0x02) = RW (0: Write)
     * bit 2 (0x04) = EN (Enable)
     * bit 3 (0x08) = Backlight
     */

    // 상위 4bit + EN = 1 → LCD가 데이터 래치
    i2c_buffer[0] = high_nibble | 0x04 | 0x08;

    // 상위 4bit + EN = 0 → EN falling edge (실제 래치 타이밍)
    i2c_buffer[1] = high_nibble | 0x00 | 0x08;

    // 하위 4bit + EN = 1
    i2c_buffer[2] = low_nibble  | 0x04 | 0x08;

    // 하위 4bit + EN = 0
    i2c_buffer[3] = low_nibble  | 0x00 | 0x08;

    /*
     * HAL_I2C_Master_Transmit
     * - START
     * - Slave Address + Write
     * - i2c_buffer[0~3] 전송
     * - STOP
     *
     * while()로 감싼 이유:
     *  → 통신 실패 시 재시도 (Blocking 방식)
     */
    while (HAL_I2C_Master_Transmit(&hi2c1,
                                   I2C_LCD_ADDRESS,
                                   i2c_buffer,
                                   4,
                                   100) != HAL_OK);
}

/*
 * lcd_data()
 *  - LCD에 "문자 데이터" 전송
 *  - RS = 1
 *  - 동작 방식은 lcd_command와 동일
 */
void lcd_data(uint8_t data)
{
    uint8_t high_nibble, low_nibble;
    uint8_t i2c_buffer[4];

    high_nibble = data & 0xF0;
    low_nibble  = (data << 4) & 0xF0;

    // RS = 1 (Data), EN = 1
    i2c_buffer[0] = high_nibble | 0x05 | 0x08;

    // RS = 1, EN = 0
    i2c_buffer[1] = high_nibble | 0x01 | 0x08;

    // 하위 4bit 전송
    i2c_buffer[2] = low_nibble  | 0x05 | 0x08;
    i2c_buffer[3] = low_nibble  | 0x01 | 0x08;

    while (HAL_I2C_Master_Transmit(&hi2c1,
                                   I2C_LCD_ADDRESS,
                                   i2c_buffer,
                                   4,
                                   100) != HAL_OK);
}

/*
 * i2c_lcd_init()
 *  - LCD 초기화 시퀀스
 *  - HD44780 데이터시트 권장 순서
 */
void i2c_lcd_init(void)
{
    HAL_Delay(50);      // 전원 안정화 대기

    lcd_command(0x33);  // 8bit 모드 초기화 시도
    HAL_Delay(5);

    lcd_command(0x32);  // 4bit 모드 진입
    HAL_Delay(5);

    lcd_command(0x28);  // Function Set: 4bit, 2Line, 5x8 Font
    HAL_Delay(5);

    lcd_command(DISPLAY_ON); // Display ON, Cursor OFF
    HAL_Delay(5);

    lcd_command(0x06);  // Entry Mode: 커서 우측 이동
    HAL_Delay(5);

    lcd_command(CLEAR_DISPLAY); // 화면 클리어
    HAL_Delay(2);       // Clear 명령은 실행 시간 김
}

/*
 * lcd_string()
 *  - 문자열 출력
 *  - NULL 문자('\0') 나올 때까지 반복
 */
void lcd_string(char *str)
{
    while (*str)
        lcd_data(*str++);
}

/*
 * move_cursor()
 *  - 커서 위치 이동
 *  - row: 0 or 1
 *  - col: 0 ~ 15
 *
 * DDRAM 주소 규칙:
 *  row 0 → 0x00
 *  row 1 → 0x40
 */
void move_cursor(uint8_t row, uint8_t col)
{
    lcd_command(0x80 | (row << 6) | col);
}
