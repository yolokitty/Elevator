#ifndef INC_DHT11_H_
#define INC_DHT11_H_


#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "stdbool.h"
#include "tim.h"
#include "delay.h"
#include "stdio.h"

//// dht11.h
//extern TIM_HandleTypeDef htim4;

enum
{
    INPUT,
    OUTPUT
};


typedef struct
{
    GPIO_TypeDef    *port;
    uint16_t            pin;
    uint8_t                temperature;
    uint8_t                humidity;
}DHT11;


void dht11Init(DHT11 *dht, GPIO_TypeDef *port, uint16_t pin);
void dht11GpioMode(DHT11 *dht, uint8_t mode);
uint8_t dht11Read(DHT11 *dht);

#endif /* INC_DHT11_H_ */
