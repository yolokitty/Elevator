### 📘 프로젝트 상세 설명

&nbsp;&nbsp;&nbsp;[![Notion](https://img.shields.io/badge/Notion-설명%20바로가기-black?style=for-the-badge&logo=notion&logoColor=white)](https://www.notion.so/31045465d27181ccb20dff8ea12b6a8c)

• **엘리베이터 제어 및 스마트 냉·난방 시스템**

대규모 확장이 가능한 엘리베이터 제어 시스템과 온습도 기반 스마트 냉·난방 실외기 자동 제어 로직을 통합 구현하였다.  
다양한 센서와 액추에이터를 활용하여 실시간 상태를 반영한 제어 시스템을 구성하였으며,  
인터럽트 기반 처리와 비동기 제어를 통해 안정성과 확장성을 확보하였다.  
  
  
  
**세부 기능**

엘리베이터 상하 이동 구현 (Stepper Motor)  
→ 스테퍼 모터를 이용하여 층 간 이동 제어  
→ 현재 층 정보를 실시간으로 출력  

Photo Interrupt를 활용한 층 위치 인식  
→ 센서를 통해 층 도달 여부 판단  

Button을 활용한 엘리베이터 호출  
→ 사용자 입력에 따른 층 이동 제어  

온습도 기반 냉·난방 제어  
→ 환경 데이터를 기반으로 실외기 자동 동작 제어  



**문제점 및 개선**

Polling 방식 사용 시 센서 값이 갱신되지 않는 문제  
→ EXTI(외부 인터럽트) 기반 Rising Edge 검출 방식으로 변경  

HAL_Delay() 사용으로 인한 시스템 블로킹 및 오측정 발생  
→ HAL_GetTick() 기반 Non-blocking 방식으로 변경하여 안정성 확보  

저속 구동 시 초기 구동력 부족 문제  
→ 기동 시 5ms 동안 최대 출력(Kick-start) 적용 후 정상 속도로 복귀  
