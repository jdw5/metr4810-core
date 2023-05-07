/*
 * ir_recv.h
 *
 *  Created on: May 5, 2023
 *      Author: Joshua Wallace
 */

#ifndef SRC_IR_RECV_H_
#define SRC_IR_RECV_H_

#include "stdint.h"

#define IR_STATE_IDLE      2
#define IR_STATE_MARK      3
#define IR_STATE_SPACE     4
#define IR_STATE_STOP      5
#define IR_STATE_OVERFLOW  6

#define IR_RAWBUF  101

typedef struct
{
	// The fields are ordered to reduce memory over caused by struct-padding
	uint8_t       rcvstate;     // State Machine state
	uint16_t      recvpin;      // Pin connected to IR data from detector
	GPIO_TypeDef* recvpinport;  // Port of pin connected to IR data from detector
	uint16_t      blinkpin;
	GPIO_TypeDef* blinkpinport;
	uint8_t       blinkflag;          // true -> enable blinking of pin on IR processing
	uint8_t       rawlen;             // counter of entries in rawbuf
	unsigned int  timer;              // State timer, counts 50uS ticks.
	unsigned int  rawbuf[IR_RAWBUF];  // raw data
	uint8_t       overflow;           // Raw buffer overflow occurred
} irparams_t;

extern void ir_recv();
extern void ir_init();
extern uint8_t ir_get(char *value);

extern void IR_ISR (void);
extern void IRrecv_DataReadyCallback(unsigned long data);

#define IR_TOLERANCE       25
#define IR_LTOL            (1.0 - (IR_TOLERANCE/100.))
#define IR_UTOL            (1.0 + (IR_TOLERANCE/100.))

#define IR_MARK   0
#define IR_SPACE  1

#endif /* SRC_IR_RECV_H_ */
