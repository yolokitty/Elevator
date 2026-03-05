
#include "segment.h"

static const uint8_t segmentData[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x27, // 7
    0x7F, // 8
    0x67  // 9
};

void segmentDisplay(uint8_t num)
{
    if (num > 9) return;   // 보호 코드

    uint8_t data = segmentData[num];

    HAL_GPIO_WritePin(SEG_A_PORT, SEG_A_PIN, (data & (1 << 0)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_B_PORT, SEG_B_PIN, (data & (1 << 1)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_C_PORT, SEG_C_PIN, (data & (1 << 2)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_D_PORT, SEG_D_PIN, (data & (1 << 3)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_E_PORT, SEG_E_PIN, (data & (1 << 4)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_F_PORT, SEG_F_PIN, (data & (1 << 5)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_G_PORT, SEG_G_PIN, (data & (1 << 6)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
