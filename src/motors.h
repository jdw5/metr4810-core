/*
 * motors.h
 *
 *  Created on: May 5, 2023
 *      Author: josh2
 */

#ifndef SRC_MOTORS_H_
#define SRC_MOTORS_H_

extern void motors(int direction);
#define STOP() motors(0)
#define FORWARD() motors(4)


#endif /* SRC_MOTORS_H_ */
