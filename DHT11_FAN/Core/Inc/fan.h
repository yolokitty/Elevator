/*
 * fan.h
 *
 *  Created on: Jan 28, 2026
 *      Author: appletea
 */

#ifndef INC_FAN_H_
#define INC_FAN_H_

#include "main.h"
#include "tim.h"

typedef struct
{
  GPIO_TypeDef  *port;
  uint16_t      number;
  GPIO_PinState onState;
  GPIO_PinState offState;
}Segment_LED;

extern volatile uint8_t Flag;

void Fan();


#endif /* INC_FAN_H_ */
