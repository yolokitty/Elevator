/*
 * led.h
 *
 *  Created on: Feb 6, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32f4xx_hal.h"


// LED 제어를 위한 구조체
typedef struct
{

	GPIO_TypeDef		*port;
	uint16_t				number;
	GPIO_PinState		onState;
	GPIO_PinState		offState;

}LED_CONTROL;



void ledOn(uint8_t num);
void ledOff(uint8_t num);
void ledToggle(uint8_t num);

void ledLeftShift(uint8_t num);
void ledLeftShift_Start(void);
void ledLeftShift_Stop(uint8_t num);

//static void ledOneOn(uint8_t index);

void ledLeftRightShift_Task(uint8_t num);

void ledLeftShift_Task(uint8_t num);
void ledRightShift_Task(uint8_t num);

#endif /* INC_LED_H_ */
