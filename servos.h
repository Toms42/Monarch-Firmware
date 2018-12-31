/*
 * servos.h
 *
 *  Created on: Dec 22, 2018
 *      Author: Jae Choi
 */

#ifndef SERVOS_H_
#define SERVOS_H_

#include "driverlib.h"

typedef struct
{
    uint16_t CCR_left;
    uint16_t CCR_right;
    uint16_t period;
    uint16_t max_period;
    uint16_t min_period;
    uint16_t range_degrees;
} servos_t;

void servos_init_GPIO(uint8_t ports, uint16_t pin_left, uint16_t pin_right, uint8_t function);

void servos_timers_init(servos_t *servos);

void servos_enable(servos_t *servos, uint32_t left_degrees, uint32_t right_degrees);
void servos_disable();
void servos_set(servos_t *servos, uint32_t left_degrees, uint32_t right_degrees);

#endif /* SERVOS_H_ */
