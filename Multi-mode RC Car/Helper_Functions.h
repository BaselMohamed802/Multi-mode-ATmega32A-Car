/*
 * Helper_Functions.h
 *
 * Created: 4/14/2026 6:28:48 PM
 * Author: Basel Mohamed Mostafa Sayed
 * Description: Header file containing all the helper functions for the Multi-Mode RC Car.
 */ 

#ifndef HELPER_FUNCTIONS_H_
#define HELPER_FUNCTIONS_H_

#define F_CPU 16000000UL

#include "ATmega32A - Drivers/std_types.h"
#include <util/delay.h>
#include <avr/interrupt.h>

/* MCAL */
#include "ATmega32A - Drivers/PeripheralDrivers/DIO/DIO.h"
#include "ATmega32A - Drivers/PeripheralDrivers/USART/USART.h"

/*
* =============================================
*			Initialize/Start functions		
* =============================================
*/ 

// Start the program, calling any initialization of peripherals
void initializeProgram(void);


/*
* =============================================
*			Motor Control Functions
* =============================================
*/

// Function to only enable the left motors (steer the Vehicle left)
void Motors_left(u8 copy_u8Speed);

// Function to only enable the right motors (steer the vehicle right)
void Motors_right(u8 copy_u8Speed);

// This function turns on all motors connected to go forward
void Motors_forward(u8 copy_u8Speed);

// This function turns on all motors connected to go backwards
void Motors_reverse(u8 copy_u8Speed);

// Function to turn off all motors (standby)
void Motors_off(void);


/*
* =============================================
*			Bluetooth Functions
* =============================================
*/

// Parses the incoming Bluetooth character and drives the motors
void ExecuteBluetoothMotorCommand(u8 command);


/*
* =============================================
*			System Modes & LCD
* =============================================
*/
#define MODE_MANUAL     0
#define MODE_AUTONOMOUS 1

// Unified LCD updater for both Manual and Autonomous modes
void Robot_UpdateLCD(u8 mode, u16 distance, const char* dirStr);


/*
* =============================================
*			Autonomous Routines & Sensors
* =============================================
*/

// Pans the servo head to the right (0 degrees) and returns the distance in cm
u16 Robot_LookRight(void);

// Pans the servo head to the left (180 degrees) and returns the distance in cm
u16 Robot_LookLeft(void);

// The main autonomous obstacle avoidance state machine
void Autonomous_Routine(void);

#endif /* HELPER_FUNCTIONS_H_ */