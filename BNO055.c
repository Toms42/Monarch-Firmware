/***************************************************************************
    This is a library for the BNO055 orientation sensor

    Designed specifically to work with the Adafruit BNO055 Breakout.

    Pick one up today in the adafruit shop!
    ------> https://www.adafruit.com/product/2472

    These sensors use I2C to communicate, 2 pins are required to interface.

    Adafruit invests time and resources providing this open source code,
    please support Adafruit andopen-source hardware by purchasing products
    from Adafruit!

    Written by KTOWN for Adafruit Industries.

    MIT license, all text above must be included in any redistribution
 ***************************************************************************/

#include "BNO055.h"

#define delay(x) __delay_cycles((unsigned long) x * 1000)

static bno055_opmode_t _mode;

/**************************************************************************/
/*!
        @brief  Sets up the HW
*/
/**************************************************************************/
int BNO055_begin(bno055_opmode_t mode)
{
    /* Enable I2C */

    // BNO055 clock stretches for 500us or more!

    /* Make sure we have the right device */
    uint8_t id = I2C_read8(BNO055_CHIP_ID_ADDR);
    if(id != BNO055_ID)
    {
        delay(1000); // hold on for boot
        id = I2C_read8(BNO055_CHIP_ID_ADDR);
        if(id != BNO055_ID) {
            return 0;  // still not? ok bail
        }
    }

    /* Switch to config mode (just in case since this is the default) */
    BNO055_setMode(OPERATION_MODE_CONFIG);

    /* Reset */
    I2C_write8(BNO055_SYS_TRIGGER_ADDR, 0x20);
    while (I2C_read8(BNO055_CHIP_ID_ADDR) != BNO055_ID)
    {
        delay(10);
    }
    delay(50);

    /* Set to normal power mode */
    I2C_write8(BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
    delay(10);

    I2C_write8(BNO055_PAGE_ID_ADDR, 0);

    /* Set the output units */
    /*
    uint8_t unitsel = (0 << 7) | // Orientation = Android
                                        (0 << 4) | // Temperature = Celsius
                                        (0 << 2) | // Euler = Degrees
                                        (1 << 1) | // Gyro = Rads
                                        (0 << 0);  // Accelerometer = m/s^2
    I2C_write8(BNO055_UNIT_SEL_ADDR, unitsel);
    */

    /* Configure axis mapping (see section 3.4) */
    /*
    I2C_write8(BNO055_AXIS_MAP_CONFIG_ADDR, REMAP_CONFIG_P2); // P0-P7, Default is P1
    delay(10);
    I2C_write8(BNO055_AXIS_MAP_SIGN_ADDR, REMAP_SIGN_P2); // P0-P7, Default is P1
    delay(10);
    */

    I2C_write8(BNO055_SYS_TRIGGER_ADDR, 0x0);
    delay(10);
    /* Set the requested operating mode (see section 3.3) */
    BNO055_setMode(mode);
    delay(20);

    return 1;
}

/**************************************************************************/
/*!
        @brief  Puts the chip in the specified operating mode
*/
/**************************************************************************/
void BNO055_setMode(bno055_opmode_t mode)
{
    _mode = mode;
    I2C_write8(BNO055_OPR_MODE_ADDR, _mode);
    delay(30);
}

/**************************************************************************/
/*!
        @brief  Changes the chip's axis remap
*/
/**************************************************************************/
void BNO055_setAxisRemap(bno055_axis_remap_config_t remapcode)
{
    bno055_opmode_t modeback = _mode;

    BNO055_setMode(OPERATION_MODE_CONFIG);
    delay(25);
    I2C_write8(BNO055_AXIS_MAP_CONFIG_ADDR, remapcode);
    delay(10);
    /* Set the requested operating mode (see section 3.3) */
    BNO055_setMode(modeback);
    delay(20);
}

/**************************************************************************/
/*!
        @brief  Changes the chip's axis signs
*/
/**************************************************************************/
void BNO055_setAxisSign(bno055_axis_remap_sign_t remapsign)
{
    bno055_opmode_t modeback = _mode;

    BNO055_setMode(OPERATION_MODE_CONFIG);
    delay(25);
    I2C_write8(BNO055_AXIS_MAP_SIGN_ADDR, remapsign);
    delay(10);
    /* Set the requested operating mode (see section 3.3) */
    BNO055_setMode(modeback);
    delay(20);
}


/**************************************************************************/
/*!
        @brief  Gets the latest system status info
*/
/**************************************************************************/
void BNO055_getSystemStatus(uint8_t *system_status, uint8_t *self_test_result, uint8_t *system_error)
{
    I2C_write8(BNO055_PAGE_ID_ADDR, 0);

    /* System Status (see section 4.3.58)
         ---------------------------------
         0 = Idle
         1 = System Error
         2 = Initializing Peripherals
         3 = System Iniitalization
         4 = Executing Self-Test
         5 = Sensor fusio algorithm running
         6 = System running without fusion algorithms */

    if (system_status != 0)
        *system_status    = I2C_read8(BNO055_SYS_STAT_ADDR);

    /* Self Test Results (see section)
         --------------------------------
         1 = test passed, 0 = test failed

         Bit 0 = Accelerometer self test
         Bit 1 = Magnetometer self test
         Bit 2 = Gyroscope self test
         Bit 3 = MCU self test

         0x0F = all good! */

    if (self_test_result != 0)
        *self_test_result = I2C_read8(BNO055_SELFTEST_RESULT_ADDR);

    /* System Error (see section 4.3.59)
         ---------------------------------
         0 = No error
         1 = Peripheral initialization error
         2 = System initialization error
         3 = Self test result failed
         4 = Register map value out of range
         5 = Register map address out of range
         6 = Register map write error
         7 = BNO low power mode not available for selected operat ion mode
         8 = Accelerometer power mode not available
         9 = Fusion algorithm configuration error
         A = Sensor configuration error */

    if (system_error != 0)
        *system_error     = I2C_read8(BNO055_SYS_ERR_ADDR);

    delay(200);
}

/**************************************************************************/
/*!
        @brief  Gets the chip revision numbers
*/
/**************************************************************************/
void BNO055_getRevInfo(bno055_rev_info_t* info)
{
    uint8_t a, b;

    /* Check the accelerometer revision */
    info->accel_rev = I2C_read8(BNO055_ACCEL_REV_ID_ADDR);

    /* Check the magnetometer revision */
    info->mag_rev   = I2C_read8(BNO055_MAG_REV_ID_ADDR);

    /* Check the gyroscope revision */
    info->gyro_rev  = I2C_read8(BNO055_GYRO_REV_ID_ADDR);

    /* Check the SW revision */
    info->bl_rev    = I2C_read8(BNO055_BL_REV_ID_ADDR);

    a = I2C_read8(BNO055_SW_REV_ID_LSB_ADDR);
    b = I2C_read8(BNO055_SW_REV_ID_MSB_ADDR);
    info->sw_rev = (((uint16_t)b) << 8) | ((uint16_t)a);
}

/**************************************************************************/
/*!
        @brief  Gets current calibration state.  Each value should be a uint8_t
                        pointer and it will be set to 0 if not calibrated and 3 if
                        fully calibrated.
*/
/**************************************************************************/
void BNO055_getCalibration(uint8_t* sys, uint8_t* gyro, uint8_t* accel, uint8_t* mag) {
    uint8_t calData = I2C_read8(BNO055_CALIB_STAT_ADDR);
    if (sys)  *sys = (calData >> 6) & 0x03;
    if (gyro) *gyro = (calData >> 4) & 0x03;
    if (accel) *accel = (calData >> 2) & 0x03;
    if (mag) *mag = calData & 0x03;

}

/**************************************************************************/
/*!
        @brief  Gets the temperature in degrees celsius
*/
/**************************************************************************/
int8_t BNO055_getTemp(void)
{
    int8_t temp = (int8_t)(I2C_read8(BNO055_TEMP_ADDR));
    return temp;
}

/**************************************************************************/
/*!
        @brief  Gets a vector reading from the specified source
*/
/**************************************************************************/
void BNO055_getVector(vector_type_t vector_type, vector *result)
{
    uint8_t buffer[6];
    int16_t x, y, z;

    /* Read vector data (6 bytes) */
    I2C_readLen((bno055_reg_t)vector_type, buffer, 6);

    x = ((int16_t)buffer[0]) | (((int16_t)buffer[1]) << 8);
    y = ((int16_t)buffer[2]) | (((int16_t)buffer[3]) << 8);
    z = ((int16_t)buffer[4]) | (((int16_t)buffer[5]) << 8);

    /* Convert the value to an appropriate range (section 3.6.4) */
    /* and assign the value to the Vector type */
    switch(vector_type)
    {
        case VECTOR_MAGNETOMETER:
            /* 1uT = 16 LSB */
            result->x = ((float)x)/16.0;
            result->y = ((float)y)/16.0;
            result->z = ((float)z)/16.0;
            break;
        case VECTOR_GYROSCOPE:
            /* 1dps = 16 LSB */
            result->x = ((float)x)/16.0;
            result->y = ((float)y)/16.0;
            result->z = ((float)z)/16.0;
            break;
        case VECTOR_EULER:
            /* 1 degree = 16 LSB */
            result->x = ((float)x)/16.0;
            result->y = ((float)y)/16.0;
            result->z = ((float)z)/16.0;
            break;
        case VECTOR_ACCELEROMETER:
        case VECTOR_LINEARACCEL:
        case VECTOR_GRAVITY:
            /* 1m/s^2 = 100 LSB */
            result->x = ((float)x)/100.0;
            result->y = ((float)y)/100.0;
            result->z = ((float)z)/100.0;
            break;
    }
}

/**************************************************************************/
/*!
        @brief  Gets a quaternion reading from the specified source
*/
/**************************************************************************/
void BNO055_getQuaternion(quaternion *result)
{
    uint8_t buffer[8];
    int16_t x, y, z, w;

    /* Read quat data (8 bytes) */
    I2C_readLen(BNO055_QUATERNION_DATA_W_LSB_ADDR, buffer, 8);
    w = (((uint16_t)buffer[1]) << 8) | ((uint16_t)buffer[0]);
    x = (((uint16_t)buffer[3]) << 8) | ((uint16_t)buffer[2]);
    y = (((uint16_t)buffer[5]) << 8) | ((uint16_t)buffer[4]);
    z = (((uint16_t)buffer[7]) << 8) | ((uint16_t)buffer[6]);

    /* Assign to Quaternion */
    /* See http://ae-bst.resource.bosch.com/media/products/dokumente/bno055/BST_BNO055_DS000_12~1.pdf
         3.6.5.5 Orientation (Quaternion)  */
    const float scale = (1.0 / (1<<14));
    // imu::Quaternion quat(scale * w, scale * x, scale * y, scale * z);

    result->x = scale * x;
    result->y = scale * y;
    result->z = scale * z;
    result->w = scale * w;
}




/**************************************************************************/
/*!
@brief  Reads the sensor's offset registers into a byte array
*/
/**************************************************************************/
//bool getSensorOffsets(uint8_t* calibData)
//{
//    if (isFullyCalibrated())
//    {
//        bno055_opmode_t lastMode = _mode;
//        setMode(OPERATION_MODE_CONFIG);
//
//        readLen(ACCEL_OFFSET_X_LSB_ADDR, calibData, NUM_BNO055_OFFSET_REGISTERS);
//
//        setMode(lastMode);
//        return true;
//    }
//    return false;
//}


/**************************************************************************/
/*!
        @brief  Checks of all cal status values are set to 3 (fully calibrated)
*/
/**************************************************************************/
int BNO055_isFullyCalibrated(void)
{
    uint8_t system, gyro, accel, mag;
    BNO055_getCalibration(&system, &gyro, &accel, &mag);
    if (system < 3 || gyro < 3 || accel < 3 || mag < 3)
            return 0;
    return 1;
}


