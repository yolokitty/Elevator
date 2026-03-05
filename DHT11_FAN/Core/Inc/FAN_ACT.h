/*
 * FAN_ACT.h
 *
 *  Created on: 2026. 2. 5.
 *      Author: junwoo
 */

#ifndef INC_FAN_ACT_H_
#define INC_FAN_ACT_H_

#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "stdbool.h"
#include "tim.h"
#include "delay.h"
#include "stdio.h"
#include "dht11.h"

extern volatile uint8_t Flag;

void FAN_ACT(DHT11 *dht);

#endif /* INC_FAN_ACT_H_ */
