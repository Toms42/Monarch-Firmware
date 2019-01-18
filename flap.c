#include "flap.h"
#include <math.h>

static flap_t param_left = {0};
static flap_t param_right = {0};

static float flap_calc_sine(servos_t *servos, float counter, flap_t *param)
{
    // Note counter should be incremented by 1/update_freq
    float adj_sinewave = sinf(6.283184f * param->frequency * counter);
    float flap_degrees = (servos->range_degrees/2 + param->offset)
                         + param->amplitude * adj_sinewave;

    return flap_degrees;
}

void flap_set_left(flap_t *param)
{
    param_left = *param;
}

void flap_set_right(flap_t *param)
{
    param_right = *param;
}

void flap_update_flap(servos_t *servos)
{
    static uint32_t counter = 0;

    // Note the negative sign in front of counter for angle_right
    // This is to flip the sinusoid so that the angle_left and angle_right
    // are mirrored flaps
    float angle_left = flap_calc_sine(servos, (float) counter/50.0f, &param_left);
    float angle_right = flap_calc_sine(servos, (float) counter/-50.0f, &param_right);

    servos_set(servos, angle_left, 180.0f - angle_right);

    counter += 1;
    if (counter == 50) counter = 0;
}
