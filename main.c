#include "driverlib.h"
#include "board.h"
#include "uart.h"
#include "uart_printf.h"
#include "servos.h"
#include "parse.h"
#include "flap.h"

void clock_init_16MHz();

static bool UART_data = false;
static servos_t servos;

int main(void)
{
    uint64_t i;
    uint64_t j;
    tag_t test_struct;
    char *test = "TAG_MEMES:475,-354, -456, 1348, 6969;";
    test_struct = decode_tag(test);

    /* Stop the Watchdog timer - Why? */
    WDT_A_hold(WDT_A_BASE);

    /* Calibrate and set up MCLK as 16MHz */
    clock_init_16MHz();

    // Set P1.0/IR LED pin to output direction
    GPIO_setAsOutputPin(PORT_IR_LED, PIN_IR_LED);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    uart_init_GPIO(PORT_UART, PIN_UART_TX, PIN_UART_RX, FUNCTION_UART);

    /* Setup servo GPIO: NOTE RIGHT SERVO IS TA1.2 AND LEFT IS TA1.1 */
    servos_init_GPIO(PORT_SERVO, PIN_LEFT_SERVO, PIN_RIGHT_SERVO, FUNCTION_PWM_SERVO);

    /* Disable the GPIO power-on default high-impedance mode
     * to activate previously configured port settings */
    PMM_unlockLPM5();

    uart_init();

    /* Setup servos - NOTE THE SERVO TIMING WILL BE COMPLETELY OFF IF clock_init_16MHz is not called */
    servos.CCR_left      = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    servos.CCR_right     = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    servos.period        = 20000;
    servos.max_period    = 2500;
    servos.min_period    = 500;
    servos.range_degrees = 180;

    servos_timers_init(&servos);
    servos_enable(&servos, 90, 90);

    // Set flap parameters
    flap_t param;
    param.amplitude = 10.0;
    param.frequency = 1.0;
    param.offset = 0.0;

    flap_set_left(&param);
    flap_set_right(&param);

    // Enable global interrupts
    __enable_interrupt();

    uint32_t angle = 90;
    uint32_t angle_span = 90;
    char c;
    char buf[MAX_PACKET_SIZE];
    int buf_ind = 0;

    while(1)
    {

        //uart_printstring("Current angle: ");
        //uart_printintln((int) angle);
        if(UART_data)
        {
            //uart_printstringln("UART CAUGHT");
            UART_data = false;
            buf[buf_ind] = uart_getc();
            if(buf[buf_ind] == (char)';')
            {
                buf[buf_ind + 1] = (char)'\0'; //add null terminator
                test_struct = decode_tag(buf);
                send_to_UART(test_struct);
                memset(buf, 0x00, buf_ind);
                buf_ind = 0;
            }
            else
            {
                buf_ind++; //increment buffer counter
            }

        }

        // Change servo angles from 0 deg to 45 deg to 90 deg over again
//        servos_set(&servos, (float) 180 - angle , (float) angle);
//        if(angle == angle_span) angle = 0;
//        else angle = angle_span;

        // Delay

        for(i = 0x21479; i > 0; i--)
        {
            for(j = 0xFFFF; j > 0; j--);
        }
    }
    return (0);
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{

    flap_update_flap(&servos);

    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);


}

#pragma vector=USCI_A1_VECTOR
__interrupt void EUSCI_A1_ISR(void)
{
    uint8_t RXData;
    bool EOM = false;

    int i = 0;

    switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:

            UART_data = true;
            //RXData = uart_getc();
            //uart_putc(RXData);
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

            break;
       case USCI_UART_UCTXIFG: break;
       case USCI_UART_UCSTTIFG: break;
       case USCI_UART_UCTXCPTIFG: break;
    }
}


void clock_init_16MHz()
{
    //Set DCO FLL reference = REFO
    CS_initClockSignal(CS_FLLREF, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //Set ACLK = REFO
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //Create struct variable to store proper software trim values
    CS_initFLLParam param = {0};
    //Set Ratio/Desired MCLK Frequency, initialize DCO, save trim values
    CS_initFLLCalculateTrim(16000, 488, &param);
    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);
    //For demonstration purpose, change DCO clock freq to 16MHz
    CS_initFLLSettle(16000, 488);
    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);
    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system. (PAGE 16)
    FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_1);

}
