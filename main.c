/**  
 *  Author:       Ahmed Sobhy
 *  Date:         10/06/2017
 *  Project Name: Fire Alarm System
 *  MCU:          PIC16F676
 *  UPWORK:       https://www.upwork.com/ab/proposals/916168207056740352
 */

// PIC16F676 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA3/MCLR pin function select (RA3/MCLR pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "stdint.h"
#include "stdbool.h"

#define RESET_TIMER1   64536

volatile uint16_t ui16TickMs = 0; 

void main(void) {
    
    uint16_t ui16SwitchTimer = 0;
    uint16_t ui16BlinkTimer = 0;
    bool bFirstEntry = true;
    
    // Initialize PORTA to input and all pull ups enabled
    CMCONbits.CM = 0x07;
    ANSEL = 0x00;
    TRISA= 0xff;
    WPUA = 0xff;
    OPTION_REGbits.nRAPU = 0;
    
    // Initialize PORTC to output
    TRISC= 0x00;
    PORTC= 0x00;
    
    // Prescalar set to 1:1 of FOSC/4 
    T1CONbits.T1CKPS = 0x00;
    
    // Internal CLK selected
    T1CONbits.TMR1CS = 0;
    
    // Init timer1 start value to interrupt every 1ms
    TMR1 = RESET_TIMER1;
    
    // enable timer1 interrupt
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    PIE1bits.TMR1IE = 1;       
    
    // turn timer1 ON
    T1CONbits.TMR1ON = 0x01;
    
    while(1){
        
        // input RA0 will be pulled low and output RC4 will be pushed High
        if( PORTAbits.RA0 == 0){
         
            PORTCbits.RC4 = 1;
        
        }
        
        // input RA4 will be pulled low and output RC4 and RC3 will be pushed High
        if( PORTAbits.RA4 == 0){
            
            PORTCbits.RC4 = 1;
            PORTCbits.RC3 = 1;
            
        }
        
        //  input RA2 will be pulled low and output RC4 and RC2 will be pushed High
        if( PORTAbits.RA2 == 0){
            
            PORTCbits.RC4 = 1;
            PORTCbits.RC2 = 1;
            
        }
        
        
        // input RA1 will be pulled low and output RC4 and RC1 will be pushed High
        if( PORTAbits.RA1 == 0){
            
            PORTCbits.RC4 = 1;
            PORTCbits.RC1 = 1;
            
        }
         
        // If switch connected to RA5 is pressed
        if( PORTAbits.RA5 == 0){
            
            // start counting seconds
            if( bFirstEntry ){
                // set timer to current time
                ui16SwitchTimer = ui16TickMs;
                bFirstEntry = false;
            }
            
            // If we Press S1 for 02 seconds , input RA5 will be pulled low and output RC4 will be pulled Low
            if( (ui16TickMs - ui16SwitchTimer) >= 2000 &&  (ui16TickMs - ui16SwitchTimer) < 3000 ){
                
                PORTCbits.RC4 = 0;
            
            }
        
            // If we Press S1 for 05 seconds , input RA5 will be pulled low and output RC1, RC2, RC3 will be pulled Low
            if( (ui16TickMs - ui16SwitchTimer) >= 5000 ){
                
                PORTCbits.RC1 = 0;
                PORTCbits.RC2 = 0;
                PORTCbits.RC3 = 0;
                bFirstEntry = true;

            }
                
        }
        
        // Output RC5 will start blinking at 1Hz to show CPU in operation
        if( ui16TickMs - ui16BlinkTimer >= 1000 ){
            
            // reset Blink timer
            ui16BlinkTimer = ui16TickMs;
            // toggle pin RC5
            PORTCbits.RC5 = ~PORTCbits.RC5;
            
        }
        
        // reset watchdog timer
        CLRWDT();
         
    }
       
}

void interrupt ISR(void){
    
    if(PIR1bits.T1IF == 1){
        
        // turn off counter
        T1CONbits.TMR1ON = 0;
        // clear the interrupt flag
        PIR1bits.T1IF = 0;

        
        // increment millisecond counter
        ui16TickMs++;

        
        // RESET_TIMER1 
        TMR1 = RESET_TIMER1;
        // turn on timer1
        T1CONbits.TMR1ON = 1;
        
    }
    
}
