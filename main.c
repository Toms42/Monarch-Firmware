#include "driverlib.h"
#include "board.h"
#include "uart.h"
#include "uart_printf.h"
#include "servos.h"

void clock_init_16MHz();

int main(void)
{
    uint32_t i;

    /* Stop the Watchdog timer - Why? */
    WDT_A_hold(WDT_A_BASE);

    /* Calibrate and set up MCLK as 16MHz */
    clock_init_16MHz(16000);

    // Set P1.0/IR LED pin to output direction
    GPIO_setAsOutputPin(PORT_IR_LED, PIN_IR_LED);

    uart_init_GPIO(PORT_UART, PIN_UART_TX, PIN_UART_RX, FUNCTION_UART);

    /* Setup servo GPIO: NOTE RIGHT SERVO IS TA1.2 AND LEFT IS TA1.1 */
    servos_init_GPIO(PORT_SERVO, PIN_LEFT_SERVO, PIN_RIGHT_SERVO, FUNCTION_PWM_SERVO);

    /* Disable the GPIO power-on default high-impedance mode
     * to activate previously configured port settings */
    PMM_unlockLPM5();

    uart_init();

    /* Setup servos - NOTE THE SERVO TIMING WILL BE COMPLETELY OFF IF clock_init_16MHz is not called */
    servos_t servos;
    servos.CCR_left      = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    servos.CCR_right     = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    servos.period        = 20000;
    servos.max_period    = 2000;
    servos.min_period    = 1000;
    servos.range_degrees = 90;

    servos_timers_init(&servos);
    servos_enable(&servos, 90, 90);

    // Enable global interrupts
    __enable_interrupt();

    uint32_t angle = 0;

    while(1)
    {
        // Toggle P1.0 output
        GPIO_toggleOutputOnPin(PORT_IR_LED, PIN_IR_LED);

        uart_printstring("Current angle: ");
        uart_printintln((int) angle);

        // Change servo angles from 0 deg to 45 deg to 90 deg over again
        servos_set(&servos, angle, angle);
        angle = (angle + 45) % 135;

        // Delay
        for(i = 100000; i > 0; i--);
    }
    return (0);
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
    static uint32_t counter = 0;

    counter += 1;
    if (counter == 50)
    {
        counter = 0;
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);


}

#pragma vector=USCI_A0_VECTOR
__interrupt void EUSCI_A0_ISR(void)
{
    uint8_t RXData;

    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
            RXData = uart_getc();
            uart_putc(RXData);

            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

            break;
       case USCI_UART_UCTXIFG: break;
       case USCI_UART_UCSTTIFG: break;
       case USCI_UART_UCTXCPTIFG: break;
    }
}


void clock_init_16MHz()
{
//    uint16_t ratio = ((uint32_t) (MCLK_desired_khz * 1000) / 32768);
    //Set DCO FLL reference = REFO
    CS_initClockSignal(CS_FLLREF, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //Set ACLK = REFO
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //Create struct variable to store proper software trim values
//    CS_initFLLParam param = {0};
    //Set Ratio/Desired MCLK Frequency, initialize DCO, save trim values
//    CS_initFLLCalculateTrim(MCLK_desired_khz, ratio, &param);
    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);
    //For demonstration purpose, change DCO clock freq to 16MHz
    CS_initFLLSettle(16000, 488);
    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);
    //Reload DCO trim values that were calculated earlier
//    CS_initFLLLoadTrim(MCLK_desired_khz, ratio, &param);
    //Clear all OSC fault flag
//    CS_clearAllOscFlagsWithTimeout(1000);
    //Enable oscillator fault interrupt
    // SFR_enableInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
    // Enable global interrupt
    // __bis_SR_register(GIE);
    //Verify if the Clock settings are as expected
    uint32_t clockValue = CS_getSMCLK();
             clockValue = CS_getMCLK();
             clockValue = CS_getACLK();

    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system. (PAGE 16)
//    FRCTL0 = FRCTLPW | NWAITS_1;
    FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_1);

}
