//****************************************************************************
//
// HAL_I2C.h - Prototypes of hardware abstraction layer for I2C between
//             MSP432P401R and OPT3001
//
//****************************************************************************

#ifndef __HAL_I2C_H_
#define __HAL_I2C_H_

#include <driverlib.h>


void I2C_init_GPIO(uint8_t port, uint16_t pin_SCL, uint16_t pin_SDA, uint8_t function);
void I2C_init(EUSCI_B_I2C_initMasterParam *i2cParam);
void I2C_set_slave(unsigned short slaveAdr);

void I2C_write8(unsigned char pointer, unsigned char writeByte);

char I2C_read8(unsigned char pointer);
int I2C_readLen(unsigned char pointer, unsigned char *result, unsigned int len);

#endif /* __HAL_I2C_H_ */
