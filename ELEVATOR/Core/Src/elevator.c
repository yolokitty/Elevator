/*
 * elevator.c
 *
 *  Created on: Feb 5, 2026
 *      Author: jsh-laptop
 */


#include "elevator.h"

COM ele;
BUD fl;

// 버튼 위치
BTNN BTN_data[12] =
{
    // up
    {GPIOC, GPIO_PIN_10,bt_released,0,0},
    {GPIOC, GPIO_PIN_11,bt_released,0,0},
    {GPIOC, GPIO_PIN_12,bt_released,0,0},
    {GPIOD, GPIO_PIN_2,bt_released,0,0},
    // down
    {GPIOC, GPIO_PIN_2,bt_released,0,0},
    {GPIOC, GPIO_PIN_1,bt_released,0,0},
    {GPIOC, GPIO_PIN_3,bt_released,0,0},
    {GPIOC, GPIO_PIN_0,bt_released,0,0},

    {GPIOB, GPIO_PIN_4,bt_released,0,0},
    {GPIOB, GPIO_PIN_5,bt_released,0,0},
    {GPIOB, GPIO_PIN_3,bt_released,0,0},
    {GPIOA, GPIO_PIN_10,bt_released,0,0},
};


LEDD LED_data[8]=
{
    {GPIOC,GPIO_PIN_8},
    {GPIOC,GPIO_PIN_6},
    {GPIOC,GPIO_PIN_5},
    {GPIOA,GPIO_PIN_12},
    {GPIOA,GPIO_PIN_11},
    {GPIOB,GPIO_PIN_12},
    {GPIOB,GPIO_PIN_2},
    {GPIOB,GPIO_PIN_10}
};


Photo photo_data[TOP] =
{
    {GPIOA,GPIO_PIN_0},
    {GPIOA,GPIO_PIN_1},
    {GPIOA,GPIO_PIN_4},
    {GPIOB,GPIO_PIN_0},
};


bool btn_tPressed(uint8_t num)
{
    // 1. 버튼이 눌렸는지 확인 (Active Low 기준: 누르면 0)
    if (HAL_GPIO_ReadPin(BTN_data[num].port, BTN_data[num].pin) == GPIO_PIN_RESET)
    {
        if (BTN_data[num].flag == 0) // 처음 눌림 탐지
        {
            BTN_data[num].flag = 1;
            BTN_data[num].pretime = HAL_GetTick(); // 시작 시간 저장
        }
        else if (BTN_data[num].flag == 1) // 눌려 있는 상태 유지 중
        {
            if (HAL_GetTick() - BTN_data[num].pretime >= 50) // 50ms 경과 확인
            {
                BTN_data[num].flag = 2; // '확정' 상태로 변경
                return true; // 여기서 true 반환 (눌린 시점 딱 한 번)
            }
        }
        // flag가 2인 상태에서는 계속 눌려 있어도 여기서 true를 반환하지 않음 (중복 방지)
    }
    else // 버튼을 떼면
    {
        BTN_data[num].flag = 0; // 리셋하여 다시 누를 준비
    }

    return false;
}

//BTN_data를 바꾼다.
void btnCheck()
{
  for(int i=0;i<12;i++)
  {
    if(btn_tPressed(i)) BTN_data[i].state = bt_pushed;
    //else                BTN_data[i].state = bt_released;
  }
}

void elvinit(COM *ele, BUD *fl)
{
  for(int i=0;i<MAX;i++)
  {
    ele->num[i].state       = idle;
    ele->num[i].prestate    = idle;
    ele->num[i].current     = 0;  // 처음위치.
    ele->num[i].goal        = 0;
    ele->num[i].absol_idx   = 0;
    ele->num[i].seq_idx     = 0;

    for(uint8_t j=0; j<TOP;j++)
    {
      ele->num[i].reserve[j] = 0;
    }
    // 층 예약 초기화
    fl->floor[i].downstate  = fl_none;
    fl->floor[i].upstate    = fl_none;
  }
}

// 예약 거는 엘레베이터
// 논블로킹이라서 여기에 카운트를 넣을수는 없다.
void res(COM *ele, BUD * fl)
{
  btnCheck();
  for(int i=0; i<TOP;i++)
  {
    if(fl->floor[i].upstate == fl_none && BTN_data[i].state == bt_pushed)
    {
      fl->floor[i].upstate = fl_Up;
      BTN_data[i].state = bt_released;
      HAL_GPIO_WritePin(LED_data[i].port, LED_data[i].pin, 1);
      return;
    }
    else if(fl->floor[i].upstate == fl_Up && BTN_data[i].state == bt_pushed)
    {
      fl->floor[i].upstate = fl_none;
      BTN_data[i].state = bt_released;
      HAL_GPIO_WritePin(LED_data[i].port, LED_data[i].pin, 0);
      return;
    }


    if(fl->floor[i].downstate == fl_none && BTN_data[i+TOP].state == bt_pushed) // 예약
    {
      fl->floor[i].downstate = fl_Down;
      BTN_data[i+TOP].state = bt_released;
      //HAL_GPIO_WritePin(LED_data[i+TOP].port, LED_data[i+TOP].pin, 1);
      return;
    }
    else if(fl->floor[i].downstate == fl_Down && BTN_data[i+TOP].state == bt_pushed) // 예약 걸린상태로 다시 누르면 취소. (토글)
    {
      fl->floor[i].downstate = fl_none;
      BTN_data[i+TOP].state = bt_released;
      //HAL_GPIO_WritePin(LED_data[i+TOP].port, LED_data[i+TOP].pin, 0);
      return;
    }

    if(ele->num[0].reserve[i] == ele_none && BTN_data[i + 2*TOP].state == bt_pushed)
    {
      ele->num[0].reserve[i] = ele_busy;
      BTN_data[i+2*TOP].state = bt_released;
      HAL_GPIO_WritePin(LED_data[i+TOP].port, LED_data[i+TOP].pin, 1); //예약은 걸림
      return;
    }
    if(ele->num[0].reserve[i] == ele_busy && BTN_data[i + 2*TOP].state == bt_pushed) //왜 취소는 안됨?
    {
      ele->num[0].reserve[i] = ele_none;
      BTN_data[i+2*TOP].state = bt_released;
      HAL_GPIO_WritePin(LED_data[i+TOP].port, LED_data[i+TOP].pin, 0);
      return;
    }
  }

}




// 현재층을 밷는 함수
// 절차무시하고 포토 인터럽트 찍힌 곳을 위치로 할당
void fl_check(COM *ele, BUD *fl)
{
  for(uint8_t i=0; i<TOP;i++)
  {
    if(HAL_GPIO_ReadPin(photo_data[i].port, photo_data[i].pin))
    {
      ele->num[0].current = i;
    }
  }
}


// 처음 명령에 강하게 의존해야 한다.
// 1. 나보다 높은층에서 위로 버튼을 누른다. 그러면 위로 가면서 모든 위층들을 쓸면서 간다.
// 2. 나보다 낮은 층에서 아래로 버튼을 누른다. 그러면 아래로 가면서 모든 아래층을 쓸면서 간다.
// 3. 나보다 높은 층에서 아래로 버튼을 누른다. 그러면 신호들을 다 무시하고 해당층에서 도착해서 아래로 쓸면서 간다.
// 4. 나보다 낮은 층에서 위로 버튼을 누른다. 그러면 신호들을 다 무시하고 해당층에 도착해서 위로 쓸면서 간다.

// for문의 우선탐색 순서는 엘레베이터 기준임.
void Do(COM *ele,BUD*fl)
{
  int idx = ele->num[0].current;

  // 강제로 위를 본다.
  if(ele->num[0].prestate == force_up)
  {
    ele->num[0].prestate = idle; // 딱 한번만 실행.
    for(int i=idx+1; i < TOP;i++)
    {
      // 위로가는 예약이 있으면 그리로 간다.
      if(fl->floor[i].upstate == fl_Up || ele->num[0].reserve[i] == ele_busy)
      {
        ele->num[0].seq_idx = i;
        ele->num[0].goal = i;
        ele->num[0].state = go_up;
        return;
      }
      // 아래로 가는 예약이 있더라도 위로가야 세련됨(desparate)

    }
  }
  else if(ele->num[0].prestate == force_down)
  {
    ele->num[0].prestate = idle;
    for(int i=idx-1; i >= 0;i--)
    {
      if(fl->floor[i].upstate == fl_Down || ele->num[0].reserve[i] == ele_busy)
      {
        ele->num[0].seq_idx = i;
        ele->num[0].goal = i;
        ele->num[0].state = go_down;
        return;
      }
    }
  }
  else
  {
    // 순항모드

    // 위로 탐색
    for(int i=idx+1; i<TOP;i++)
    {
      if(fl->floor[i].upstate == fl_Up || ele->num[0].reserve[i] == ele_busy)
      {
        ele->num[0].seq_idx = i;
        ele->num[0].goal = i;             // 일단 추가.
        ele->num[0].state = go_up;
        return;
      }
    }
    // 위에 없으면 위에 다운 탐색
    for(int i=idx-1; i>=0;i--)
    {
      if(fl->floor[i].downstate == fl_Down || ele->num[0].reserve[i] == ele_busy)
      {
        ele->num[0].seq_idx = i;
        ele->num[0].goal = i;             // 일단 추가
        ele->num[0].state = go_down;
        return;
      }
    }
    //절대명령 - 혼잡한 예약인 경우 우선순위를 낮게 둠 순항모드의 우선권 높음 부여
    for(int i=idx-1;i>=0;i--)
    {
      if(fl->floor[i].upstate == fl_Up) // 나보다 아래층에서 위로 올라가는 명령이 있다면 다른거 다 무시하고 도착 후 go_up
      {
        ele->num[0].absol_idx = i;
        ele->num[0].seq_idx = i; // Open 에서 이를 비교하는 로직이 있음
        ele->num[0].state = force_up;
        return;
      }
    }
    for(int i=idx+1; i<TOP;i++)
    {
      if(fl->floor[i].downstate == fl_Down) // 내 위층에서 내려가는 명령이 걸리면 다 무시하고 도착 후 go_down
      {
        ele->num[0].absol_idx = i;
        ele->num[0].seq_idx = i;
        ele->num[0].state = force_down;
        return;
      }
    }
  }
  // 예약없음======================================추가==========================================================================
  // 같은 층에서 눌렀을때 문이 열리는 동작을 위해서 waiting으로 가야한다.
  if(fl->floor[idx].upstate == fl_Up || fl->floor[idx].downstate == fl_Down)
  {
    ele->num[0].prestate = idle;
    ele->num[0].state = waiting;
    return;
  }
}


void MT(COM*ele, Stepper_t *motor)
{
  if(ele->num[0].state == go_up || ele->num[0].state == force_down)
  {
    Stepper_Start(motor,2, DIR_CCW,1100);
  }
  else if(ele->num[0].state == go_down || ele->num[0].state == force_up)
  {
    Stepper_Start(motor,2, DIR_CW,1100);
  }
}

// 이전 상태를 기억해서 다음 목적지 선택
bool preGoal(COM*ele,BUD*fl)
{
  uint8_t idx = ele->num[0].current;

  if(ele->num[0].prestate == go_up && idx <TOP-1)
  {
    for(int i=idx+1;i<TOP;i++)
    {
      if(fl->floor[i].upstate == fl_Up || ele->num[0].reserve[i] == ele_busy)
      {
        ele->num[0].goal = i;
        ele->num[0].state = idle;
        return true;
      }
    }
    // 예약이 없는 경우 때문에 반드시 idle로 설정해야 한다.
  }
  else if(ele->num[0].prestate == go_down && idx > 0)
  {
    for(int i=idx-1; i>=0;i--)
    {
      if(fl->floor[i].downstate == fl_Down || ele->num[0].reserve[i] == ele_busy)
      {
        ele->num[0].goal = i;
        ele->num[0].state = idle;
        return true;
      }
    }
    // 예약이 없는 경우
  }
  return false;
}
// 문제없던 이놈이 말썽이네 이제
void ABSGoing(COM *ele,BUD *fl, Stepper_t *motor)
{
  uint8_t idx = ele->num[0].current; // 체크함수가 있음
  static uint8_t f_flag = 0;
  // 강제이동 명령이 취소되었으면 중간에 정지(강제이동은 최초의 예약에만 적용되므로 idle상태 진입할때 안에 사람이 없다고 가정) ================

  // force_up은 하강상태임.
  if(!f_flag && fl->floor[ele->num[0].absol_idx].upstate == fl_none && ele->num[0].state == force_up)
  {
    f_flag = 1;
    ele->num[0].absol_idx = idx - 1;
  }
  else if(!f_flag && fl->floor[ele->num[0].absol_idx].downstate == fl_none && ele->num[0].state == force_down)
  {
    f_flag = 1;
    ele->num[0].absol_idx = idx + 1;
  }


  if(ele->num[0].absol_idx == idx)
  {
    f_flag = 0;
    ele->num[0].prestate = ele->num[0].state;    // 이전 상태를 기억
    ele->num[0].state = waiting;
  }

  MT(ele,motor);
}

// 순차 주행도 목표 인덱스를 정해야 하나?
// 그렇게 해서 내 첫위치와 목표 인덱스 사이 범위만 스캔?? 취소도 고려?...
// 경유해서 다시 골함수로 돌아오면 무슨이유인지 goal이 최신화가 안된다.
// goal = 1, seq_idx = 2, current = 1
void Goal(COM*ele,BUD*fl)
{
  uint8_t idx = ele->num[0].current;
  if(idx == ele->num[0].seq_idx) return;

  // 바로 위, 바로 아래층만 검사.
  if(ele->num[0].state == go_up && idx <TOP-1)
  {
    if(fl->floor[idx+1].upstate == fl_Up || ele->num[0].reserve[idx+1] == ele_busy)
    {
      ele->num[0].goal = idx+1;
      return;
    }
    else
    {
      ele->num[0].goal = ele->num[0].seq_idx; // 예약 취소 변경 ====================================================
      return;
    }
  }
  else if(ele->num[0].state == go_down && idx > 0)
  {
    if(fl->floor[idx-1].downstate == fl_Down || ele->num[0].reserve[idx-1] == ele_busy)
    {
      ele->num[0].goal = idx-1;
      return;
    }
    else
    {
      ele->num[0].goal = ele->num[0].seq_idx;
      return;
    }
  }
}

// 이동하면서..중간에 멈출자리가 있으면 멈춰야 한다.
// 자 두번쨰 세번쨰 오면서 버그가 생겨..
void Going(COM *ele,BUD *fl, Stepper_t *motor)
{

  uint8_t idx = ele->num[0].current;
  uint8_t seq = ele->num[0].seq_idx;
  static uint8_t a_flag = 0;

  // seq_idx가 예약 취소되있는 경우
  // 1. 상승중이면서 위쪽 예약이 취소된경우
  // 2. 하강중이면서 아래쪽 예약이 취소된경우
  // 3. 엘레베이터 예약이 걸린 경우-층 에약과 무관하게 idle로 가면 안된다.

  if( !a_flag && ele->num[0].state == go_up && fl->floor[seq].upstate == fl_none && ele->num[0].reserve[seq] == ele_none)
  {
    // 최선은 근
    a_flag = 1;
    ele->num[0].seq_idx = idx + 1;
  }
  else if(!a_flag &&ele->num[0].state == go_down && fl->floor[seq].downstate == fl_none&& ele->num[0].reserve[seq] == ele_none)
  {
    ele->num[0].seq_idx = idx - 1;
    a_flag = 1;
  }

  if(idx == ele->num[0].seq_idx || idx == ele->num[0].goal)
  {
    a_flag = 0;
    ele->num[0].prestate = ele->num[0].state;
    ele->num[0].state = waiting;
    return;
  }

  if(!a_flag)
  {
    Goal(ele, fl); // 순서 변경 금지
  }

  MT(ele,motor);

}


// 자 절대로 온경우와 일반으로 온 경우가 있다.
void Open(COM * ele, BUD * fl)
{
  uint8_t idx = ele->num[0].current;
  // 예약 지우기 - 절대명령은 층에서 하는것이 아닌 엘레베이터에서 인식하는 것임 제거는 똑같이
  if(ele->num[0].prestate == go_up || ele->num[0].prestate == force_up)
  {
    fl->floor[idx].upstate = fl_none;
    HAL_GPIO_WritePin(LED_data[idx].port, LED_data[idx].pin, 0);
  }
  else if(ele->num[0].prestate == go_down || ele->num[0].prestate == force_down)
  {
    fl->floor[idx].downstate = fl_none;
  }
  else if(ele->num[0].prestate == idle) // 같은 층의 위,아래 버튼을 누른경우
  {
    if(fl->floor[idx].upstate == fl_Up)
    {
      fl->floor[idx].upstate = fl_none;
    }
    else if(fl->floor[idx].downstate == fl_Down)
    {
      fl->floor[idx].downstate = fl_none;
    }
  }
  ele->num[0].reserve[idx] = ele_none;
  HAL_GPIO_WritePin(LED_data[idx+TOP].port, LED_data[idx+TOP].pin, 0);

  // start 타이머
  static uint8_t flag = 0;
  static uint32_t prevTime = 0;
  if(flag == 0)
  {
    flag = 1;
    prevTime = HAL_GetTick();
  }
  else if(flag == 1 && ((HAL_GetTick() - prevTime) > 5000))
  {
    flag = 2;
  }
  // 타이머 end

  if(flag == 2)
  {
    // 반드시 위로 올라간다.- 근데 예약된게 없으면 IDLE
    if(ele->num[0].prestate == force_up)
    {
      flag = 0;
      ele->num[0].state = idle; // Do 함수에서 우선순위 스캔을 해줘야 한다.
      return;
      // 여기서 위의 예약이 시간이 지나기 전에 갱신되지 않으면 아래예약을 볼 수 밖에 없음.
    }
    else if(ele->num[0].prestate == force_down)
    {
      flag = 0;
      ele->num[0].state = idle;
      return;
    }

    // 자 seq_idx에 도달하기 전까지 going에서 waiting을 돌꺼야
    // seq_idx에 도달하면 그떄부터 아래의 이 코드를 하는거지.
    if(idx != ele->num[0].seq_idx)
    {
      flag = 0;
      // 다음 골을 정해줘야 함.
      preGoal(ele, fl);
      ele->num[0].state = ele->num[0].prestate;
      return;
    }
    // 전부 아닌 경우는 그냥 idle로 가자.(재판단해)
    flag = 0;
    ele->num[0].state = idle;
    return;
  }
}






