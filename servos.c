#include "servos.h"

void servos_init_GPIO(uint8_t ports, uint16_t pin_left, uint16_t pin_right, uint8_t function)
{
    /* Set pin as functional/PWM outputs */
    GPIO_setAsPeripheralModuleFunctionOutputPin(ports, pin_left, function);
    GPIO_setAsPeripheralModuleFunctionOutputPin(ports, pin_right, function);

    /* Disable the GPIO power-on default high-impedance mode
     * to activate previously configured port settings */
    // PMM_unlockLPM5();
}

void servos_timers_init(servos_t *servos)
{
    /* Set up timer A - NOTE SMCLK MUST BE 16MHz */
    Timer_A_initUpModeParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_16;
    param.timerPeriod = servos->period;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    param.timerClear = TIMER_A_DO_CLEAR;
    param.startTimer = false;
    Timer_A_initUpMode(TIMER_A1_BASE, &param);

    /* Set up the PWM output for left PWM servo */
    Timer_A_initCompareModeParam initComp1Param = {0};
    initComp1Param.compareRegister = servos->CCR_left;
    initComp1Param.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp1Param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    initComp1Param.compareValue = servos->min_period;
    Timer_A_initCompareMode(TIMER_A1_BASE, &initComp1Param);

    /* Set up the PWM output for right PWM sevo */
    Timer_A_initCompareModeParam initComp2Param = {0};
    initComp2Param.compareRegister = servos->CCR_right;
    initComp2Param.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp2Param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    initComp2Param.compareValue = servos->min_period;
    Timer_A_initCompareMode(TIMER_A1_BASE, &initComp2Param);
}

void servos_set(servos_t *servos, uint32_t left_degrees, uint32_t right_degrees)
{
    /* Convert range into periods for left and right servos */
    /* Note range is uint32_t so the multiplication with left/right degrees does not overflow */
    uint32_t range        = servos->max_period - servos->min_period;
    uint16_t left_period  = (range * left_degrees) / servos->range_degrees + servos->min_period;
    uint16_t right_period = (range * right_degrees) / servos->range_degrees + servos->min_period;

    Timer_A_setCompareValue(TIMER_A1_BASE, servos->CCR_left, left_period);
    Timer_A_setCompareValue(TIMER_A1_BASE, servos->CCR_right, right_period);
}

void servos_enable(servos_t *servos, uint32_t left_degrees, uint32_t right_degrees)
{
    servos_set(servos, left_degrees, right_degrees);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
}

void servos_disable()
{
    Timer_A_stop(TIMER_A1_BASE);
}
