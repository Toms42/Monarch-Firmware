#include "uart.h"

void uart_init_GPIO(uint8_t port, uint16_t pin_tx, uint16_t pin_rx, uint8_t function)
{
    /* Configure UART pins */
    GPIO_setAsPeripheralModuleFunctionInputPin(port, pin_tx, function);
    GPIO_setAsPeripheralModuleFunctionInputPin(port, pin_rx, function);
}

void uart_init()
{
    /* Configure UART
     * SMCLK = 1MHz, Baudrate = 115200
     * UCBRx = 8, UCBRFx = 0, UCBRSx = 0xD6, UCOS16 = 0
     */
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 8;
    param.firstModReg = 0;
    param.secondModReg = 0xD6;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;

    if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &param)) {
        return;
    }

    /* Enable USCI_A0 RX interrupt */
    EUSCI_A_UART_enable(EUSCI_A0_BASE);
    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);

    /* Enable global interrupts */
    // __enable_interrupt();

}

void uart_putc(char c)
{
    EUSCI_A_UART_transmitData(EUSCI_A0_BASE, c);
}

void uart_puts(char *s, uint32_t len)
{
    int i = 0;
    for (i = 0; i < len; i++)
    {
        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, s[i]);
    }
}

char uart_getc()
{
    return EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
}
