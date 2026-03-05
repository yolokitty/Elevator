/*
 * led.c
 *
 *  Created on: Feb 6, 2026
 *      Author: kimsuyeon
 */


#include "led.h"


static uint8_t  shiftRun = 0;
static uint32_t shiftTick = 0;

static uint8_t  shiftIndex = 0;
static uint8_t  leftshiftIndex = 0;
static uint8_t  rightshiftIndex = 8;
static int8_t   shiftDir = 1;   // 방향도 전역으로



LED_CONTROL led[8]=
    {

        {GPIOC, GPIO_PIN_8, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOC, GPIO_PIN_6, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOC, GPIO_PIN_5, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOA, GPIO_PIN_12, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOA, GPIO_PIN_11, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOB, GPIO_PIN_12, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOB, GPIO_PIN_2, GPIO_PIN_SET, GPIO_PIN_RESET},
        {GPIOB, GPIO_PIN_10, GPIO_PIN_SET, GPIO_PIN_RESET},

    };


// LED 전부 On
void ledOn(uint8_t num)
{
  for(uint8_t i = 0; i < num; i++)
  {
    HAL_GPIO_WritePin(led[i].port, led[i].number, led[i].onState);
  }
}


// LED 전부 Off
void ledOff(uint8_t num)
{
  for(uint8_t i = 0; i < num; i++)
  {
    HAL_GPIO_WritePin(led[i].port, led[i].number, led[i].offState);
  }
}


// LED 토글
void ledToggle(uint8_t num)
{
  HAL_GPIO_TogglePin(led[num].port, led[num].number);
};

void ledLeftShift(uint8_t num){
  for(uint8_t i = 0; i < num; i++){
    ledOn(i);
    HAL_Delay(100);
  }
  HAL_Delay(500);
};


void ledLeftShift_Start(void)
{
    shiftRun = 1;
    shiftIndex = 0;
    shiftDir = 1;
    shiftTick = HAL_GetTick();
}

void ledLeftShift_Stop(uint8_t num)
{
    shiftRun = 0;
    shiftIndex = 0;
    ledOff(num);
}

void ledLefRightShift_Task(uint8_t num)
{
    if (!shiftRun) return;

    if (HAL_GetTick() - shiftTick >= 200)
    {
        shiftTick = HAL_GetTick();

        // 1. 전체 OFF
        ledOff(8);

        ledOn(shiftIndex);

        // 3. 인덱스 이동
        shiftIndex += shiftDir;

        // 4. 끝에서 방향 반전
        if (shiftIndex >= num)
        {
            shiftIndex = num;
            shiftDir = -1;
        }
        else if (shiftIndex <= 0)
        {
            shiftIndex = 0;
            shiftDir = 1;
        }
    }
}


void ledLeftShift_Task(uint8_t num)
{
    if (!shiftRun) return;

    if (HAL_GetTick() - shiftTick >= 200)
    {
        shiftTick = HAL_GetTick();

        // 1. 전체 OFF
        ledOff(8);

        // 2. 현재 위치까지 ON
        ledOn(leftshiftIndex % 9);

        // 3. 인덱스 증가
        leftshiftIndex++;
    }
}

void ledRightShift_Task(uint8_t num)
{
    if (!shiftRun) return;

    if (HAL_GetTick() - shiftTick >= 200)
    {
        shiftTick = HAL_GetTick();

        // 1. 전체 OFF
        ledOff(8);

        ledOn(rightshiftIndex);

        // 인덱스 감소
        if (rightshiftIndex == 0)
          rightshiftIndex = 8;
        else
          rightshiftIndex--;
    }
}
