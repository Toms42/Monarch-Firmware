#include <msp430.h>
#include "driverlib.h"
#include "board.h"
#include "uart.h"
#include "uart_printf.h"
#include "servos.h"
#include "parse.h"

#define scalar (10)
#define absolute_min (0)
#define absolute_max (180)

void clock_init_16MHz();
void timer_init_milli();

volatile bool UART_data = false;
volatile uint64_t millie = 0;

volatile float a_curr = 90;
volatile float a_next = 90;


float amplitude = 0;
float dihedral = 90;
float roll = 0;
float ang_v = 0;
float glide_thresh = 0;

float A0 = 90;


float a_max = 90;
float a_min = 90;

int dir = 1;

servos_t servos;
float get_a_min(float A0, float amplitude);
float get_a_max(float A0, float amplitude);

int main(void)
{
    uint64_t i;
    uint64_t j;
    tag_t tag_struct;
    char *test = "TAG_MEMES:475,-354, -456, 1348, 6969;";
    tag_struct = decode_tag(test);

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
    //timer_init_milli();

    /* Setup servos - NOTE THE SERVO TIMING WILL BE COMPLETELY OFF IF clock_init_16MHz is not called */

    servos.CCR_left      = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    servos.CCR_right     = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    servos.period        = 20000;
    servos.max_period    = 2500;
    servos.min_period    = 500;
    servos.range_degrees = 180;

    servos_timers_init(&servos);
    servos_enable(&servos, 90, 90);

    // Enable global interrupts
    __enable_interrupt();

    uint32_t angle = 90;
    uint32_t angle_span = 180;

    char buf[MAX_PACKET_SIZE];
    int buf_ind = 0;

    while(1)
    {
        //uart_printstring("Current angle: ");
        //uart_printintln(millie);
        if(UART_data) //check if there is data on the UART
        {
            //uart_printstringln("UART CAUGHT");
            UART_data = false;
            buf[buf_ind] = uart_getc();
            if(buf[buf_ind] == (char)';') //end of message
            {
                buf[buf_ind + 1] = (char)'\0'; //add null terminator
                tag_struct = decode_tag(buf);

                //populate values from struct
                amplitude = (float)tag_struct.tag_values[0]/scalar;
                dihedral = (float)tag_struct.tag_values[1]/scalar + 90;
                roll = (float)tag_struct.tag_values[2]/scalar;
                ang_v = (float)tag_struct.tag_values[3]/scalar;
                glide_thresh = (float)tag_struct.tag_values[4]/scalar;

                //assign A0 points for each wing
                A0 = dihedral;

                //check if we are within threshold for gliding
                if(ang_v < glide_thresh)
                {
                  amplitude = 0;
                  ang_v = glide_thresh;
                }
                //assign max and min values for each wing
                a_max = get_a_max(A0, amplitude);
                a_min = get_a_min(A0, amplitude);

                //amax left, right are assigned, rest in ISR

;               send_to_UART(tag_struct);
                memset(buf, 0x00, buf_ind);
                buf_ind = 0;
            }
            else
            {
                buf_ind++; //increment buffer counter
            }

        }

        // Change servo angles from 0 deg to 45 deg to 90 deg over again
        //servos_set(&servos, (float) 180 - angle  , (float) angle);

        if(angle == angle_span) angle = 0;
        else angle = angle_span;

        // Delay

        /*for(i = 0x20000; i > 0; i--)
        {
            for(j = 0xFFFF; j > 0; j--);
        }*/
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
        millie++;
        counter = 0;


    }
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); //blink LED

    //add a step to the current value
    a_next = a_curr + ang_v/50*dir;

    //check if exceeding bounds
    if(a_next > a_max){
        a_next = a_max;
        dir = -1*dir;
    }
    else if(a_next < a_min){
        a_next = a_min;
        dir = -1*dir;
    }

    //update current value to next value
    a_curr = a_next;

    //send to servos

    servos_set(&servos, (float) 180 - a_next - roll , (float) a_next + roll);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

}

/*
#pragma vector=TIMER2_A2_VECTOR
__interrupt void TIMER2_A2_ISR(void)
{
    millie++;
    uart_printintln(millie);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

}
*/

#pragma vector=USCI_A1_VECTOR
__interrupt void EUSCI_A1_ISR(void)
{
    //uint8_t RXData;




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


/*
//initialize timer 1 A3
void timer_init_milli()
{
    Timer_B_initUpModeParam param = {0};
    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_16;
    param.timerPeriod = 1000;
    param.timerInterruptEnable_TBIE = TIMER_B_TAIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_ENABLE;
    param.timerClear = TIMER_B_DO_CLEAR;
    param.startTimer = false;
    Timer_B_initUpMode(TIMER_B1_BASE, &param);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);
    //    Timer_A_enableInterrupt(TIMER_A1_BASE);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

}
*/


/*returns A0 for left wing*/
float get_A0_left(float dihedral, float roll)
{
    return dihedral;
}

/*returns A0 for right wing*/
float get_A0_right(float dihedral, float roll)
{
    return dihedral;
}

float get_a_min(float A0, float amplitude)
{
    float tmp_a_min = A0 - amplitude/2;
    if(tmp_a_min < absolute_min){
        return absolute_min;
    }
    return tmp_a_min;

}

float get_a_max(float A0, float amplitude)
{
    float tmp_a_max = A0 + amplitude/2;
    if(tmp_a_max > absolute_max){
        return absolute_max;
    }
    return tmp_a_max;

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
