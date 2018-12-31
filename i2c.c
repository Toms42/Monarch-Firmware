#include <driverlib.h>
#include "i2c.h"

/* I2C Master Configuration Parameter */
//EUSCI_B_I2C_initMasterParam i2cParam =
//{
//        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
//        16000000,                               // SMCLK = 16MHz
//        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 400khz
//        0,                                      // No byte counter threshold
//        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
//};


void I2C_init_GPIO(uint8_t port, uint16_t pin_SCL, uint16_t pin_SDA, uint8_t function)
{
    /* Select I2C function for I2C_SCL & I2C_SDA */
    GPIO_setAsPeripheralModuleFunctionOutputPin(port, pin_SCL, function);
    GPIO_setAsPeripheralModuleFunctionOutputPin(port, pin_SDA, function);
}


/*******************************************************************************
 * @brief  Configures I2C
 * @param  none
 * @return none
 ******************************************************************************/

void I2C_init(EUSCI_B_I2C_initMasterParam *i2cParam)
{
    /* Initialize USCI_B0 and I2C Master to communicate with slave devices*/
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, i2cParam);
    /* Disable I2C module to make changes */
    EUSCI_B_I2C_disable(EUSCI_B0_BASE);
    /* Enable I2C Module to start operations */
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);
}


/*******************************************************************************
 * @brief  Writes data to the sensor
 * @param  pointer  Address of register you want to modify
 * @param  writeByte Data to be written to the specified register
 * @return none
 ******************************************************************************/

void I2C_write8(unsigned char pointer, unsigned char writeByte)
{
    /* Set master to transmit mode PL */
    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    /* Clear any existing interrupt flag PL */
    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    /* Initiate start and send first character */
    EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, pointer);
    /* Finish and receive */
    EUSCI_B_I2C_masterSendMultiByteFinish(EUSCI_B0_BASE, writeByte);

}


/*******************************************************************************
 * @brief  Reads data from the sensor
 * @param  pointer Address of register to read from
 * @return Register contents
 ******************************************************************************/

char I2C_read8(unsigned char pointer)
{
    volatile int val = 0;
    volatile int valScratch = 0;

    /* Set master to transmit mode PL */
    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    /* Clear any existing interrupt flag PL */
    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    /* Initiate start and send first character */
    EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, pointer);

    return EUSCI_B_I2C_masterReceiveSingleByte(EUSCI_B0_BASE);
}


/*******************************************************************************
 * @brief  Reads data from the sensor
 * @param  pointer Address of register to read from
 * @return Register contents
 ******************************************************************************/

int I2C_readLen(unsigned char pointer, unsigned char *result, unsigned int len)
{
    /* Set master to transmit mode PL */
    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    /* Clear any existing interrupt flag PL */
    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    /* Initiate start and send first character */
    EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, pointer);

    /*
     * Generate Start condition and set it to receive mode.
     * This sends out the slave address and continues to read
     * until you issue a STOP
     */
    EUSCI_B_I2C_masterReceiveStart(EUSCI_B0_BASE);
    /* Read len - 1 number of bytes. */
    int i = 0;
    for (i = 0; i < len - 1; i++)
    {
        /* Wait for RX buffer to fill */
        while(!(EUSCI_B_I2C_getInterruptStatus(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0)));

        /* Read from I2C RX register */
        result[i] = EUSCI_B_I2C_masterReceiveMultiByteNext(EUSCI_B0_BASE);
    }
    /* Receive the last (i = len - 1)th byte then send STOP condition */
    result[len - 1] = EUSCI_B_I2C_masterReceiveMultiByteFinish(EUSCI_B0_BASE);

    return 1;
}




void I2C_set_slave(unsigned short slaveAdr)
{
    /* Specify slave address for I2C */
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE, slaveAdr);
    /* Enable and clear the interrupt flag */
    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
        EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_RECEIVE_INTERRUPT0);

}
