/*
 * board.h
 *
 *  Created on: Dec 22, 2018
 *      Author: Jae Choi
 */

#ifndef BOARD_H_
#define BOARD_H_


#define FUNCTION_PWM_LED        GPIO_SECONDARY_MODULE_FUNCTION
#define PORT_IR_LED             GPIO_PORT_P1
#define PORT_PWM_LED            GPIO_PORT_P1
#define PIN_IR_LED              GPIO_PIN0
#define PIN_PWM_LED             GPIO_PIN1

#define FUNCTION_PWM_SERVO      GPIO_SECONDARY_MODULE_FUNCTION
#define PORT_SERVO              GPIO_PORT_P1
#define PIN_RIGHT_SERVO         GPIO_PIN4
#define PIN_LEFT_SERVO          GPIO_PIN5

#define FUNCTION_I2C            GPIO_PRIMARY_MODULE_FUNCTION
#define PORT_I2C_SCL            GPIO_PORT_P1
#define PORT_I2C_SDA            GPIO_PORT_P1
#define PIN_I2C_SCL             GPIO_PIN3
#define PIN_I2C_SDA             GPIO_PIN2

#define GPIO_PORT_VBATT         GPIO_PORT_P1
#define GPIO_PIN_VBATT          GPIO_PIN7


#endif /* BOARD_H_ */
