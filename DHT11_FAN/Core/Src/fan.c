/*
 * fan.c
 *
 *  Created on: Jan 28, 2026
 *      Author: appletea
 */

#include "fan.h"


extern volatile uint8_t Flag;
static uint32_t t = 0; // 팬이 켜진 시점을 저장할 변수

void Fan()
{
	if(t==0)
	{
		t=HAL_GetTick();
	}
    // 1. 전원 버튼 처리 (Flag1)
    if (Flag)
    {
    	TIM2->CCR1 = 20;
    	if (HAL_GetTick() -t < 50)
    	{
    		TIM2->CCR1 = 49;
    	}
    }
    else
    {
    	TIM2->CCR1 = 0;
    	t=0;
    }
}

