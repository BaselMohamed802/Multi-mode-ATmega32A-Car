/*
 * Helper_Functions.c
 *
 * Created: 4/14/2026 6:29:18 PM
 * Author: Basel Mohamed Mostafa Sayed
 * Description:
 *			This header file contains all the functions used in the project.
 */ 

#define F_CPU 16000000UL

#include "ATmega32A - Drivers/std_types.h"
#include "Helper_Functions.h"
#include "CONFIG.h"
#include <util/delay.h>
#include <avr/interrupt.h>

/* MCAL */
#include "ATmega32A - Drivers/PeripheralDrivers/DIO/DIO.h"
#include "ATmega32A - Drivers/PeripheralDrivers/USART/USART.h"
#include "ATmega32A - Drivers/PeripheralDrivers/TIMER/TIMER.h"

/* HAL */
#include "ATmega32A - Drivers/ComponentDrivers/LCD 16x2/LCD.h"

/*
* =============================================
*			Initialize/Start functions
* =============================================
*/

// Start the program, calling any initialization of peripherals
void initializeProgram(){
	// 1. Initialize Standard DIO Pins
	DIO_setPinDirection(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_OUTPUT);
	DIO_setPinDirection(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_OUTPUT); // Added missing IN2 init
	DIO_setPinDirection(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_OUTPUT);
	DIO_setPinDirection(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_OUTPUT); // Added missing IN2 init
	DIO_setPinDirection(ENABLE_KEY_PORT, ENABLE_KEY_PIN, DIO_PIN_OUTPUT);
	DIO_setPinDirection(STATE_PORT, STATE_PIN, DIO_PIN_INPUT);

	// 2. Initialize PWM Pin (OC0 / PB3) as Output
	DIO_setPinDirection(PWM_MOTOR_PORT, PWM_MOTOR_PIN, DIO_PIN_OUTPUT);

	// 3. Initialize LCD
	LCD_init();
	
	// 4. Initialize USART - Default HC-05 baud rate is usually 9600
	USART_init(USART_ASYNC_MODE, 9600, PARITY_DISABLED, USART_DATA_8BIT, ONE_STOP_BIT);
	USART_enableInterrupt(USART_INT_RX_COMPLETE);
	
	// 5. Initialize Timer0 for Fast PWM (Non-Inverting)
	TIMER_PWMConfig_t pwmConfig = {
		.timerId = TIMER0,
		.channel = TIMER_CHANNEL_A,
		.mode = TIMER_MODE_FAST_PWM,
		.prescaler = TIMER_PRESCALER_64, // Good frequency for DC Motors
		.ocMode = TIMER_OC_CLEAR,        // Non-inverting setting
		.frequency = 0,
		.topValue = 0
	};
	TIMER_initPWM(&pwmConfig);
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, 0); // Start with motors stopped

	// 6. Enable Global Interrupts
	sei();
}

/*
* =============================================
*			Motor Control Functions
* =============================================
*/

void Motors_forward(u8 copy_u8Speed) {
	if (copy_u8Speed > 100) copy_u8Speed = 100; // Clamp speed safely
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, copy_u8Speed);
	
	// Both sides go Forward
	DIO_setPinValue(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
}

void Motors_reverse(u8 copy_u8Speed) {
	if (copy_u8Speed > 100) copy_u8Speed = 100;
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, copy_u8Speed);
	
	// Both sides go Reverse
	DIO_setPinValue(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_HIGH);
}

void Motors_left(u8 copy_u8Speed) {
	if (copy_u8Speed > 100) copy_u8Speed = 100;
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, copy_u8Speed);
	
	// Pivot Turn: Left side Reverse, Right side Forward
	DIO_setPinValue(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
}

void Motors_right(u8 copy_u8Speed) {
	if (copy_u8Speed > 100) copy_u8Speed = 100;
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, copy_u8Speed);
	
	// Pivot Turn: Left side Forward, Right side Reverse
	DIO_setPinValue(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_HIGH);
}

void Motors_off(void) {
	// Drop speed to 0
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, 0);
	
	// Safely cut all direction pins
	DIO_setPinValue(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
}

/*
*  =============================================
*			Bluetooth Functions
*  =============================================
*/

void ExecuteBluetoothMotorCommand(u8 command) {
	switch(command) {
		case 'F':
		Motors_forward(100);
		break;
		case 'B':
		Motors_reverse(100);
		break;
		case 'R':
		Motors_right(80);
		break;
		case 'L':
		Motors_left(80);
		break;
		case 'S':
		Motors_off();
		break;
		default:
		Motors_off(); // In case for any corrupted/unknown characters
		break;
	}
}