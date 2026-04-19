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

int main(void) {
	/* Define Variables */
	u8 receivedCommand = 0;
	
	// 1. WAKE UP THE PERIPHERALS!
	initializeProgram();
	
	// Optional: Send a welcome message to prove USART is alive
	USART_sendString((const u8*)"System Ready! Waiting for command...\r\n");
	
	while (1) {
		if (USART_receiveByteNonBlocking(&receivedCommand) == E_OK) {
			
			// Echo the character back to the terminal so you can see it
			USART_sendString((const u8*)"Received: ");
			USART_sendByte(receivedCommand);
			USART_sendString((const u8*)"\r\n");
			
			// Execute the motor command
			ExecuteBluetoothMotorCommand(receivedCommand);
		}
	}
}