/*
 * main.c
 *
 * Created: 4/17/2026 6:39:09 PM
 * Author: Basel Mohamed Mostafa Sayed
 */ 

#define F_CPU 16000000UL

#include "ATmega32A - Drivers/std_types.h"
#include "Helper_Functions.h"
#include <util/delay.h>
#include <avr/interrupt.h>

/* MCAL */
#include "ATmega32A - Drivers/PeripheralDrivers/DIO/DIO.h"
#include "ATmega32A - Drivers/PeripheralDrivers/USART/USART.h"

/* HAL */
#include "ATmega32A - Drivers/ComponentDrivers/HC-SR04 Ultrasonic/HCSR04.h"

int main(void) {
	/* Define Variables */
	u8 receivedCommand = 0;
	u8 systemMode = MODE_MANUAL; // Start in Bluetooth mode safely
	
	// Initialize the peripherals and pins
	initializeProgram();
	LCD_clear();
	Robot_UpdateLCD(systemMode, 0, "Standby");
	
	while (1) {
		
		// 1. THE NON-BLOCKING LISTENER
		// Always check for Bluetooth commands, no matter what mode we are in!
		if (USART_receiveByteNonBlocking(&receivedCommand) == E_OK) {
			
			// Try pressing the extra buttons on your app to see which one toggles this.
			// Common extra characters are X, W, w, V, v.
			if (receivedCommand == 'X' || receivedCommand == 'W' || receivedCommand == 'w') {
				
				// Toggle the mode between 0 and 1 using XOR
				systemMode ^= 1; 
				
				// Safety brake during the transition
				Motors_off();    
				LCD_clear();
				
			} else if (systemMode == MODE_MANUAL) {
				// Only execute driving commands if we are in manual mode
				ExecuteBluetoothMotorCommand(receivedCommand);
			}
		}

		// 2. THE AUTONOMOUS ENGINE
		if (systemMode == MODE_AUTONOMOUS) {
			Autonomous_Routine();
		}
	}
}