/*
 * i2c_lcd.h
 *
 *  Created on: Feb 3, 2026
 *      Author: appletea
 */

#ifndef INC_I2C_LCD_H_
#define INC_I2C_LCD_H_


#include "stm32f4xx_hal.h"

#define I2C_LCD_ADDRESS (0x27 << 1)	// A0, A1, A2가 High라서

//LCD command

#define DISPLAY_ON 		0x0C
#define DISPLAY_OFF 	0x08
#define CLEAR_DISPLAY 	0x01
#define RETURN_HOME 	0x02


void lcd_command(uint8_t command);
void lcd_data(uint8_t data);
void i2c_lcd_init(void);
void lcd_string(char *str);
void move_cursor(uint8_t row, uint8_t col);





#endif /* INC_I2C_LCD_H_ */
