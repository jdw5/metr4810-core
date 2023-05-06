/*
 * ir_recv.c
 *
 *  Created on: May 5, 2023
 *      Author: Joshua Wallace
 */

#include "stdint.h"
#include "main.h"

#define TIMER_COUNTER 512000
#define MAX_WIDTH 10000
#define THRESHOLD 2500

// Value gets progressively stored in this value as it gets recieved
uint32_t inputCode;

// Records the current bit position that is been written to.
uint8_t bitPosition;

// Store the last time for the edge detection
uint32_t lastTime;

//Prototypes

extern void ir_init() {

    // Set globals
    inputCode = 0;
    bitPosition = 0;
    lastTime = 0;

    __GPIOB_CLK_ENABLE();

    // Set fast speed
    GPIOB->OSPEEDR |= (GPIO_SPEED_FAST << 10);

    // No push/pull
    GPIOB->PUPDR &= ~(0x03 << (3 * 2));

    // Set to alternative function
    GPIOB->MODER &= ~(0x03 << (3 * 2));
    GPIOB->MODER |= (0x02 << (3 * 2));

    // Select Timer 2 as the alternate function
    GPIOB->AFR[1] &= ~((0x0F) << (2 * 4));
    GPIOB->AFR[1] |= (GPIO_AF1_TIM2 << (2 * 4));

    // Enable timer 2 clock.
    __TIM2_CLK_ENABLE();

    // Set prescalar clock.
    TIM2->PSC = ((SystemCoreClock / 2) / TIMER_COUNTER) - 1;

    // Count up
    TIM2->CR1 &= ~TIM_CR1_DIR;

    // Set for input capture/compare on timer input 1
    TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
    TIM2->CCMR2 |= TIM_CCMR2_CC3S_0;

    // Disable digital filtering to capture every event
    TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;

    // Set input prescalar to 0
    TIM2->CCMR2 &= ~TIM_CCMR2_IC3PSC;

    // Enable for falling edge
    TIM2->CCER &= ~(TIM_CCER_CC3NP | TIM_CCER_CC3P);
    TIM2->CCER |= (TIM_CCER_CC3P);

    // Enable Capture for channel 3
    TIM2->CCER |= TIM_CCER_CC2E;

    // Enable capture and update interrupts
    TIM2->DIER |= TIM_DIER_CC2IE;
    TIM2->DIER |= TIM_DIER_UIE;

    // Set priority 10 and enable callback.
    HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    // Enable
    TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * ISR for input capture for IR remote.
 * Calculates the key pressed/recieved over IR using input capture.
*/
void ir_recv() {

    // Read CCR value (resets IF flag)
    return;

}

/*
 * Interrupt handler for Timer 2 Interrupt Handler
 */
void TIM2_IRQHandler(void) {

    // Clear UIF flag if overflow occured.
    if ((TIM2->SR & TIM_SR_UIF) != TIM_SR_UIF) {

        TIM2->SR &= ~TIM_SR_UIF;
    }

    // Call callback if channel 3 triggered an interrupt
    if ((TIM2->SR & TIM_SR_CC3IF) == TIM_SR_CC3IF) {

        ir_recv();
    }
}

/**
 * Sets the varaible "value" to contain the value of the key pressed
 * and returns 1 if there was a value that is ready to be read.
 *
 * If no key has been pressed, this function returns a 0 and does not
 * modify the value varaible.
*/
extern uint8_t ir_get(char *value) {
    return 0x0;
}
