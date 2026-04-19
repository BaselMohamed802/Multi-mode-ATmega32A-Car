/*
 * Helper_Functions.h
 *
 * Created: 4/14/2026 6:28:48 PM
 *  Author: Basel Mohamed Mostafa Sayed
 *  Description:
 *
 */ 

#define F_CPU 16000000UL

#include "ATmega32A - Drivers/std_types.h"
#include <util/delay.h>
#include <avr/interrupt.h>

/* MCAL */
#include "ATmega32A - Drivers/PeripheralDrivers/DIO/DIO.h"
#include "ATmega32A - Drivers/PeripheralDrivers/USART/USART.h"

#ifndef HELPER_FUNCTIONS_H_
#define HELPER_FUNCTIONS_H_

/*
*  =============================================
*			Initialize/Start functions		
*  =============================================
*/ 

// Start the program, calling any initialization of peripherals
void initializeProgram();

/*
*  =============================================
*			Motor Control Functions
*  =============================================
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
*  =============================================
*			Bluetooth Functions
*  =============================================
*/

void ExecuteBluetoothMotorCommand(u8 command);

#endif /* HELPER_FUNCTIONS_H_ */