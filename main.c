#include "driverlib.h"
#include "board.h"

void clock_init(uint32_t MCLK_desired_khz);

int main(void)
{
    uint32_t i;

    /* Stop the Watchdog timer - Why? */
    WDT_A_hold(WDT_A_BASE);

    /* Calibrate and set up MCLK as 16MHz */
//    clock_init(16000);

    // Set P1.0/IR LED pin to output direction
    GPIO_setAsOutputPin(PORT_IR_LED, PIN_IR_LED);

    while(1)
    {
        // Toggle P1.0 output
        GPIO_toggleOutputOnPin(PORT_IR_LED, PIN_IR_LED);

        // Delay
        for(i = 100000; i > 0; i--);
    }
    return (0);
}

void clock_init(uint32_t MCLK_desired_khz)
{
    uint16_t ratio = ((uint32_t) (MCLK_desired_khz * 1000) / 32768);
    //Set DCO FLL reference = REFO
    CS_initClockSignal(CS_FLLREF, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //Set ACLK = REFO
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //Create struct variable to store proper software trim values
    CS_initFLLParam param = {0};
    //Set Ratio/Desired MCLK Frequency, initialize DCO, save trim values
    CS_initFLLCalculateTrim(MCLK_desired_khz, ratio, &param);
    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);
    //For demonstration purpose, change DCO clock freq to 16MHz
    CS_initFLLSettle(16000, 488);
    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);
    //Reload DCO trim values that were calculated earlier
    CS_initFLLLoadTrim(MCLK_desired_khz, ratio, &param);
    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);
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
    FRCTL0 = FRCTLPW | NWAITS_1;

}
