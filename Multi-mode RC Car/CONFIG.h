/*
 * Helper_Functions.h
 *
 * Created: 4/14/2026 6:28:48 PM
 *  Author: Basel Mohamed Mostafa Sayed
 *  Description:
 *
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

/* 
*   ===================================================
*				 L298 Motors Pin outs
*   =================================================== 
*/ 

// (Left motors)
#define IN1_LEFT_MOTORS_PORT DIO_PORTC
#define IN1_LEFT_MOTORS_PIN DIO_PIN3
#define IN2_LEFT_MOTORS_PORT DIO_PORTC
#define IN2_LEFT_MOTORS_PIN DIO_PIN4

// (Right motors)
#define IN1_RIGHT_MOTORS_PORT DIO_PORTC
#define IN1_RIGHT_MOTORS_PIN DIO_PIN5
#define IN2_RIGHT_MOTORS_PORT DIO_PORTC
#define IN2_RIGHT_MOTORS_PIN DIO_PIN6

// PWM control for all motors using Timer 0 peripheral
#define PWM_MOTOR_PORT DIO_PORTB
#define PWM_MOTOR_PIN DIO_PIN3

/*
*   ===================================================
*				 HC-05 Bluetooth Module
*   ===================================================
*/

#define BTOOTH_TXD_PORT DIO_PORTD
#define BTOOTH_TXD_PIN DIO_PIN0
#define BTOOTH_RXD_PORT DIO_PORTD
#define BTOOTH_RXD_PIN DIO_PIN1
#define ENABLE_KEY_PORT DIO_PORTB
#define ENABLE_KEY_PIN DIO_PIN4
#define STATE_PORT DIO_PORTB
#define STATE_PIN DIO_PIN5

/*
*   ===================================================
*					 Other Components
*   ===================================================
*/

#endif /* CONFIG_H_ */