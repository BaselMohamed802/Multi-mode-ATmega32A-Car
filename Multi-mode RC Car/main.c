/*
 * main.c
 *
 * Created: [Current Date]
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
    u8 systemMode = MODE_MANUAL;
    
    // 1. Wake up and configure all peripherals, timers, pins, and interrupts
    initializeProgram();
    
    // 2. Clear the LCD and set the initial standby state
    LCD_clear();
    Robot_UpdateLCD(systemMode, 0, "Standby");
    
    while (1) {
        
        // ==================================================================
        // BLUETOOTH MOOD NON-BLOCKING (KEY '1' to turn on Bluetooth mode)
        // ==================================================================
        if (USART_receiveByteNonBlocking(&receivedCommand) == E_OK) {
            
            // --- Switching Logic ---
            if (receivedCommand == '1' && systemMode != MODE_MANUAL) {
                systemMode = MODE_MANUAL;
                Motors_off(); // Turn off all motors during transition
                SERVO_setAngle_Timer2(90); // Center the servo head using Timer 2
                LCD_clear();
                Robot_UpdateLCD(systemMode, 0, "Standby");
                USART_sendString((const u8*)"[SYS] Switched to MANUAL Mode\r\n");
            }
            else if (receivedCommand == '2' && systemMode != MODE_AUTONOMOUS) {
                systemMode = MODE_AUTONOMOUS;
                Motors_off(); // Turn off all motors during transition
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
        // THE AUTONOMOUS MODE (KEY '2' Using Terminal to activate)
        // =========================================================
        if (systemMode == MODE_AUTONOMOUS) {
            // Hand control over to the ultrasonic sensor and servo FSM
            Autonomous_Routine();
        }
    }
    
    return 0;
}