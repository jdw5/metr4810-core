/*
 * ir_recv.h
 *
 *  Created on: May 5, 2023
 *      Author: Joshua Wallace
 */

#ifndef SRC_IR_RECV_H_
#define SRC_IR_RECV_H_

#include "stdint.h"

extern void ir_recv();
extern void ir_init();
extern uint8_t ir_get(char *value);

#endif /* SRC_IR_RECV_H_ */
