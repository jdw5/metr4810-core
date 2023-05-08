#include "ir_transmit.h"
#include "main.h"

TIM_HandleTypeDef htim3;

// Macro for the DWT library which must be included
#define US_DELAY(us) DWT_Delay_us(us)

/**
* @brief	Transmit the providied buffer over the IR
* @param buf 
* @param len
* @param khz	
*/
void ir_transmit(uint32_t buf[], uint16_t len, uint16_t khz) {
	// Set IR carrier frequency
	//ir_enable_transmit(khz);

	for (uint16_t i = 0;  i < len;  i++) {
		if (i & 1) {
            ir_send_space(buf[i]);
        } else {
            ir_send_mark(buf[i]);
        }
	}
	// Send a low to signL the LED is no longer transmitting
	ir_send_space(0);
}


/**
* @brief 	Generate a high signal (1) for the specified time
* @param time
*/
void ir_send_mark(uint32_t time) {
	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_2); // Enable PWM output

	if (time > 0) {
        US_DELAY(time);
    }
}

/**
* @brief 	Generate a low signal (0) for the specified time
* @param	time
*/
void ir_send_space(uint32_t time) {
	HAL_TIM_OC_Stop(&htim3, TIM_CHANNEL_2); // Disable PWM output

	if (time > 0) {
        US_DELAY(time);
    }
}

// Enables IR output.  The khz value controls the modulation frequency in kilohertz.
// To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
//
/**
* @brief 	Initialise the IR transmitter for output and configure modulation settings
*			Modulation is done by turning the pin high and low
*/
void ir_enable_transmit(uint16_t khz) {
	GPIO_InitTypeDef GPIO_IR_TIMER_PWM;
	TIM_OC_InitTypeDef IR_TIMER_PWM_CH;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();

	GPIO_IR_TIMER_PWM.Pin = GPIO_PIN_7;
	GPIO_IR_TIMER_PWM.Mode = GPIO_MODE_AF_PP;
	GPIO_IR_TIMER_PWM.Pull = GPIO_NOPULL;
	GPIO_IR_TIMER_PWM.Speed = GPIO_SPEED_HIGH;
	GPIO_IR_TIMER_PWM.Alternate = GPIO_AF2_TIM3;

	HAL_GPIO_Init(GPIOA, &GPIO_IR_TIMER_PWM);

	HAL_TIM_OC_DeInit(&htim3);

	/* PWM_frequency = timer_tick_frequency / (TIM_Period + 1) */

	htim3.Instance = TIM3;
	uint32_t period = 1000 / khz;
	htim3.Init.Period = period & 0xFFFF;
	htim3.Init.Prescaler = 100;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	HAL_TIM_Base_Init(&htim3);
	HAL_TIM_OC_Init(&htim3);

	/* PWM mode 2 = Clear on compare match */
	/* PWM mode 1 = Set on compare match */
	IR_TIMER_PWM_CH.OCMode = TIM_OCMODE_PWM1;

	/* To get proper duty cycle, you have simple equation */
	/* pulse_length = ((TIM_Period + 1) * DutyCycle) / 100 - 1 */
	/* where DutyCycle is in percent, between 0 and 100% */

	IR_TIMER_PWM_CH.Pulse = (((uint32_t)period)/2) & 0xFFFF;
	IR_TIMER_PWM_CH.OCPolarity = TIM_OCPOLARITY_HIGH;
	IR_TIMER_PWM_CH.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	IR_TIMER_PWM_CH.OCFastMode = TIM_OCFAST_DISABLE;
	IR_TIMER_PWM_CH.OCIdleState = TIM_OCIDLESTATE_RESET;
	IR_TIMER_PWM_CH.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	HAL_TIM_OC_ConfigChannel(&htim3, &IR_TIMER_PWM_CH, TIM_CHANNEL_2);
	TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, TIM_CCxN_ENABLE | TIM_CCx_ENABLE );

	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_2); // start generating IR carrier

    DWT_Delay_Init();
}

/**
* @brief 	Encode the provided address and command into the signal pointer
*			The values in the signal data will be the time delay in microseconds
*/
void nec_encode(uint8_t address, uint8_t command, irnec_t* signal)
{
    uint8_t data_byte[4];

    int sample_idx = 0;

    signal->samples[sample_idx++] = NEC_LEAD_PULSE_US;
    signal->samples[sample_idx++] = NEC_LEAD_SPACE_US;

    data_byte[0] = address;
    data_byte[1] = ~address;
    data_byte[2] = command;
    data_byte[3] = ~command;

    for (int b = 0; b < 4; b++) {
        for (int i = 0; i < 8; i++) {
            int data_bit = (data_byte[b] >> i) & 0x1;

            signal->samples[sample_idx++] = NEC_PULSE_US;

            if (data_bit)
                signal->samples[sample_idx++] = NEC_SPACE_1_US;
            else
                signal->samples[sample_idx++] = NEC_SPACE_0_US;
        }
    }

    signal->samples[sample_idx] = NEC_PULSE_US;

    signal->length = NEC_SAMPLES;
}