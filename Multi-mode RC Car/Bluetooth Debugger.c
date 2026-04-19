/*
 * Bluetooth_Debugger.c
 *
 * Created: 4/17/2026 6:39:09 PM
 *  Author: Basel Mohamed Mostafa Sayed
 *  
 */ 

#define F_CPU 16000000UL

#include "ATmega32A - Drivers/std_types.h"
#include <util/delay.h>
#include <avr/interrupt.h>

/* MCAL */
#include "ATmega32A - Drivers/PeripheralDrivers/DIO/DIO.h"
#include "ATmega32A - Drivers/PeripheralDrivers/USART/USART.h"

/* HAL */
#include "../../../Microcontrollers Drivers/ATmega32A-Drivers/ATmega32A - Drivers/ComponentDrivers/LCD 16x2/LCD.h"

int main(void) {
	u8 receivedData = 0;
	
	// 1. Initialize the LCD
	// (Ensure LCD_CONFIG.h matches your Proteus wiring)
	LCD_init();
	LCD_displayString((const u8*)"BT Debugger:");
	
	// 2. Initialize USART for HC-05
	// Default HC-05 baud rate is usually 9600
	USART_init(USART_ASYNC_MODE, 9600, PARITY_DISABLED, USART_DATA_8BIT, ONE_STOP_BIT);
	
	// 3. Enable USART Receive Interrupt (required for your ring buffer)
	USART_enableInterrupt(USART_INT_RX_COMPLETE);
	
	// 4. Enable Global Interrupts
	sei();
	
	// Set up the second row of the LCD
	LCD_setCursor(1, 0);
	LCD_displayString((const u8*)"Cmd: ");

	while(1) {
		// 5. Check if a new character has arrived in the ring buffer
		if (USART_receiveByteNonBlocking(&receivedData) == E_OK) {
			
			// Clear the previous character and hex value to prevent visual overlap
			LCD_setCursor(1, 5);
			LCD_displayString((const u8*)"           ");
			
			// Print the literal ASCII character
			LCD_setCursor(1, 5);
			LCD_displayChar(receivedData);
			
			// Print the ASCII HEX value
			LCD_displayString((const u8*)" (0x");
			
			// Handle single-digit hex formatting (e.g., ensuring 0x0A instead of 0xA)
			if(receivedData < 0x10) {
				LCD_displayChar('0');
			}
			LCD_displayNumber(receivedData, 16);
			LCD_displayChar(')');
		}
		
		// Small delay to prevent LCD flickering if the app spams continuous commands
		_delay_ms(10);
	}
	
	return 0;
}