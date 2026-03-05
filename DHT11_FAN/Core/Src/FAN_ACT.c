/*
 * FAN_ACT.c
 *
 *  Created on: 2026. 2. 5.
 *      Author: junwoo
 */
#include "FAN_ACT.h"
#include "stdio.h"
#include "i2c_lcd.h"

// main.c에 선언된 변수를 가져다 쓰기 위해 extern 선언
extern char lcd_buf[20];

volatile uint8_t Flag = 0;

void FAN_ACT(DHT11 *dht)
{
    // static 변수는 함수가 끝나도 값이 초기화되지 않고 유지됩니다.
    static uint32_t last_run_tick = 0;
    char f_buf[20];

    // 현재 시간이 마지막 실행 시간으로부터 2000ms가 지났는지 확인
    if (HAL_GetTick() - last_run_tick < 2000)
    {
        // 2초가 안 지났으면 아무것도 안 하고 바로 함수 종료 (Non-blocking)
        return;
    }

    // --- 여기서부터는 2초마다 한 번씩만 실행됨 ---
    last_run_tick = HAL_GetTick(); // 실행 시간 업데이트

    if(dht11Read(dht))
    {
        move_cursor(0,0);
        sprintf(f_buf, "Temp: %d C        ", dht->temperature);
        lcd_string(f_buf);

        move_cursor(1,0);
        sprintf(f_buf, "Humi: %d %%       ", dht->humidity);
        lcd_string(f_buf);

        printf("Temperature: %d C, Humidity: %d %%\r\n", dht->temperature, dht->humidity);

        // 팬 돌리는 조건
         if(dht->temperature >= 25)
         {
        	 Flag=1;
         }
         else if(dht->temperature < 25)
         {
        	 Flag=0;
         }
    }
    else
    {
        move_cursor(0,0);
        sprintf(f_buf, "Sensor Error   ");
        lcd_string(f_buf);

        move_cursor(1,0);
        sprintf(f_buf, "Check Connection");
        lcd_string(f_buf);

        printf("Error");
    }
}
