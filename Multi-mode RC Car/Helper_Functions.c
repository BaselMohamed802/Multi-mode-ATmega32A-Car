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
#include "ATmega32A - Drivers/ComponentDrivers/HC-SR04 Ultrasonic/HCSR04.h"
#include "ATmega32A - Drivers/ComponentDrivers/Servo SG-90/SERVO.h"

/*
* =============================================
*			Initialize/Start functions
* =============================================
*/

void initializeProgram(){
	// 1. Initialize Standard DIO Pins
	DIO_setPinDirection(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_OUTPUT);
	DIO_setPinDirection(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_OUTPUT); 
	DIO_setPinDirection(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_OUTPUT);
	DIO_setPinDirection(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_OUTPUT); 
	DIO_setPinDirection(ENABLE_KEY_PORT, ENABLE_KEY_PIN, DIO_PIN_OUTPUT);
	DIO_setPinDirection(STATE_PORT, STATE_PIN, DIO_PIN_INPUT);

	// 2. Initialize PWM Pin (OC0 / PB3) as Output
	DIO_setPinDirection(PWM_MOTOR_PORT, PWM_MOTOR_PIN, DIO_PIN_OUTPUT);

	// 3. Initialize LCD
	LCD_init();
	
	// 4. Initialize USART 
	USART_init(USART_ASYNC_MODE, 9600, PARITY_DISABLED, USART_DATA_8BIT, ONE_STOP_BIT);
	USART_enableInterrupt(USART_INT_RX_COMPLETE);
	
	// 5. Initialize Ultrasonic sensor
	HCSR04_init();
	
	// 6. Initialize Timer0 for Fast PWM (Motors)
	TIMER_PWMConfig_t pwmConfig = {
		.timerId = TIMER0,
		.channel = TIMER_CHANNEL_A,
		.mode = TIMER_MODE_FAST_PWM,
		.prescaler = TIMER_PRESCALER_64, 
		.ocMode = TIMER_OC_CLEAR,        
		.frequency = 0,
		.topValue = 0
	};
	TIMER_initPWM(&pwmConfig);
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, 0); 
	
	// 7. Initialize Servo Motor using Timer 2 (PD7)
	SERVO_init_Timer2();

	// 8. Enable Global Interrupts
	sei();
}

/*
* =============================================
*			Motor Control Functions
* =============================================
*/

void Motors_forward(u8 copy_u8Speed) {
	if (copy_u8Speed > 100) copy_u8Speed = 100; 
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, copy_u8Speed);
	
	DIO_setPinValue(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
}

void Motors_reverse(u8 copy_u8Speed) {
	if (copy_u8Speed > 100) copy_u8Speed = 100;
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, copy_u8Speed);
	
	DIO_setPinValue(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_HIGH);
}

void Motors_left(u8 copy_u8Speed) {
	if (copy_u8Speed > 100) copy_u8Speed = 100;
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, copy_u8Speed);
	
	DIO_setPinValue(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
}

void Motors_right(u8 copy_u8Speed) {
	if (copy_u8Speed > 100) copy_u8Speed = 100;
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, copy_u8Speed);
	
	DIO_setPinValue(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_HIGH);
	DIO_setPinValue(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_HIGH);
}

void Motors_off(void) {
	TIMER_setDutyCycle(TIMER0, TIMER_CHANNEL_A, 0);
	DIO_setPinValue(IN1_LEFT_MOTORS_PORT, IN1_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_LEFT_MOTORS_PORT, IN2_LEFT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN1_RIGHT_MOTORS_PORT, IN1_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
	DIO_setPinValue(IN2_RIGHT_MOTORS_PORT, IN2_RIGHT_MOTORS_PIN, DIO_PIN_LOW);
}

/*
* =============================================
*			Bluetooth Functions
* =============================================
*/

void ExecuteBluetoothMotorCommand(u8 command) {
	switch(command) {
		case 'F':
			Motors_forward(100);
			Robot_UpdateLCD(MODE_MANUAL, 0, "FWD");
			break;
		case 'B':
			Motors_reverse(100);
			Robot_UpdateLCD(MODE_MANUAL, 0, "REV");
			break;
		case 'R':
			Motors_right(80);
			Robot_UpdateLCD(MODE_MANUAL, 0, "RGT");
			break;
		case 'L':
			Motors_left(80);
			Robot_UpdateLCD(MODE_MANUAL, 0, "LFT");
			break;
		case 'S':
			Motors_off();
			Robot_UpdateLCD(MODE_MANUAL, 0, "STP");
			break;
		default:
			Motors_off(); 
			Robot_UpdateLCD(MODE_MANUAL, 0, "ERR");
			break;
	}
}

/*
* =============================================
*			LCD and Autonomous Routines
* =============================================
*/

void Robot_UpdateLCD(u8 mode, u16 distance, const char* dirStr) {
	LCD_setCursor(0, 0); 
	
	if (mode == MODE_MANUAL) {
		LCD_displayString((const u8*)"Mode: BLUETOOTH ");
		LCD_setCursor(1, 0); 
		LCD_displayString((const u8*)"Cmd: ");
		LCD_displayString((const u8*)dirStr);
		LCD_displayString((const u8*)"       "); 
	} else {
		LCD_displayString((const u8*)"Mode: AUTONOMOUS");
		LCD_setCursor(1, 0); 
		LCD_displayString((const u8*)"D:");
		LCD_displayInt(distance);
		LCD_displayString((const u8*)"cm Dir:");
		LCD_displayString((const u8*)dirStr);
		LCD_displayString((const u8*)"  ");
	}
}

// Helper function to look right and return the distance
u16 Robot_LookRight(void) {
	u16 dist = 0;
	SERVO_setAngle_Timer2(0); // Pan Right
	_delay_ms(500);           // Wait for the mechanical gears to reach position
	HCSR04_getDistance(&dist);
	return dist;
}

// Helper function to look left and return the distance
u16 Robot_LookLeft(void) {
	u16 dist = 0;
	SERVO_setAngle_Timer2(180); // Pan Left
	_delay_ms(500);             // Wait for the mechanical gears to reach position
	HCSR04_getDistance(&dist);
	return dist;
}


void Autonomous_Routine(void) {
	u16 distance = 0;
	u16 distanceRight = 0;
	u16 distanceLeft = 0;
	u8 status = HCSR04_getDistance(&distance);

	if (status == HCSR04_OK) {
		if (distance > 25) {
			// Path is clear (> 25cm), keep driving forward
			SERVO_setAngle_Timer2(90); // Ensure head is facing forward
			Motors_forward(80);
			Robot_UpdateLCD(MODE_AUTONOMOUS, distance, "FWD");
		} else {
			// === OBSTACLE DETECTED! INITIATE AVOIDANCE SEQUENCE ===
			
			// 1. Slam the brakes
			Motors_off();
			Robot_UpdateLCD(MODE_AUTONOMOUS, distance, "STP");
			_delay_ms(200); 

			// 2. Back up slightly to give the robot room to turn
			Motors_reverse(80);
			Robot_UpdateLCD(MODE_AUTONOMOUS, distance, "REV");
			_delay_ms(400);
			Motors_off();

			// 3. Scan the environment
			Robot_UpdateLCD(MODE_AUTONOMOUS, distance, "SCN"); // "SCN" = Scanning
			distanceRight = Robot_LookRight();
			distanceLeft = Robot_LookLeft();

			// 4. Center the head again
			SERVO_setAngle_Timer2(90);
			_delay_ms(300);

			// 5. Make a decision based on the scan data
			if (distanceRight >= distanceLeft && distanceRight > 20) {
				// Right side is more open
				Motors_right(80);
				Robot_UpdateLCD(MODE_AUTONOMOUS, distanceRight, "RGT");
				_delay_ms(350); // Adjust this delay based on how long it takes your robot to turn 90 degrees
			} 
			else if (distanceLeft > distanceRight && distanceLeft > 20) {
				// Left side is more open
				Motors_left(80);
				Robot_UpdateLCD(MODE_AUTONOMOUS, distanceLeft, "LFT");
				_delay_ms(350); 
			} 
			else {
				// Both sides are blocked! Initiate a U-Turn
				Motors_right(80);
				Robot_UpdateLCD(MODE_AUTONOMOUS, distanceRight, "UTN"); // "UTN" = U-Turn
				_delay_ms(700); // Double the turn delay to spin 180 degrees
			}
			
			Motors_off();
		}
	} else {
		// Sensor timeout/error
		Motors_off();
		Robot_UpdateLCD(MODE_AUTONOMOUS, 0, "ERR");
	}

	// Brief delay to prevent ultrasonic echo overlap
	_delay_ms(50);
}