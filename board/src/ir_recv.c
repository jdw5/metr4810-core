/*
 * Copyright (c) 2022 Jan Privara
 * SPDX-License-Identifier: MIT
 */

#include "ir_recv.h"
#include "main.h"

volatile irparams_t irparams;
volatile uint8_t[] irresults

static inline int in_range(uint32_t val, uint32_t min, uint32_t max)
{
    if (val >= min && val <= max)
        return 1;
    return 0;
}

int nec_decode_raw_bits(const nec_waveform_t* waveform, uint32_t * raw_bits)
{
    int sample_idx = 0;

    *raw_bits = 0;

    // check there is enough samples
    if (waveform->length < NEC_SAMPLES)
        return 0;

    // check leading pulse is within range
    if (!in_range(waveform->samples[sample_idx++],
                  NEC_LEAD_PULSE_US_MIN, NEC_LEAD_PULSE_US_MAX))
        return 0;

    // check leading space is within range
    if (!in_range(waveform->samples[sample_idx++],
                  NEC_LEAD_SPACE_US_MIN, NEC_LEAD_SPACE_US_MAX))
        return 0;

    for (int i = 0; i < NEC_RAW_BITS; i++) {
        uint32_t pulse = waveform->samples[sample_idx++];
        uint32_t space = waveform->samples[sample_idx++];

        // check the data pulse is within range
        if (!in_range(pulse, NEC_PULSE_US_MIN, NEC_PULSE_US_MAX))
            return 0;

        // decode raw bit 0 or 1 based on the space duration
        if (in_range(space, NEC_SPACE_1_US_MIN, NEC_SPACE_1_US_MAX))
            *raw_bits |= (1 << i);
        else if (!in_range(space, NEC_SPACE_0_US_MIN, NEC_SPACE_0_US_MAX))
            return 0;
    }

    // check the trailing pulse is within range
    if (!in_range(waveform->samples[sample_idx++],
                  NEC_PULSE_US_MIN, NEC_PULSE_US_MAX))
        return 0;

    return 1;
}

//------------------------------------------------------------------------------
int nec_decode(const nec_waveform_t * waveform, nec_decoded_data_t * res)
{
    uint32_t raw_bits_buf = 0;

    // decode raw bits
    if (!nec_decode_raw_bits(waveform, &raw_bits_buf))
        return 0;

    uint8_t addr     = (raw_bits_buf)       & 0xff;
    uint8_t addr_inv = (raw_bits_buf >> 8)  & 0xff;
    uint8_t cmd      = (raw_bits_buf >> 16) & 0xff;
    uint8_t cmd_inv  = (raw_bits_buf >> 24) & 0xff;

    // check address equals to inverted address
    if (addr != (~addr_inv & 0xff))
        return 0;

    // check command equals to inverted command
    if (cmd != (~cmd_inv & 0xff))
        return 0;

    // set the result
    res->address = addr;
    res->command = cmd;

    return NEC_SAMPLES;
}

// Allow all parts of the code access to the ISR data
// NB. The data can be changed by the ISR at any time, even mid-function
// Therefore we declare it as "volatile" to stop the compiler/CPU caching it
volatile irparams_t irparams;
volatile ir_decode_results irresults;

//+=============================================================================
void IRrecv_IRrecvInit (GPIO_TypeDef* recvpinport, uint16_t recvpin)
{
	irparams.recvpinport = recvpinport;
	irparams.recvpin = recvpin;
	irparams.blinkflag = 0;
}



// initialization
//
void IRrecv_enableIRIn()
{
	// Initialize state machine variables
	irparams.rcvstate = IR_STATE_IDLE;
	irparams.rawlen = 0;

	// Set pin modes
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = irparams.recvpin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(irparams.recvpinport, &GPIO_InitStruct);

	// Setup pulse clock timer interrupt
	// Prescale /500 (100M/500 = 5 microseconds per tick)
	// Period = 50us

	TIM_ClockConfigTypeDef sClockSourceConfig;

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 500;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 10;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

// Return if receiving new IR signals
uint8_t IRrecv_isIdle ( )
{
	return (irparams.rcvstate == IR_STATE_IDLE || irparams.rcvstate == IR_STATE_STOP) ? 1 : 0;
}

// Restart the ISR state machine
void IRrecv_resume ( )
{
	irparams.rcvstate = IR_STATE_IDLE;
	irparams.rawlen = 0;
}