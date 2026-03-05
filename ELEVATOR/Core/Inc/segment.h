/*
 * fnd.h
 *
 *  Created on: Feb 6, 2026
 *      Author: kimsuyeon
 */

#ifndef INC_SEGMENT_H_
#define INC_SEGMENT_H_


#include "stm32f4xx_hal.h"


/* ===== Segment Pins ===== */
/* a b c d e f g */
#define SEG_A_PORT   GPIOC
#define SEG_A_PIN    GPIO_PIN_9

#define SEG_B_PORT   GPIOB
#define SEG_B_PIN    GPIO_PIN_8

#define SEG_C_PORT   GPIOB
#define SEG_C_PIN    GPIO_PIN_9

#define SEG_D_PORT   GPIOA
#define SEG_D_PIN    GPIO_PIN_5

#define SEG_E_PORT   GPIOA
#define SEG_E_PIN    GPIO_PIN_7

#define SEG_F_PORT   GPIOB
#define SEG_F_PIN    GPIO_PIN_6

#define SEG_G_PORT   GPIOC
#define SEG_G_PIN    GPIO_PIN_7


void segmentDisplay(uint8_t num);

#endif /* INC_SEGMENT_H_ */
