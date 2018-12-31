/*!
 * @file Adafruit_MPL3115A2.cpp
 *
 * @mainpage Adafruit MPL3115A2 alitmeter
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's MPL3115A2 driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit MPL3115A2 breakout: https://www.adafruit.com/products/1893
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * @section author Author
 *
 * Written by Kevin "KTOWN" Townsend for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "MPL3115A2.h"

#define delay(x) __delay_cycles(x * 1000)

static ctrl_reg1 _ctrl_reg1;


/**************************************************************************/
/*!
        @brief  Setups the HW (reads coefficients values, etc.)
        @param twoWire Optional TwoWire I2C object
        @return true on successful startup, false otherwise
*/
/**************************************************************************/
int MPL3115A2_begin()
{

    uint8_t whoami = I2C_read8(MPL3115A2_WHOAMI);
    if (whoami != 0xC4) return 0;

    I2C_write8(MPL3115A2_CTRL_REG1, MPL3115A2_CTRL_REG1_RST);
    delay(10);

    while(I2C_read8(MPL3115A2_CTRL_REG1) & MPL3115A2_CTRL_REG1_RST) delay(10);

    _ctrl_reg1.reg = MPL3115A2_CTRL_REG1_OS128 | MPL3115A2_CTRL_REG1_ALT;

    I2C_write8(MPL3115A2_CTRL_REG1, _ctrl_reg1.reg);

    I2C_write8(MPL3115A2_PT_DATA_CFG,
               MPL3115A2_PT_DATA_CFG_TDEFE |
               MPL3115A2_PT_DATA_CFG_PDEFE |
               MPL3115A2_PT_DATA_CFG_DREM);
    return 1;
}

/**************************************************************************/
/*!
        @brief  Gets the floating-point pressure level in kPa
        @return altitude reading as a floating point value
*/
/**************************************************************************/
float MPL3115A2_getPressure()
{
    while(I2C_read8(MPL3115A2_CTRL_REG1) & MPL3115A2_CTRL_REG1_OST) delay(10);

    _ctrl_reg1.bit.ALT = 0;
    I2C_write8(MPL3115A2_CTRL_REG1, _ctrl_reg1.reg);

    _ctrl_reg1.bit.OST = 1;
    I2C_write8(MPL3115A2_CTRL_REG1, _ctrl_reg1.reg);

    uint8_t sta = 0;
    while (! (sta & MPL3115A2_REGISTER_STATUS_PDR)) {
        sta = I2C_read8(MPL3115A2_REGISTER_STATUS);
        delay(10);
    }
    /*******************************************************************************************/
    //uint32_t pressure;
    // _i2c->beginTransmission(MPL3115A2_ADDRESS); // start transmission to device
    // _i2c->write(MPL3115A2_REGISTER_PRESSURE_MSB);
    // _i2c->endTransmission(false); // end transmission

    // _i2c->requestFrom((uint8_t)MPL3115A2_ADDRESS, (uint8_t)3);// send data n-bytes read
    // pressure = _i2c->read(); // receive DATA
    // pressure <<= 8;
    // pressure |= _i2c->read(); // receive DATA
    // pressure <<= 8;
    // pressure |= _i2c->read(); // receive DATA
    // pressure >>= 4;
    /*******************************************************************************************/
    uint8_t pressure[3];
    pressure[0] = I2C_read8(MPL3115A2_REGISTER_PRESSURE_MSB);
    pressure[1] = I2C_read8(MPL3115A2_REGISTER_PRESSURE_CSB);
    pressure[2] = I2C_read8(MPL3115A2_REGISTER_PRESSURE_LSB);

    float baro = (pressure[0] << 12) | (pressure[1] << 4) | (pressure[2] >> 4);
    baro /= 4.0;
    return baro;
}

/**************************************************************************/
/*!
        @brief  Gets the floating-point altitude value
        @return altitude reading as a floating-point value
*/
/**************************************************************************/
float MPL3115A2_getAltitude()
    {

    while(I2C_read8(MPL3115A2_CTRL_REG1) & MPL3115A2_CTRL_REG1_OST) delay(10);

    _ctrl_reg1.bit.ALT = 1;
    I2C_write8(MPL3115A2_CTRL_REG1, _ctrl_reg1.reg);

    _ctrl_reg1.bit.OST = 1;
    I2C_write8(MPL3115A2_CTRL_REG1, _ctrl_reg1.reg);

    uint8_t sta = 0;
    while (! (sta & MPL3115A2_REGISTER_STATUS_PDR)) {
        sta = I2C_read8(MPL3115A2_REGISTER_STATUS);
        delay(10);
    }

    // int32_t alt;

    // _i2c->beginTransmission(MPL3115A2_ADDRESS); // start transmission to device
    // _i2c->write(MPL3115A2_REGISTER_PRESSURE_MSB);
    // _i2c->endTransmission(false); // end transmission


    // _i2c->requestFrom((uint8_t)MPL3115A2_ADDRESS, (uint8_t)3);// send data n-bytes read
    // alt  = ((uint32_t)_i2c->read()) << 24; // receive DATA
    // alt |= ((uint32_t)_i2c->read()) << 16; // receive DATA
    // alt |= ((uint32_t)_i2c->read()) << 8; // receive DATA

    uint32_t alt[3];
    alt[0] = I2C_read8(MPL3115A2_REGISTER_PRESSURE_MSB);
    alt[1] = I2C_read8(MPL3115A2_REGISTER_PRESSURE_CSB);
    alt[2] = I2C_read8(MPL3115A2_REGISTER_PRESSURE_LSB);

    float altitude = (alt[0] << 24) | (alt[1] << 16) | (alt[2] << 8);
    altitude /= 65536.0;
    return altitude;
}

/**************************************************************************/
/*!
        @brief  Set the local sea level barometric pressure
        @param pascal the pressure to use as the baseline
*/
/**************************************************************************/
void MPL3115A2_setSeaPressure(float pascal) {
    // uint16_t bar = pascal/2;
    // _i2c->beginTransmission(MPL3115A2_ADDRESS);
    // _i2c->write((uint8_t)MPL3115A2_BAR_IN_MSB);
    // _i2c->write((uint8_t)(bar>>8));
    // _i2c->write((uint8_t)bar);
    // _i2c->endTransmission(false);
}

/**************************************************************************/
/*!
        @brief  Gets the floating-point temperature in Centigrade
        @return temperature reading in Centigrade as a floating-point value
*/
/**************************************************************************/
float MPL3115A2_getTemperature()
{

    uint8_t sta = 0;
    while (!(sta & MPL3115A2_REGISTER_STATUS_TDR)) {
        sta = I2C_read8(MPL3115A2_REGISTER_STATUS);
        delay(10);
    }

    // int16_t t;
    // _i2c->beginTransmission(MPL3115A2_ADDRESS); // start transmission to device
    // _i2c->write(MPL3115A2_REGISTER_TEMP_MSB);
    // _i2c->endTransmission(false); // end transmission

    // _i2c->requestFrom((uint8_t)MPL3115A2_ADDRESS, (uint8_t)2);// send data n-bytes read
    // t = _i2c->read(); // receive DATA
    // t <<= 8;
    // t |= _i2c->read(); // receive DATA
    // t >>= 4;

    uint8_t t_array[2];
    t_array[0] = I2C_read8(MPL3115A2_REGISTER_TEMP_MSB);
    t_array[1] = I2C_read8(MPL3115A2_REGISTER_TEMP_LSB);

    uint16_t t = (t_array[0] << 4) | (t_array[1] >> 4);

    if (t & 0x800) {
        t |= 0xF000;
    }

    float temp = t;
    temp /= 16.0;
    return temp;
}




/**************************************************************************/
/*!
        @brief  read 1 byte of data at the specified address
        @param a the address to read
        @return the read data byte
*/
/**************************************************************************/
// uint8_t I2C_read8(uint8_t a) {
//   _i2c->beginTransmission(MPL3115A2_ADDRESS); // start transmission to device
//   _i2c->write(a); // sends register address to read from
//   _i2c->endTransmission(false); // end transmission

//   _i2c->requestFrom((uint8_t)MPL3115A2_ADDRESS, (uint8_t)1);// send data n-bytes read
//   return _i2c->read(); // receive DATA
// }

// /**************************************************************************/
// !
//     @brief  write a byte of data to the specified address
//     @param a the address to write to
//     @param d the byte to write

// /**************************************************************************/
// void I2C_write8(uint8_t a, uint8_t d) {
//   _i2c->beginTransmission(MPL3115A2_ADDRESS); // start transmission to device
//   _i2c->write(a); // sends register address to write to
//   _i2c->write(d); // sends register data
//   _i2c->endTransmission(false); // end transmission
// }
