#ifndef IRremote_h
#define IRremote_h

#include "main.h"
#include "stdint.h"

#define IR_TIMER TIM2
#define IR_PIN GPIO_PIN_7
#define IR_PORT GPIOA
#define IR_AF GPIO_AF2_TIM3
#define IR_TIM_CHANNEL TIM_CHANNEL_2
#define GPIO_CLOCK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE();
#define TIM_CLOCK_ENABLE() __HAL_RCC_TIM3_CLK_ENABLE();
#define KHZ_OUT 38

typedef struct {
	uint16_t length;    // number of samples
	uint32_t* samples;
} irnec_t;

#define NEC_RAW_BITS 32

#define NEC_SAMPLES (2 + (NEC_RAW_BITS * 2) + 1) 

// Protocol timing
#define NEC_PULSE_US 562

#define NEC_LEAD_PULSE_US (16 * NEC_PULSE_US) // 9000 us
#define NEC_LEAD_SPACE_US (8 * NEC_PULSE_US)  // 4500 us

#define NEC_SPACE_1_US (3 * NEC_PULSE_US)     // 1687 us
#define NEC_SPACE_0_US (NEC_PULSE_US)         // 562 us

// Timing tolerances
#define NEC_PULSE_TOLERANCE (NEC_PULSE_US * 20 / 100)
#define NEC_SPACE_TOLERANCE (NEC_PULSE_US * 20 / 100)

#define NEC_PULSE_US_MIN (NEC_PULSE_US - NEC_PULSE_TOLERANCE)
#define NEC_PULSE_US_MAX (NEC_PULSE_US + NEC_PULSE_TOLERANCE)

#define NEC_LEAD_PULSE_US_MIN (NEC_LEAD_PULSE_US - NEC_PULSE_TOLERANCE)
#define NEC_LEAD_PULSE_US_MAX (NEC_LEAD_PULSE_US + NEC_PULSE_TOLERANCE)

#define NEC_LEAD_SPACE_US_MIN (NEC_LEAD_SPACE_US - NEC_SPACE_TOLERANCE)
#define NEC_LEAD_SPACE_US_MAX (NEC_LEAD_SPACE_US + NEC_SPACE_TOLERANCE)

#define NEC_SPACE_0_US_MIN (NEC_SPACE_0_US - NEC_SPACE_TOLERANCE)
#define NEC_SPACE_0_US_MAX (NEC_SPACE_0_US + NEC_SPACE_TOLERANCE)

#define NEC_SPACE_1_US_MIN (NEC_SPACE_1_US - NEC_SPACE_TOLERANCE)
#define NEC_SPACE_1_US_MAX (NEC_SPACE_1_US + NEC_SPACE_TOLERANCE)

void nec_encode(uint8_t address, uint8_t command, irnec_t* signal);
void ir_enable_transmit(uint16_t khz);
void ir_send_space(uint32_t time);
void ir_send_mark(uint32_t time);
void ir_transmit(uint32_t buf[], uint16_t len, uint16_t khz);

#endif