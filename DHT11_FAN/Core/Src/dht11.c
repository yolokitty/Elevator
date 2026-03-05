#include "dht11.h"
#include <stdio.h>

// main.c에서 선언된 타이머 핸들러 외부 선언
extern TIM_HandleTypeDef htim11; // 데이터 측정용
extern TIM_HandleTypeDef htim10; // delay_us용

// DHT11 초기화
void dht11Init(DHT11 *dht, GPIO_TypeDef *port, uint16_t pin)
{
    dht->port = port;
    dht->pin = pin;
    dht->temperature = 0;
    dht->humidity = 0;
}

// GPIO 모드 설정
void dht11GpioMode(DHT11 *dht, uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(mode == OUTPUT)
    {
        GPIO_InitStruct.Pin = dht->pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
    }
    else if(mode == INPUT)
    {
        GPIO_InitStruct.Pin = dht->pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL; // DHT11은 보통 외부 풀업 저항이 있음
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
    }
}

// DHT11 데이터 읽기
uint8_t dht11Read(DHT11 *dht)
{
    bool ret = true;
    uint16_t timeTick = 0;
    uint8_t pulse[40] = {0};
    uint16_t timeout = 0;

    uint8_t humValue1 = 0, humValue2 = 0;
    uint8_t tempValue1 = 0, tempValue2 = 0;
    uint8_t parityValue = 0;

    // 타이머 시작 (측정 및 지연용)
    HAL_TIM_Base_Start(&htim11);
    HAL_TIM_Base_Start(&htim10);

    // 1. 통신 시작 신호 전송
    dht11GpioMode(dht, OUTPUT);
    HAL_GPIO_WritePin(dht->port, dht->pin, GPIO_PIN_RESET);
    HAL_Delay(20); // 최소 18ms 이상
    HAL_GPIO_WritePin(dht->port, dht->pin, GPIO_PIN_SET);
    delay_us(20);

    dht11GpioMode(dht, INPUT); // 입력 모드로 변경

    // 2. DHT11 응답 신호 대기 (Low 응답)
    __HAL_TIM_SET_COUNTER(&htim11, 0);
    while(HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_RESET)
    {
        if(__HAL_TIM_GET_COUNTER(&htim11) > 100) {
            printf("DHT11: Low Signal Timeout\r\n");
            return false;
        }
    }

    // 3. DHT11 응답 신호 대기 (High 응답)
    __HAL_TIM_SET_COUNTER(&htim11, 0);
    while(HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_SET)
    {
        if(__HAL_TIM_GET_COUNTER(&htim11) > 100) {
            printf("DHT11: High Signal Timeout\r\n");
            return false;
        }
    }

    // 4. 40비트 데이터 비트 읽기
    for(uint8_t i = 0; i < 40; i++)
    {
        // Low 구간 대기 (약 50us)
        timeout = 0;
        while(HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_RESET) {
            if(++timeout > 10000) return false; // 무한루프 방지
        }

        // High 구간 시간 측정 시작
        __HAL_TIM_SET_COUNTER(&htim11, 0);
        timeout = 0;
        while(HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_SET)
        {
            timeTick = __HAL_TIM_GET_COUNTER(&htim11);
            if(++timeout > 10000) return false; // 무한루프 방지
        }

        // 시간 길이에 따른 0 또는 1 판별
        if(timeTick > 10 && timeTick < 35) {
            pulse[i] = 0;
        }
        else if(timeTick >= 35 && timeTick < 90) {
            pulse[i] = 1;
        }
    }

    // 타이머 정지
    HAL_TIM_Base_Stop(&htim11);

    // 5. 데이터 처리 (비트 결합)
    for(uint8_t i = 0; i < 8; i++)  humValue1 = (humValue1 << 1) + pulse[i];
    for(uint8_t i = 8; i < 16; i++) humValue2 = (humValue2 << 1) + pulse[i];
    for(uint8_t i = 16; i < 24; i++) tempValue1 = (tempValue1 << 1) + pulse[i];
    for(uint8_t i = 24; i < 32; i++) tempValue2 = (tempValue2 << 1) + pulse[i];
    for(uint8_t i = 32; i < 40; i++) parityValue = (parityValue << 1) + pulse[i];

    // 결과 저장
    dht->temperature = tempValue1;
    dht->humidity = humValue1;

    // 6. 체크섬 검증
    uint8_t checkSum = humValue1 + humValue2 + tempValue1 + tempValue2;
    if(checkSum != parityValue)
    {
        printf("DHT11: Check Sum Error (Calc:%d, Recv:%d)\r\n", checkSum, parityValue);
        ret = false;
    }

    return ret;
}
