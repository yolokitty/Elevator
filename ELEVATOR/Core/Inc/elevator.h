/*
 * elevator.h
 *
 *  Created on: Feb 5, 2026
 *      Author: jsh-laptop
 */

#ifndef INC_ELEVATOR_H_
#define INC_ELEVATOR_H_

#include "main.h"
#include "stepper_t.h"
#include "stdbool.h"

#define MAX 1   // 사용 엘레베이터 대수 (미구현)
#define TOP 4   // 꼭대기 층


//예약 취소
enum
{
  no_can,
  can
};

// 버튼의 상태
enum
{
  bt_pushed,
  bt_released
};
// 해당층의 예약 상태
enum
{
  fl_none,
  fl_Up,
  fl_Down
};

// 엘레베이터 상태
enum
{
  idle,
  go_up,
  go_down,
  waiting,
  force_up,
  force_down
};

// 엘레베이터 예약
enum
{
  ele_none,
  ele_busy
};

// 엘레베이터의 구조체
typedef struct
{
  uint8_t state;          // 엘리베이터 예약 enum 참조
  uint8_t prestate;       // 관성 - 이동했던 방향우선 탐색
  uint8_t current;        // 현재 위치
  uint8_t goal;           // 목표 위치
  uint8_t reserve[TOP];   // 예약은 해당 층에 1이면 예약 0이면 미예약으로 하자.
  uint8_t absol_idx;      // 절대명령 - 층
  uint8_t seq_idx;        // 순항명령 - seq_idx는 중간목적지가 계속 갱신될수 있으므로 최종목적지가 있어야 경유할 수 있음.

} ELV;

// 엘레베이터 병렬 구조체
typedef struct
{
  ELV num[MAX];
}COM;


// 층의 구조체
typedef struct
{
  uint8_t upstate;    // None, up   예약
  uint8_t downstate;  // None,down  예약
} FLR;

typedef struct
{
  FLR floor[TOP];
}BUD;

// 버튼 구조체
typedef struct
{
  GPIO_TypeDef *port;
  uint16_t      pin;
  uint8_t       state;
  uint32_t      pretime;
  uint8_t       flag;
}BTNN;

typedef struct
{
  GPIO_TypeDef *port;
  uint16_t       pin;
}LEDD;

typedef struct
{
  GPIO_TypeDef *port;
  uint8_t       pin;
}Photo;


extern COM ele; //COM선언보다 위로 올리면 에러.
extern BUD fl;
extern BTNN BTN_data[12];
extern LEDD LED_data[8];
extern Photo photo_data[TOP];


void fl_check(COM *ele, BUD *fl);
void res(COM *ele, BUD * fl);
void ABSGoing(COM *ele,BUD *fl, Stepper_t *motor);
void Goal(COM *ele,BUD*fl);
bool preGoal(COM*ele,BUD*fl);
uint8_t check(COM *ele, BUD *fl);
void elvinit(COM *ele, BUD *fl);
void Do(COM *ele, BUD *fl);
void Going(COM *ele,BUD *fl, Stepper_t *motor);
void Open(COM *ele, BUD *fl);


#endif /* INC_ELEVATOR_H_ */








