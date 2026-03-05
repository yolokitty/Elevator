/*
 * stepper.h
 *
 *  Created on: Feb 4, 2026
 *      Author: jsh-laptop
 */

#ifndef INC_STEPPER_T_H_
#define INC_STEPPER_T_H_

#include "stm32f4xx_hal.h"



/* ============================================================
 * Stepper Motor Direction 정의
 * ============================================================ */
#define DIR_CW      0       // 시계 방향
#define DIR_CCW     1       // 반시계 방향


/* ============================================================
 * Stepper 상태 머신 정의
 * ------------------------------------------------------------
 * 산업용 드라이버는 대부분 FSM(State Machine) 구조 사용
 * ============================================================ */
typedef enum
{
    STEPPER_IDLE = 0,   // 모터 대기 상태
    STEPPER_RUN,        // 모터 회전 중
    STEPPER_STOP        // 모터 정지 처리 중

} StepperState_t;


/* ============================================================
 * Stepper Motor 객체 구조체
 * ------------------------------------------------------------
 * 하나의 모터를 하나의 객체로 관리
 * (멀티 모터 확장 가능 구조)
 * ============================================================ */
typedef struct
{
    StepperState_t state;       // 현재 상태

    uint8_t direction;          // 회전 방향
    uint8_t speedLevel;         // 속도 단계

    volatile uint16_t remainingSteps; // 남은 스텝 수 (ISR에서 감소)
    volatile uint8_t currentStep;     // 현재 Step Index

    volatile uint8_t busy;      // 모터 동작 여부 Flag

    TIM_HandleTypeDef *htim;    // 모터 제어용 Timer

} Stepper_t;


/* ============================================================
 * API 함수
 * ============================================================ */

// Stepper 초기화
void Stepper_Init(Stepper_t *motor, TIM_HandleTypeDef *htim);

// 모터 회전 시작 (각도 기반)
void Stepper_Start(Stepper_t *motor, uint16_t degrees, uint8_t direction, uint16_t speed);

// 모터 정지 요청
void Stepper_Stop(Stepper_t *motor);

// 모터 동작 여부 확인
uint8_t Stepper_IsBusy(Stepper_t *motor);

// FSM 실행 (메인 루프에서 호출)
void Stepper_Task(Stepper_t *motor);

// Timer ISR에서 호출되는 함수
void Stepper_ISR(Stepper_t *motor);




#endif /* INC_STEPPER_T_H_ */
