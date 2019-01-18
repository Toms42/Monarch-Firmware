/*
 * flap.h
 *
 *  Created on: Dec 22, 2018
 *      Author: Jae Choi
 */

#ifndef FLAP_H_
#define FLAP_H_

#include "servos.h"

typedef struct flap
{
    float amplitude;
    float frequency;
    float offset;
} flap_t;


void flap_set_left(flap_t *param);
void flap_set_right(flap_t *param);

void flap_update_flap(servos_t *servos);

#endif /* FLAP_H_ */
