#include "stepper_t.h"



/* ============================================================
 * 28BYJ-48 모터 출력 핀 정의
 * ============================================================ */
#define IN1_PIN   GPIO_PIN_1
#define IN1_PORT  GPIOB
#define IN2_PIN   GPIO_PIN_15
#define IN2_PORT  GPIOB
#define IN3_PIN   GPIO_PIN_14
#define IN3_PORT  GPIOB
#define IN4_PIN   GPIO_PIN_13
#define IN4_PORT  GPIOB


/* ============================================================
 * Half Step Drive 시퀀스
 * ------------------------------------------------------------
 * 28BYJ-48 은 Half Step 모드 사용 시
 * 4096 Step = 1 회전
 * ============================================================ */
static const uint8_t HALF_STEP_SEQ[8][4] =
{
    {1,0,0,0},
    {1,1,0,0},
    {0,1,0,0},
    {0,1,1,0},
    {0,0,1,0},
    {0,0,1,1},
    {0,0,0,1},
    {1,0,0,1}
};


/* ============================================================
 * 속도 테이블
 * ------------------------------------------------------------
 * 단위 : Hz (Step Frequency)
 * Timer 인터럽트 발생 주기 = Step 발생 주기
 * ============================================================ */
static const uint16_t speedTable[] = {80,120,160,200,250,1100};

#define SPEED_TABLE_SIZE (sizeof(speedTable)/sizeof(speedTable[0]))


/* ============================================================
 * 내부 함수 : 타이머 속도 설정
 * ------------------------------------------------------------
 * Timer ARR 값을 변경하여 Step 속도 제어
 * ============================================================ */
static void Stepper_SetSpeed(Stepper_t *motor)
{
    // Speed Level 범위 체크
    if(motor->speedLevel >= SPEED_TABLE_SIZE)
        motor->speedLevel = SPEED_TABLE_SIZE - 1;

    uint16_t freq = speedTable[motor->speedLevel];

    // Timer 클럭 : 1MHz 기준
    uint16_t arr = (1000000 / freq) - 1;

    motor->htim->Instance->PSC = 100 - 1;
    motor->htim->Instance->ARR = arr;

    __HAL_TIM_SET_COUNTER(motor->htim, 0);
}


/* ============================================================
 * Stepper 초기화
 * ============================================================ */
void Stepper_Init(Stepper_t *motor, TIM_HandleTypeDef *htim)
{
    motor->state = STEPPER_IDLE;
    motor->busy = 0;
    motor->currentStep = 0;
    motor->remainingSteps = 0;
    motor->htim = htim;
}


/* ============================================================
 * 모터 회전 시작 함수
 * ------------------------------------------------------------
 * degrees : 회전 각도
 * direction : 회전 방향
 * speed : 속도 단계
 * ============================================================ */
void Stepper_Start(Stepper_t *motor, uint16_t degrees, uint8_t direction, uint16_t speed)
{
    // 모터 동작 중이면 명령 무시
    if(motor->busy)
        return;

    // 각도를 Step 수로 변환
    uint16_t steps = (uint32_t)degrees * 4096 / 360;

    motor->remainingSteps = steps;
    motor->direction = direction;
    motor->speedLevel = speed;
    motor->busy = 1;

    // Timer 속도 설정
    Stepper_SetSpeed(motor);

    // Timer 인터럽트 시작
    HAL_TIM_Base_Start_IT(motor->htim);

    motor->state = STEPPER_RUN;
}


/* ============================================================
 * 모터 정지 요청
 * ============================================================ */
void Stepper_Stop(Stepper_t *motor)
{
    motor->state = STEPPER_STOP;
}


/* ============================================================
 * 모터 Busy 확인
 * ============================================================ */
uint8_t Stepper_IsBusy(Stepper_t *motor)
{
    return motor->busy;
}


/* ============================================================
 * FSM Task
 * ------------------------------------------------------------
 * 메인 루프에서 주기적으로 호출
 * ============================================================ */
void Stepper_Task(Stepper_t *motor)
{
    switch(motor->state)
    {
        case STEPPER_IDLE:
            // 대기 상태
            break;

        case STEPPER_RUN:

            // 남은 Step이 없으면 정지 상태로 이동
            if(motor->remainingSteps == 0)
            {
                motor->state = STEPPER_STOP;
            }

            break;

        case STEPPER_STOP:

            // Timer 인터럽트 중지
            HAL_TIM_Base_Stop_IT(motor->htim);

            motor->busy = 0;
            motor->state = STEPPER_IDLE;

            break;
    }
}


/* ============================================================
 * Stepper ISR 처리
 * ------------------------------------------------------------
 * Timer 인터럽트 발생 시 호출
 * 실제 Step 출력 담당
 * ============================================================ */
void Stepper_ISR(Stepper_t *motor)
{
    // Step 종료 체크
    if(motor->remainingSteps == 0)
        return;

    /* 방향에 따른 Step 계산 */
    if(motor->direction == DIR_CW)
        motor->currentStep = (motor->currentStep + 1) % 8;
    else
        motor->currentStep = (motor->currentStep == 0) ? 7 : motor->currentStep - 1;


    /* GPIO 출력 */
    HAL_GPIO_WritePin(IN1_PORT, IN1_PIN, HALF_STEP_SEQ[motor->currentStep][0]);
    HAL_GPIO_WritePin(IN2_PORT, IN2_PIN, HALF_STEP_SEQ[motor->currentStep][1]);
    HAL_GPIO_WritePin(IN3_PORT, IN3_PIN, HALF_STEP_SEQ[motor->currentStep][2]);
    HAL_GPIO_WritePin(IN4_PORT, IN4_PIN, HALF_STEP_SEQ[motor->currentStep][3]);

    // Step 감소
    motor->remainingSteps--;
}
