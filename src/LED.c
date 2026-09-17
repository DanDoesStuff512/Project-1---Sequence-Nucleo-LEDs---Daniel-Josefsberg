// ***********************************************************************
// * Author: Dan Josefsberg
// * CPEG222 Project 1, 9/16/2026
// * NucleoF466ZE CMSIS Sequence Nucleo LEDs with USR BTN
// ***********************************************************************
#include "stm32f4xx.h"
#include <stdbool.h>

#define BTN_PIN 13
#define BTN_PORT GPIOC

#define LED_R_PIN 14
#define LED_G_PIN 0
#define LED_B_PIN 7
#define LED_PORT GPIOB

volatile uint32_t last_time = 0;
volatile uint8_t state = 0;
volatile uint32_t ms = 0;

void SysTick_Handler(void) {
    ms++;
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1 << BTN_PIN)) {
        if (ms - last_time > 50) {
            last_time = ms;
            state = (state + 1) % 3;

            if (state == 0) {
                LED_PORT->BSRR = (1 << LED_R_PIN) | (1 << (LED_G_PIN + 16)) | (1 << (LED_B_PIN + 16));
            } else if (state == 1) {
                LED_PORT->BSRR = (1 << LED_G_PIN) | (1 << (LED_R_PIN + 16)) | (1 << (LED_B_PIN + 16));
            } else {
                LED_PORT->BSRR = (1 << LED_B_PIN) | (1 << (LED_R_PIN + 16)) | (1 << (LED_G_PIN + 16));
            }
        }

        EXTI->PR |= (1 << BTN_PIN);
    }
}

int main(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    LED_PORT->MODER &= ~(0x3 << (LED_R_PIN * 2));
    LED_PORT->MODER |=  (0x1 << (LED_R_PIN * 2));
    LED_PORT->MODER &= ~(0x3 << (LED_G_PIN * 2));
    LED_PORT->MODER |=  (0x1 << (LED_G_PIN * 2));
    LED_PORT->MODER &= ~(0x3 << (LED_B_PIN * 2));
    LED_PORT->MODER |=  (0x1 << (LED_B_PIN * 2));

    BTN_PORT->MODER &= ~(0x3 << (BTN_PIN * 2));

    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PC;
    EXTI->IMR  |= (1 << BTN_PIN);
    EXTI->FTSR |= (1 << BTN_PIN);

    NVIC_EnableIRQ(EXTI15_10_IRQn);

    SysTick_Config(SystemCoreClock / 1000);

    LED_PORT->BSRR = (1 << LED_R_PIN) | (1 << (LED_G_PIN + 16)) | (1 << (LED_B_PIN + 16));

    while (1) {}
}
