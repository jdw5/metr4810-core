// Timer counter frequency for input capture
#define TIMER_COUNTER_FREQ 500000

// Max width of a pulse before ignoring the pulse.
#define MAX_WIDTH 100000

// Threshold value for determining a 1 or 0 for input capture.
#define THRESHOLD 25000 

// Flag that is set once a key has been pressed. 1 = pressed, 0 = no value
int keyPressedFlag;

// The value that was recieved over IR
char keyPressedValue;

// Value gets progressively stored in this value as it gets recieved
uint32_t inputCode;

// Records the current bit position that is been written to.
uint8_t bitPosition;

// Store the last time for the edge detection
uint32_t lastTime;

/**
 * Initialise IR reciever hardware and setup input capture.
 * 
 *  PB10 - timer 2 channel 3
*/
extern void irremote_init() { 

    // Set global vars
    keyPressedFlag = 0;
    keyPressedValue = 0;
    inputCode = 0;
    bitPosition = 0;
    lastTime = 0;

    __GPIOB_CLK_ENABLE();

    // Set fast speed
    GPIOB->OSPEEDR |= (GPIO_SPEED_FAST << 10);	

    // No push/pull
    GPIOB->PUPDR &= ~(0x03 << (10 * 2));

    // Set to alternative function
    GPIOB->MODER &= ~(0x03 << (10 * 2));
    GPIOB->MODER |= (0x02 << (10 * 2));

    // Select Timer 2 as the alternate function
    GPIOB->AFR[1] &= ~((0x0F) << (2 * 4));
    GPIOB->AFR[1] |= (GPIO_AF1_TIM2 << (2 * 4));

    // Enable timer 2 clock.
    __TIM2_CLK_ENABLE();

    // Set prescalar clock.
    TIM2->PSC = ((SystemCoreClock / 2) / TIMER_COUNTER_FREQ) - 1;

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
    TIM2->CCER |= TIM_CCER_CC3E;    

    // Enble capture and update interrupts
    TIM2->DIER |= TIM_DIER_CC3IE;		
    TIM2->DIER |= TIM_DIER_UIE;

    // Set priority 10 and enable callback.
    HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    // Enable
    TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * Returns the index of an item in array of a certain length.
 * If item not in array, then it returns -1
*/
int array_index_of(uint32_t *array, int length, uint32_t item) {

    for (int i = 0; i < length; i++) {

        // Check if array item matches search term.
        if (array[i] == item) {

            return i;
        }
    }

    // Could not be found in an array, return -1
    return -1;
}

/**
 * ISR for input capture for IR remote. 
 * Calculates the key pressed/recieved over IR using input capture.
*/
void irremote_recv() { 

    // Read CCR value (resets IF flag)
    uint32_t current = TIM2->CCR3;

    // Work out time since last event
    int pulseWidth = current - lastTime;

    // Ignore the super long pulses at the start of the sequence.
    if (pulseWidth > MAX_WIDTH) {

        // Reset variable states.
        bitPosition = 0;
        inputCode = 0;

        lastTime = current;
        return;
    }

    // Set a 1 in the current bit position if a 1 is read.
    if (pulseWidth > THRESHOLD) {

        inputCode |= (1 << bitPosition);
    }

    // Increment bit position since we have written a 1 or a 0 (implicitly)
    bitPosition++;
    
    // If we are at the end of the 32bit stream, work out the key that was pressed.
    if (bitPosition == 32) {

        bitPosition = 0;

        // Array of the different codes from 0 to 9
        uint32_t irCodes[] = {0xE916FF00, 0xF30CFF00, 0xE718FF00, 0xA15EFF00,
             0xF708FF00, 0xE31CFF00, 0xA55AFF00, 0xBD42FF00, 0xAD52FF00, 0xB54AFF00};

        // Work out the index of the input code that was read
        // Note that the index of the irCodes array is the key pressed.
        int keyPressed = array_index_of(irCodes, 10, inputCode);

        // If the code was found in the array set the keyPressedFlag and set the value.
        if (keyPressed != -1) {

            keyPressedValue = keyPressed;
            keyPressedFlag = 1;
        }
        
        // Reset the variable for the next character.
        inputCode = 0;
    }

    lastTime = current;
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

        irremote_recv();
    }   

}

/**
 * Sets the varaible "value" to contain the value of the key pressed
 * and returns 1 if there was a value that is ready to be read.
 * 
 * If no key has been pressed, this function returns a 0 and does not 
 * modify the value varaible.
*/
extern int irremote_readkey(char *value) { 

    if (keyPressedFlag) {

        // Assign the value of the keyPressedValue to the memory location of variable
        *value = keyPressedValue;

        // Reset the key pressed flag since the value has now been read.
        keyPressedFlag = 0;

        // Return 1 to signify a change/read.
        return 1;
    }

    // No key had been pressed, return 0
    return 0;
}