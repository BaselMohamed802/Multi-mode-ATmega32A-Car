/*
 * main.c
 *
 * Created: 4/17/2026 6:39:09 PM
 * Author: Basel Mohamed Mostafa Sayed
 * Description: Master Multi-Mode FSM Control Script
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
#include "ATmega32A - Drivers/ComponentDrivers/Servo SG-90/SERVO.h"

int main(void) {
    /* Define Variables */
    u8 receivedCommand = 0;
    u8 systemMode = MODE_MANUAL; // Safely default to Manual upon boot
    
    // 1. Wake up and configure all peripherals, timers, pins, and interrupts
    initializeProgram();
    
    // 2. Clear the LCD and set the initial standby state
    LCD_clear();
    Robot_UpdateLCD(systemMode, 0, "Standby");
    
    // Optional: Send a welcome menu to the Bluetooth Terminal
    USART_sendString((const u8*)"\r\n=== AMIT RC CAR ONLINE ===\r\n");
    USART_sendString((const u8*)"Send '1' -> Manual Mode\r\n");
    USART_sendString((const u8*)"Send '2' -> Autonomous Mode\r\n");
    
    while (1) {
        
        // =========================================================
        // THE NON-BLOCKING BLUETOOTH LISTENER (Always Active)
        // =========================================================
        if (USART_receiveByteNonBlocking(&receivedCommand) == E_OK) {
            
            // --- FSM MODE SWITCHING LOGIC ---
            if (receivedCommand == '1' && systemMode != MODE_MANUAL) {
                systemMode = MODE_MANUAL;
                Motors_off(); // Slam the brakes during transition
                SERVO_setAngle_Timer2(90); // Center the head
                LCD_clear();
                Robot_UpdateLCD(systemMode, 0, "Standby");
                USART_sendString((const u8*)"[SYS] Switched to MANUAL Mode\r\n");
            }
            else if (receivedCommand == '2' && systemMode != MODE_AUTONOMOUS) {
                systemMode = MODE_AUTONOMOUS;
                Motors_off(); // Slam the brakes during transition
                LCD_clear();
                Robot_UpdateLCD(systemMode, 0, "Starting...");
                USART_sendString((const u8*)"[SYS] Switched to AUTONOMOUS Mode\r\n");
            }
            // --- MANUAL DRIVING LOGIC ---
            else if (systemMode == MODE_MANUAL) {
                // Pass driving characters (F, B, R, L, S) to the motor parser
                ExecuteBluetoothMotorCommand(receivedCommand);
            }
        }

        // =========================================================
        // THE AUTONOMOUS ENGINE
        // =========================================================
        if (systemMode == MODE_AUTONOMOUS) {
            // Hand control over to the ultrasonic sensor and servo FSM
            Autonomous_Routine();
        }
    }
    
    return 0;
}