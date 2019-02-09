/*
 this is proper working code alloking with rc circuit
 * issue: not known
 */

#include <stdio.h>
#include <stdlib.h>
#include <pic16f1526.h>
#include <string.h>


// PIC16F1526 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection (HS Oscillator, High-speed crystal/resonator connected between OSC1 and OSC2 pins)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bit (VCAP pin function disabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#define _XTAL_FREQ 16000000
#include <xc.h>
#define RELAY1 RF1 // 1ST SW
#define FAN RF0 //2ND sw
#define RELAY2 RE5//DIMMER1
#define RELAY3 RA3//DIMMER2
#define RELAY4 RA2//DIMMER3


#define SW1 RF7//DIMMER1
#define SW2 RF5//DIMMER2
#define SW3 RF3//DIMMER3
#define SW4 RF2
#define SW5 RA5

// fan response switch
unsigned char dimmer1, dimmer2, dimmer3;
int i=0, pk=0,ak=0,tk=0;
//int FAN_SPEED=0,FAN_SPEED1=0;
int FAN_SPEED1_10s=0,FAN_SPEED1_1s=0;
int FAN_SPEED2_10s=0,FAN_SPEED2_1s=0;
int FAN_SPEED3_10s=0,FAN_SPEED3_1s=0;

int TX_FLAG=0, FAN_FLAG1=0, FAN_FLAG2=0, FAN_FLAG3=0;
int man=1;
unsigned char child_LOCK[5]="00000";
unsigned char st[26]="T000000000000000000000";
unsigned char name[36]="a";
unsigned char copy[36]="$";
//char parents[32]="p";
int COPY_FLAG=0;
unsigned int M1;unsigned int M2;unsigned int M3;unsigned int M4;unsigned int M5;

#define ONN 1
#define OFF 0

void interrupt  isr()
{     
    
           // ************************************* UART *********************************************** //
        if(RCIF == 1)
        {
            if(RC1STAbits.OERR) // If over run error, then reset the receiver
            {                
                RC1STAbits.CREN = 0;
                RC1STAbits.CREN = 1;
                
                while(PIR1bits.TXIF==0); 
                TX1REG='F';
                while(PIR1bits.TXIF==0); 
                
            }
            name[i]=RC1REG;            
            if(name[0]=='%')
            {
                i++;                
                if(i>15)
                {
                    i=0;
                    TX_FLAG = 1;
                    RCIF=0;
                }               
            }
            else
            {                
                RC1STAbits.CREN = 0;
                RC1STAbits.CREN = 1;
                
                i=0;
                while(PIR1bits.TXIF==0); 
                TX1REG='F';
                while(PIR1bits.TXIF==0); 
                TX1REG='R';
                while(PIR1bits.TXIF==0); 
                TX1REG='R';
            }                
        } 
   //*******************************DIMMER 11111 *************************************
    
    if(PIE1bits.TMR2IE==1 && PIR1bits.TMR2IF==1)
    {           
        PIR1bits.TMR2IF=0;
        RELAY2=ONN;
        T2CONbits.TMR2ON=0;
    } 
    
     if(PIE1bits.TMR1IE == 1 && PIR1bits.TMR1IF==1)
    {
        PIR1bits.TMR1IF=0;
        T1CONbits.TMR1ON = 0;        
        RELAY2=OFF;
        PR2=0x9F;
        T2CONbits.TMR2ON=1;
               
    }

    //*******************************DIMMER 22222 *************************************
    
    if(PIE3bits.TMR4IE==1 && PIR3bits.TMR4IF==1)
    {           
        PIR3bits.TMR4IF=0;
        RELAY3=ONN;
        T4CONbits.TMR4ON=0;

    }
    
    if(PIE3bits.TMR3IE == 1 && PIR3bits.TMR3IF==1)
    {
        PIR3bits.TMR3IF=0;
        
        RELAY3=OFF;
        PR4=0x9F;

        T4CONbits.TMR4ON=1;
        T3CONbits.TMR3ON = 0;        
    }

    //*******************************DIMMER 33333 *************************************    
    
    if(PIE3bits.TMR6IE == 1 && PIR3bits.TMR6IF == 1)
    {           
        PIR3bits.TMR6IF=0;
        RELAY4=ONN;
        T6CONbits.TMR6ON=0;
    } 
    
    if(PIE3bits.TMR5IE == 1 && PIR3bits.TMR5IF==1)
    {
         PIR3bits.TMR5IF=0;        
        RELAY4=OFF;
        PR6=0x9F;
        T6CONbits.TMR6ON=1;
        T5CONbits.TMR5ON=0;        
    }
    
    
    if(PIR1bits.CCP1IF==1 || PIR3bits.CCP3IF == 1 || PIR4bits.CCP9IF==1)
    {              
        if(PIR1bits.CCP1IF == 1)
        {
            PIR1bits.CCP1IF = 0;
        // ************************************* FAN SPEED *********************************************** //    
        if(FAN_FLAG1==1 && pk==1)
        {
            switch(FAN_SPEED1_10s)
                {
                    case '0':           // 8.5                    /**/
                            switch(FAN_SPEED1_1s)
                             {
                            case '0':           // 8.5
                                     TMR1H=0x7B;        TMR1L=0x30;     T1CONbits.TMR1ON = 1;       break;
                             case '1':           // 8.4
                                     TMR1H=0x7C;        TMR1L=0xC0;     T1CONbits.TMR1ON = 1;       break;
                             case '2':           // 8.35
                                     TMR1H=0x7D;        TMR1L=0x88;     T1CONbits.TMR1ON = 1;       break;
                             case '3':           // 8.25
                                     TMR1H=0x7F;        TMR1L=0x18;     T1CONbits.TMR1ON = 1;       break;
                             case '4':          // 8.15
                                     TMR1H=0x80;        TMR1L=0xA8;     T1CONbits.TMR1ON = 1;       break;
                             case '5':               // 8.1
                                     TMR1H=0x81;        TMR1L=0x70;     T1CONbits.TMR1ON = 1;       break;
                             case '6':               // 8.0    
                                     TMR1H=0x83;        TMR1L=0x00;     T1CONbits.TMR1ON = 1;       break;
                             case '7':            //7.95
                                     TMR1H=0x83;        TMR1L=0xC8;     T1CONbits.TMR1ON = 1;       break;
                             case '8':           //7.9
                                     TMR1H=0x87;        TMR1L=0xB0;     T1CONbits.TMR1ON = 1;       break;
                             case '9':           // 7.85
                                     TMR1H=0x85;        TMR1L=0x58;     T1CONbits.TMR1ON = 1;       break;
                             default:
                                 break;
                         }                    
                        break;
                case '1':           // 7.8-7.3

                            switch(FAN_SPEED1_1s)
                                 {
                                 case '0':           // 7.8
                                         TMR1H=0x86;    TMR1L=0x20;    T1CONbits.TMR1ON = 1;        break;
                                 case '1':           // 7.75
                                         TMR1H=0x86;    TMR1L=0xE8;    T1CONbits.TMR1ON = 1;        break;
                                 case '2':           // 7.7
                                         TMR1H=0x87;    TMR1L=0xB0;    T1CONbits.TMR1ON = 1;        break;
                                 case '3':           // 7.65
                                         TMR1H=0x88;    TMR1L=0x78;    T1CONbits.TMR1ON = 1;        break;
                                 case '4':            // 7.6
                                         TMR1H=0x89;    TMR1L=0x40;    T1CONbits.TMR1ON = 1;        break;
                                 case '5':               // 7.55
                                         TMR1H=0x8A;    TMR1L=0x08;    T1CONbits.TMR1ON = 1;        break;
                                 case '6':               // 7.5    
                                         TMR1H=0x8A;    TMR1L=0xD0;    T1CONbits.TMR1ON = 1;        break;
                                 case '7':            //7.45
                                         TMR1H=0x8B;    TMR1L=0x98;    T1CONbits.TMR1ON = 1;        break;
                                 case '8':           //7.4
                                         TMR1H=0x8C;    TMR1L=0x60;    T1CONbits.TMR1ON = 1;        break;
                                 case '9':           // 7.35
                                         TMR1H=0x8D;    TMR1L=0x28;    T1CONbits.TMR1ON = 1;        break;
                                 default:
                                     break;
                                }
                        break;
                case '2':           // 7.3-
/**/
                        switch(FAN_SPEED1_1s)
                             {
                             case '0':           // 7.3-6.85
                                     TMR1H=0x8D;     TMR1L=0xF0;       T1CONbits.TMR1ON = 1;        break;
                             case '1':           // 7.25
                                     TMR1H=0x8E;     TMR1L=0xB8;       T1CONbits.TMR1ON = 1;        break;
                             case '2':           // 7.20
                                     TMR1H=0x8F;     TMR1L=0x80;       T1CONbits.TMR1ON = 1;        break;
                             case '3':           // 7.15
                                     TMR1H=0x90;     TMR1L=0x48;       T1CONbits.TMR1ON = 1;        break;
                             case '4'://TX1REG='n';      // 7.1
                                     TMR1H=0x91;     TMR1L=0x10;       T1CONbits.TMR1ON = 1;        break;
                             case '5':               // 7.05
                                     TMR1H=0x91;     TMR1L=0xD8;       T1CONbits.TMR1ON = 1;        break;
                             case '6':               // 7.0    
                                     TMR1H=0x92;     TMR1L=0xA0;       T1CONbits.TMR1ON = 1;        break;
                             case '7':            //6.95
                                     TMR1H=0x93;     TMR1L=0x68;       T1CONbits.TMR1ON = 1;        break;
                             case '8':           //6.9
                                     TMR1H=0x94;     TMR1L=0x30;       T1CONbits.TMR1ON = 1;        break;
                             case '9':           // 6.85
                                     TMR1H=0x94;     TMR1L=0xF8;       T1CONbits.TMR1ON = 1;        break;
                             default:
                                 break;
                         }                    
                        break;
                case '3':           // 6.8-5.9                
/**/
                        switch(FAN_SPEED1_1s)
                             {
                             case '0':           // 6.8
                                     TMR1H=0x95;     TMR1L=0xC0;      T1CONbits.TMR1ON = 1;         break;
                             case '1':           // 6.7
                                     TMR1H=0x97;     TMR1L=0x50;      T1CONbits.TMR1ON = 1;         break;
                             case '2':           // 6.6
                                     TMR1H=0x98;     TMR1L=0xE0;      T1CONbits.TMR1ON = 1;         break;
                             case '3':           // 6.5
                                     TMR1H=0x9A;     TMR1L=0x70;      T1CONbits.TMR1ON = 1;         break;
                             case '4'://TX1REG='n';      // 6.4
                                     TMR1H=0x9C;     TMR1L=0x00;      T1CONbits.TMR1ON = 1;         break;
                             case '5':               // 6.3
                                     TMR1H=0x9D;     TMR1L=0x90;      T1CONbits.TMR1ON = 1;         break;
                             case '6':               // 6.2   
                                     TMR1H=0x9F;     TMR1L=0x20;      T1CONbits.TMR1ON = 1;         break;
                             case '7':            //6.1
                                     TMR1H=0xA0;     TMR1L=0xB0;      T1CONbits.TMR1ON = 1;         break;
                             case '8':           //6.0
                                     TMR1H=0xA2;     TMR1L=0x40;      T1CONbits.TMR1ON = 1;         break;
                             case '9':           // 5.9
                                     TMR1H=0xA3;     TMR1L=0xD0;      T1CONbits.TMR1ON = 1;         break;
                             default:
                                     break;
                            }
                        break;
                case '4'://TX1REG='n';      // 5.8-4.9                    
/**/
                        switch(FAN_SPEED1_1s)
                             {
                             case '0':           // 5.8
                                     TMR1H=0xA5;     TMR1L=0x60;      T1CONbits.TMR1ON = 1;         break;
                             case '1':           // 5.7
                                     TMR1H=0xA6;     TMR1L=0xF0;      T1CONbits.TMR1ON = 1;         break;
                             case '2':           // 5.6
                                     TMR1H=0xA8;     TMR1L=0x80;      T1CONbits.TMR1ON = 1;         break;
                             case '3':           // 5.5
                                     TMR1H=0xAA;     TMR1L=0x10;      T1CONbits.TMR1ON = 1;         break;
                             case '4':           // 5.4
                                     TMR1H=0xAB;     TMR1L=0xA0;      T1CONbits.TMR1ON = 1;         break;
                             case '5':               // 5.3
                                     TMR1H=0xAD;     TMR1L=0x30;      T1CONbits.TMR1ON = 1;         break;
                             case '6':               // 5.2    
                                     TMR1H=0xAE;     TMR1L=0xC0;      T1CONbits.TMR1ON = 1;         break;
                             case '7':              // 5.1
                                     TMR1H=0xB0;     TMR1L=0x50;      T1CONbits.TMR1ON = 1;         break;
                             case '8':              // 5.0
                                     TMR1H=0xB1;     TMR1L=0xE0;      T1CONbits.TMR1ON = 1;         break;
                             case '9':              // 4.9
                                     TMR1H=0xB3;     TMR1L=0x70;      T1CONbits.TMR1ON = 1;          break;
                             default:
                                     break;
                         }
                        break;
                case '5':               // 4.8-3.9
/**/
                        switch(FAN_SPEED1_1s)
                             {
                             case '0':           // 4.8
                                     TMR1H=0xB5;     TMR1L=0x00;      T1CONbits.TMR1ON = 1;          break;
                             case '1':           // 4.7
                                     TMR1H=0xB6;     TMR1L=0x90;      T1CONbits.TMR1ON = 1;          break;
                             case '2':           // 4.6
                                     TMR1H=0xB8;     TMR1L=0x20;      T1CONbits.TMR1ON = 1;          break;
                             case '3':           // 4.5
                                     TMR1H=0xB9;     TMR1L=0xB0;      T1CONbits.TMR1ON = 1;          break;
                             case '4'://TX1REG='n';      // 4.4
                                     TMR1H=0xBB;     TMR1L=0x40;      T1CONbits.TMR1ON = 1;          break;
                             case '5':               // 4.3
                                     TMR1H=0xBC;     TMR1L=0xD0;      T1CONbits.TMR1ON = 1;          break;
                             case '6':               // 4.2   
                                     TMR1H=0xBE;     TMR1L=0x60;      T1CONbits.TMR1ON = 1;          break;
                             case '7':            //4.1
                                     TMR1H=0xBF;     TMR1L=0xF0;      T1CONbits.TMR1ON = 1;          break;
                             case '8':           //4.0
                                     TMR1H=0xC1;     TMR1L=0x80;      T1CONbits.TMR1ON = 1;          break;
                             case '9':           // 3.9
                                     TMR1H=0xC3;     TMR1L=0x10;      T1CONbits.TMR1ON = 1;          break;
                             default:
                                 break;
                            }                    
                        break;
                case '6':               // 3.8-2.9 
/**/
                        switch(FAN_SPEED1_1s)
                             {
                             case '0':           // 3.8
                                     TMR1H=0xC4;     TMR1L=0xA0;      T1CONbits.TMR1ON = 1;          break;
                             case '1':           // 3.7
                                     TMR1H=0xC6;     TMR1L=0x30;      T1CONbits.TMR1ON = 1;           break;
                             case '2':           // 3.6
                                     TMR1H=0xC7;     TMR1L=0xC0;      T1CONbits.TMR1ON = 1;          break;
                             case '3':           // 3.5
                                     TMR1H=0xC9;     TMR1L=0x50;      T1CONbits.TMR1ON = 1;          break;
                             case '4'://TX1REG='n';      // 3.4
                                     TMR1H=0xCA;     TMR1L=0xE0;      T1CONbits.TMR1ON = 1;          break;
                             case '5':               // 3.3
                                     TMR1H=0xCC;     TMR1L=0x70;      T1CONbits.TMR1ON = 1;          break;
                             case '6':               // 3.2   
                                     TMR1H=0xCE;     TMR1L=0x00;      T1CONbits.TMR1ON = 1;          break;
                             case '7':            //3.1
                                     TMR1H=0xCF;     TMR1L=0x90;      T1CONbits.TMR1ON = 1;          break;
                             case '8':           // 3.0
                                     TMR1H=0xD1;     TMR1L=0x20;      T1CONbits.TMR1ON = 1;          break;
                             case '9':           // 2.9
                                     TMR1H=0xD2;     TMR1L=0xB0;      T1CONbits.TMR1ON = 1;          break;
                             default:
                                     break;
                            }                    
                        break;
                case '7':            //2.8-1.9
/**/
                        switch(FAN_SPEED1_1s)
                             {
                             case '0':           // 2.8
                                     TMR1H=0xD4;     TMR1L=0x40;      T1CONbits.TMR1ON = 1;          break;
                             case '1':           // 2.7
                                     TMR1H=0xD5;     TMR1L=0xD0;      T1CONbits.TMR1ON = 1;          break;
                             case '2':           // 2.6
                                     TMR1H=0xD7;     TMR1L=0x60;      T1CONbits.TMR1ON = 1;          break;
                             case '3':           // 2.5
                                     TMR1H=0xD8;     TMR1L=0xF0;      T1CONbits.TMR1ON = 1;          break;
                             case '4'://TX1REG='n';      // 2.4
                                     TMR1H=0xDA;     TMR1L=0x80;      T1CONbits.TMR1ON = 1;          break;
                             case '5':               // 2.3
                                     TMR1H=0xDC;     TMR1L=0x10;      T1CONbits.TMR1ON = 1;          break;
                             case '6':               // 2.2  
                                     TMR1H=0xDD;     TMR1L=0xA0;      T1CONbits.TMR1ON = 1;          break;
                             case '7':            // 2.1
                                     TMR1H=0xDF;     TMR1L=0x30;      T1CONbits.TMR1ON = 1;          break;
                             case '8':           // 2.0
                                     TMR1H=0xE0;     TMR1L=0xC0;      T1CONbits.TMR1ON = 1;          break;
                             case '9':           // 1.9
                                     TMR1H=0xE2;     TMR1L=0x50;      T1CONbits.TMR1ON = 1;          break;
                             default:
                                     break;
                            }
                        break;
                case '8':           //1.8-1.2
/**/
                        switch(FAN_SPEED1_1s)
                             {
                             case '0':           // 1.8
                                     TMR1H=0xE3;     TMR1L=0xE0;       T1CONbits.TMR1ON = 1;         break;
                             case '1':           // 1.75
                                     TMR1H=0xE4;     TMR1L=0xA8;       T1CONbits.TMR1ON = 1;         break;
                             case '2':           // 1.7
                                     TMR1H=0xE5;     TMR1L=0x70;       T1CONbits.TMR1ON = 1;         break;
                             case '3':           // 1.65
                                     TMR1H=0xE6;     TMR1L=0x38;       T1CONbits.TMR1ON = 1;         break;
                             case '4'://TX1REG='n';      // 1.6
                                     TMR1H=0xE7;     TMR1L=0x00;       T1CONbits.TMR1ON = 1;         break;
                             case '5':               // 1.5
                                     TMR1H=0xE8;     TMR1L=0x90;       T1CONbits.TMR1ON = 1;         break;
                             case '6':               // 1.4   
                                     TMR1H=0xEA;     TMR1L=0x20;       T1CONbits.TMR1ON = 1;         break;
                             case '7':            //1.3
                                     TMR1H=0xEB;     TMR1L=0xB0;       T1CONbits.TMR1ON = 1;         break;
                             case '8':           //1.25
                                     TMR1H=0xEC;     TMR1L=0x78;       T1CONbits.TMR1ON = 1;         break;
                             case '9':           // 1.2
                                     TMR1H=0xED;     TMR1L=0x40;       T1CONbits.TMR1ON = 1;         break;
                             default:
                                     break;
                            }
                        break;
                case '9':           // 1.1-0.2
/**/
                        switch(FAN_SPEED1_1s)
                             {
                             case '0':           // 1.1
                                     TMR1H=0xEE;    TMR1L=0xD0;        T1CONbits.TMR1ON = 1;         break;
                             case '1':           // 1.0
                                     TMR1H=0xF0;    TMR1L=0x60;        T1CONbits.TMR1ON = 1;         break;
                             case '2':           // 0.9
                                     TMR1H=0xF1;    TMR1L=0xF0;        T1CONbits.TMR1ON = 1;         break;
                             case '3':           // 0.8
                                     TMR1H=0xF3;    TMR1L=0x80;        T1CONbits.TMR1ON = 1;         break;
                             case '4'://TX1REG='n';      // 0.7
                                     TMR1H=0xF5;    TMR1L=0x10;        T1CONbits.TMR1ON = 1;         break;
                             case '5':               // 0.6
                                     TMR1H=0xF6;    TMR1L=0xA0;        T1CONbits.TMR1ON = 1;         break;
                             case '6':               // 0.5    
                                     TMR1H=0xF8;    TMR1L=0x30;        T1CONbits.TMR1ON = 1;         break;
                             case '7':            //0.4
                                     TMR1H=0xF9;    TMR1L=0xC0;        T1CONbits.TMR1ON = 1;         break;
                             case '8':           //0.3
                                     TMR1H=0xFB;    TMR1L=0x50;        T1CONbits.TMR1ON = 1;         break;
                             case '9':           // 0.2
                                     TMR1H=0xFC;    TMR1L=0xE0;        T1CONbits.TMR1ON = 1;         break;
                             default:
                              break;
                            }
                        break;
                        default:
                        break;
            }
        }  
        PIR1bits.CCP1IF = 0;
        }
        if(PIR3bits.CCP3IF == 1)
        {
            PIR3bits.CCP3IF=0;
        // ************************************* FAN SPEED *********************************************** //    
        if(FAN_FLAG2==1 && ak==1)
        {
            switch(FAN_SPEED2_10s)
                {
                case '0':           // 8.5
                    /**/
                        switch(FAN_SPEED2_1s)
                             {
                             case '0':           // 8.5
                                     TMR3H=0x7B;        TMR3L=0x30;     T3CONbits.TMR3ON = 1;       break;
                             case '1':           // 8.4
                                     TMR3H=0x7C;        TMR3L=0xC0;     T3CONbits.TMR3ON = 1;       break;
                             case '2':           // 8.35
                                     TMR3H=0x7D;        TMR3L=0x88;     T3CONbits.TMR3ON = 1;       break;
                             case '3':           // 8.25
                                     TMR3H=0x7F;        TMR3L=0x18;     T3CONbits.TMR3ON = 1;       break;
                             case '4':          // 8.15
                                     TMR3H=0x80;        TMR3L=0xA8;     T3CONbits.TMR3ON = 1;       break;
                             case '5':               // 8.1
                                     TMR3H=0x81;        TMR3L=0x70;     T3CONbits.TMR3ON = 1;       break;
                             case '6':               // 8.0    
                                     TMR3H=0x83;        TMR3L=0x00;     T3CONbits.TMR3ON = 1;       break;
                             case '7':            //7.95
                                     TMR3H=0x83;        TMR3L=0xC8;     T3CONbits.TMR3ON = 1;       break;
                             case '8':           //7.9
                                     TMR3H=0x87;        TMR3L=0xB0;     T3CONbits.TMR3ON = 1;       break;
                             case '9':           // 7.85
                                     TMR3H=0x85;        TMR3L=0x58;     T3CONbits.TMR3ON = 1;       break;
                             default:
                                 break;
                         }                    
                        break;
                case '1':           // 7.8-7.3

                            switch(FAN_SPEED2_1s)
                                 {
                                 case '0':           // 7.8
                                         TMR3H=0x86;    TMR3L=0x20;    T3CONbits.TMR3ON = 1;        break;
                                 case '1':           // 7.75
                                         TMR3H=0x86;    TMR3L=0xE8;    T3CONbits.TMR3ON = 1;        break;
                                 case '2':           // 7.7
                                         TMR3H=0x87;    TMR3L=0xB0;    T3CONbits.TMR3ON = 1;        break;
                                 case '3':           // 7.65
                                         TMR3H=0x88;    TMR3L=0x78;    T3CONbits.TMR3ON = 1;        break;
                                 case '4':            // 7.6
                                         TMR3H=0x89;    TMR3L=0x40;    T3CONbits.TMR3ON = 1;        break;
                                 case '5':               // 7.55
                                         TMR3H=0x8A;    TMR3L=0x08;    T3CONbits.TMR3ON = 1;        break;
                                 case '6':               // 7.5    
                                         TMR3H=0x8A;    TMR3L=0xD0;    T3CONbits.TMR3ON = 1;        break;
                                 case '7':            //7.45
                                         TMR3H=0x8B;    TMR3L=0x98;    T3CONbits.TMR3ON = 1;        break;
                                 case '8':           //7.4
                                         TMR3H=0x8C;    TMR3L=0x60;    T3CONbits.TMR3ON = 1;        break;
                                 case '9':           // 7.35
                                         TMR3H=0x8D;    TMR3L=0x28;    T3CONbits.TMR3ON = 1;        break;
                                 default:
                                     break;
                                }
                        break;
                case '2':           // 7.3-
/**/
                        switch(FAN_SPEED2_1s)
                             {
                             case '0':           // 7.3-6.85
                                     TMR3H=0x8D;     TMR3L=0xF0;       T3CONbits.TMR3ON = 1;        break;
                             case '1':           // 7.25
                                     TMR3H=0x8E;     TMR3L=0xB8;       T3CONbits.TMR3ON = 1;        break;
                             case '2':           // 7.20
                                     TMR3H=0x8F;     TMR3L=0x80;       T3CONbits.TMR3ON = 1;        break;
                             case '3':           // 7.15
                                     TMR3H=0x90;     TMR3L=0x48;       T3CONbits.TMR3ON = 1;        break;
                             case '4'://TX1REG='n';      // 7.1
                                     TMR3H=0x91;     TMR3L=0x10;       T3CONbits.TMR3ON = 1;        break;
                             case '5':               // 7.05
                                     TMR3H=0x91;     TMR3L=0xD8;       T3CONbits.TMR3ON = 1;        break;
                             case '6':               // 7.0    
                                     TMR3H=0x92;     TMR3L=0xA0;       T3CONbits.TMR3ON = 1;        break;
                             case '7':            //6.95
                                     TMR3H=0x93;     TMR3L=0x68;       T3CONbits.TMR3ON = 1;        break;
                             case '8':           //6.9
                                     TMR3H=0x94;     TMR3L=0x30;       T3CONbits.TMR3ON = 1;        break;
                             case '9':           // 6.85
                                     TMR3H=0x94;     TMR3L=0xF8;       T3CONbits.TMR3ON = 1;        break;
                             default:
                                 break;
                         }                    
                        break;
                case '3':           // 6.8-5.9                
/**/
                        switch(FAN_SPEED2_1s)
                             {
                             case '0':           // 6.8
                                     TMR3H=0x95;     TMR3L=0xC0;      T3CONbits.TMR3ON = 1;         break;
                             case '1':           // 6.7
                                     TMR3H=0x97;     TMR3L=0x50;      T3CONbits.TMR3ON = 1;         break;
                             case '2':           // 6.6
                                     TMR3H=0x98;     TMR3L=0xE0;      T3CONbits.TMR3ON = 1;         break;
                             case '3':           // 6.5
                                     TMR3H=0x9A;     TMR3L=0x70;      T3CONbits.TMR3ON = 1;         break;
                             case '4'://TX1REG='n';      // 6.4
                                     TMR3H=0x9C;     TMR3L=0x00;      T3CONbits.TMR3ON = 1;         break;
                             case '5':               // 6.3
                                     TMR3H=0x9D;     TMR3L=0x90;      T3CONbits.TMR3ON = 1;         break;
                             case '6':               // 6.2   
                                     TMR3H=0x9F;     TMR3L=0x20;      T3CONbits.TMR3ON = 1;         break;
                             case '7':            //6.1
                                     TMR3H=0xA0;     TMR3L=0xB0;      T3CONbits.TMR3ON = 1;         break;
                             case '8':           //6.0
                                     TMR3H=0xA2;     TMR3L=0x40;      T3CONbits.TMR3ON = 1;         break;
                             case '9':           // 5.9
                                     TMR3H=0xA3;     TMR3L=0xD0;      T3CONbits.TMR3ON = 1;         break;
                             default:
                                     break;
                            }
                        break;
                case '4'://TX1REG='n';      // 5.8-4.9                    
/**/
                        switch(FAN_SPEED2_1s)
                             {
                             case '0':           // 5.8
                                     TMR3H=0xA5;     TMR3L=0x60;      T3CONbits.TMR3ON = 1;         break;
                             case '1':           // 5.7
                                     TMR3H=0xA6;     TMR3L=0xF0;      T3CONbits.TMR3ON = 1;         break;
                             case '2':           // 5.6
                                     TMR3H=0xA8;     TMR3L=0x80;      T3CONbits.TMR3ON = 1;         break;
                             case '3':           // 5.5
                                     TMR3H=0xAA;     TMR3L=0x10;      T3CONbits.TMR3ON = 1;         break;
                             case '4':           // 5.4
                                     TMR3H=0xAB;     TMR3L=0xA0;      T3CONbits.TMR3ON = 1;         break;
                             case '5':               // 5.3
                                     TMR3H=0xAD;     TMR3L=0x30;      T3CONbits.TMR3ON = 1;         break;
                             case '6':               // 5.2    
                                     TMR3H=0xAE;     TMR3L=0xC0;      T3CONbits.TMR3ON = 1;         break;
                             case '7':              // 5.1
                                     TMR3H=0xB0;     TMR3L=0x50;      T3CONbits.TMR3ON = 1;         break;
                             case '8':              // 5.0
                                     TMR3H=0xB1;     TMR3L=0xE0;      T3CONbits.TMR3ON = 1;         break;
                             case '9':              // 4.9
                                     TMR3H=0xB3;     TMR3L=0x70;      T3CONbits.TMR3ON = 1;          break;
                             default:
                                     break;
                         }
                        break;
                case '5':               // 4.8-3.9
/**/
                        switch(FAN_SPEED2_1s)
                             {
                             case '0':           // 4.8
                                     TMR3H=0xB5;     TMR3L=0x00;      T3CONbits.TMR3ON = 1;          break;
                             case '1':           // 4.7
                                     TMR3H=0xB6;     TMR3L=0x90;      T3CONbits.TMR3ON = 1;          break;
                             case '2':           // 4.6
                                     TMR3H=0xB8;     TMR3L=0x20;      T3CONbits.TMR3ON = 1;          break;
                             case '3':           // 4.5
                                     TMR3H=0xB9;     TMR3L=0xB0;      T3CONbits.TMR3ON = 1;          break;
                             case '4'://TX1REG='n';      // 4.4
                                     TMR3H=0xBB;     TMR3L=0x40;      T3CONbits.TMR3ON = 1;          break;
                             case '5':               // 4.3
                                     TMR3H=0xBC;     TMR3L=0xD0;      T3CONbits.TMR3ON = 1;          break;
                             case '6':               // 4.2   
                                     TMR3H=0xBE;     TMR3L=0x60;      T3CONbits.TMR3ON = 1;          break;
                             case '7':            //4.1
                                     TMR3H=0xBF;     TMR3L=0xF0;      T3CONbits.TMR3ON = 1;          break;
                             case '8':           //4.0
                                     TMR3H=0xC1;     TMR3L=0x80;      T3CONbits.TMR3ON = 1;          break;
                             case '9':           // 3.9
                                     TMR3H=0xC3;     TMR3L=0x10;      T3CONbits.TMR3ON = 1;          break;
                             default:
                                 break;
                            }                    
                        break;
                case '6':               // 3.8-2.9 
/**/
                        switch(FAN_SPEED2_1s)
                             {
                             case '0':           // 3.8
                                     TMR3H=0xC4;     TMR3L=0xA0;      T3CONbits.TMR3ON = 1;          break;
                             case '1':           // 3.7
                                     TMR3H=0xC6;     TMR3L=0x30;      T3CONbits.TMR3ON = 1;           break;
                             case '2':           // 3.6
                                     TMR3H=0xC7;     TMR3L=0xC0;      T3CONbits.TMR3ON = 1;          break;
                             case '3':           // 3.5
                                     TMR3H=0xC9;     TMR3L=0x50;      T3CONbits.TMR3ON = 1;          break;
                             case '4'://TX1REG='n';      // 3.4
                                     TMR3H=0xCA;     TMR3L=0xE0;      T3CONbits.TMR3ON = 1;          break;
                             case '5':               // 3.3
                                     TMR3H=0xCC;     TMR3L=0x70;      T3CONbits.TMR3ON = 1;          break;
                             case '6':               // 3.2   
                                     TMR3H=0xCE;     TMR3L=0x00;      T3CONbits.TMR3ON = 1;          break;
                             case '7':            //3.1
                                     TMR3H=0xCF;     TMR3L=0x90;      T3CONbits.TMR3ON = 1;          break;
                             case '8':           // 3.0
                                     TMR3H=0xD1;     TMR3L=0x20;      T3CONbits.TMR3ON = 1;          break;
                             case '9':           // 2.9
                                     TMR3H=0xD2;     TMR3L=0xB0;      T3CONbits.TMR3ON = 1;          break;
                             default:
                                     break;
                            }                    
                        break;
                case '7':            //2.8-1.9
/**/
                        switch(FAN_SPEED2_1s)
                             {
                             case '0':           // 2.8
                                     TMR3H=0xD4;     TMR3L=0x40;      T3CONbits.TMR3ON = 1;          break;
                             case '1':           // 2.
                                     TMR3H=0xD5;     TMR3L=0xD0;      T3CONbits.TMR3ON = 1;          break;
                             case '2':           // 2.6
                                     TMR3H=0xD7;     TMR3L=0x60;      T3CONbits.TMR3ON = 1;          break;
                             case '3':           // 2.5
                                     TMR3H=0xD8;     TMR3L=0xF0;      T3CONbits.TMR3ON = 1;          break;
                             case '4'://TX1REG='n';      // 2.4
                                     TMR3H=0xDA;     TMR3L=0x80;      T3CONbits.TMR3ON = 1;          break;
                             case '5':               // 2.3
                                     TMR3H=0xDC;     TMR3L=0x10;      T3CONbits.TMR3ON = 1;          break;
                             case '6':               // 2.2  
                                     TMR3H=0xDD;     TMR3L=0xA0;      T3CONbits.TMR3ON = 1;          break;
                             case '7':            // 2.1
                                     TMR3H=0xDF;     TMR3L=0x30;      T3CONbits.TMR3ON = 1;          break;
                             case '8':           // 2.0
                                     TMR3H=0xE0;     TMR3L=0xC0;      T3CONbits.TMR3ON = 1;          break;
                             case '9':           // 1.9
                                     TMR3H=0xE2;     TMR3L=0x50;      T3CONbits.TMR3ON = 1;          break;
                             default:
                                     break;
                            }
                        break;
                case '8':           //1.8-1.2
/**/
                        switch(FAN_SPEED2_1s)
                             {
                             case '0':           // 1.8
                                     TMR3H=0xE3;     TMR3L=0xE0;       T3CONbits.TMR3ON = 1;         break;
                             case '1':           // 1.75
                                     TMR3H=0xE4;     TMR3L=0xA8;       T3CONbits.TMR3ON = 1;         break;
                             case '2':           // 1.7
                                     TMR3H=0xE5;     TMR3L=0x70;       T3CONbits.TMR3ON = 1;         break;
                             case '3':           // 1.65
                                     TMR3H=0xE6;     TMR3L=0x38;       T3CONbits.TMR3ON = 1;         break;
                             case '4'://TX1REG='n';      // 1.6
                                     TMR3H=0xE7;     TMR3L=0x00;       T3CONbits.TMR3ON = 1;         break;
                             case '5':               // 1.5
                                     TMR3H=0xE8;     TMR3L=0x90;       T3CONbits.TMR3ON = 1;         break;
                             case '6':               // 1.4   
                                     TMR3H=0xEA;     TMR3L=0x20;       T3CONbits.TMR3ON = 1;         break;
                             case '7':            //1.3
                                     TMR3H=0xEB;     TMR3L=0xB0;       T3CONbits.TMR3ON = 1;         break;
                             case '8':           //1.25
                                     TMR3H=0xEC;     TMR3L=0x78;       T3CONbits.TMR3ON = 1;         break;
                             case '9':           // 1.2
                                     TMR3H=0xED;     TMR3L=0x40;       T3CONbits.TMR3ON = 1;         break;
                             default:
                                     break;
                            }
                        break;
                case '9':           // 1.1-0.2
/**/
                        switch(FAN_SPEED2_1s)
                             {
                             case '0':           // 1.1
                                     TMR3H=0xEE;    TMR3L=0xD0;        T3CONbits.TMR3ON = 1;         break;
                             case '1':           // 1.0
                                     TMR3H=0xF0;    TMR3L=0x60;        T3CONbits.TMR3ON = 1;         break;
                             case '2':           // 0.9
                                     TMR3H=0xF1;    TMR3L=0xF0;        T3CONbits.TMR3ON = 1;         break;
                             case '3':           // 0.8
                                     TMR3H=0xF3;    TMR3L=0x80;        T3CONbits.TMR3ON = 1;         break;
                             case '4'://TX1REG='n';      // 0.7
                                     TMR3H=0xF5;    TMR3L=0x10;        T3CONbits.TMR3ON = 1;         break;
                             case '5':               // 0.6
                                     TMR3H=0xF6;    TMR3L=0xA0;        T3CONbits.TMR3ON = 1;         break;
                             case '6':               // 0.5    
                                     TMR3H=0xF8;    TMR3L=0x30;        T3CONbits.TMR3ON = 1;         break;
                             case '7':            //0.4
                                     TMR3H=0xF9;    TMR3L=0xC0;        T3CONbits.TMR3ON = 1;         break;
                             case '8':           //0.3
                                     TMR3H=0xFB;    TMR3L=0x50;        T3CONbits.TMR3ON = 1;         break;
                             case '9':           // 0.2
                                     TMR3H=0xFC;    TMR3L=0xE0;        T3CONbits.TMR3ON = 1;         break;
                             default:
                                     break;
                            }
                        break;
                default:
                        break;
            }
        }  
        PIR3bits.CCP3IF = 0;
        }
        
        if(PIR4bits.CCP9IF == 1)
        {
            PIR4bits.CCP9IF=0;
        // ************************************* FAN SPEED *********************************************** //    
        if(FAN_FLAG3==1 && tk==1)
        {
            switch(FAN_SPEED3_10s)
                {
                case '0':           // 8.5
                    /**/
                        switch(FAN_SPEED3_1s)
                             {
                             case '0':           // 8.5
                                     TMR5H=0x7B;        TMR5L=0x30;     T5CONbits.TMR5ON = 1;       break;
                             case '1':           // 8.4
                                     TMR5H=0x7C;        TMR5L=0xC0;     T5CONbits.TMR5ON = 1;       break;
                             case '2':           // 8.35
                                     TMR5H=0x7D;        TMR5L=0x88;     T5CONbits.TMR5ON = 1;       break;
                             case '3':           // 8.25
                                     TMR5H=0x7F;        TMR5L=0x18;     T5CONbits.TMR5ON = 1;       break;
                             case '4':          // 8.15
                                     TMR5H=0x80;        TMR5L=0xA8;     T5CONbits.TMR5ON = 1;       break;
                             case '5':               // 8.1
                                     TMR5H=0x81;        TMR5L=0x70;     T5CONbits.TMR5ON = 1;       break;
                             case '6':               // 8.0    
                                     TMR5H=0x83;        TMR5L=0x00;     T5CONbits.TMR5ON = 1;       break;
                             case '7':            //7.95
                                     TMR5H=0x83;        TMR5L=0xC8;     T5CONbits.TMR5ON = 1;       break;
                             case '8':           //7.9
                                     TMR5H=0x87;        TMR5L=0xB0;     T5CONbits.TMR5ON = 1;       break;
                             case '9':           // 7.85
                                     TMR5H=0x85;        TMR5L=0x58;     T5CONbits.TMR5ON = 1;       break;
                             default:
                                 break;
                         }                    
                        break;
                case '1':           // 7.8-7.3

                            switch(FAN_SPEED3_1s)
                                 {
                                 case '0':           // 7.8
                                         TMR5H=0x86;    TMR5L=0x20;    T5CONbits.TMR5ON = 1;        break;
                                 case '1':           // 7.75
                                         TMR5H=0x86;    TMR5L=0xE8;    T5CONbits.TMR5ON = 1;        break;
                                 case '2':           // 7.7
                                         TMR5H=0x87;    TMR5L=0xB0;    T5CONbits.TMR5ON = 1;        break;
                                 case '3':           // 7.65
                                         TMR5H=0x88;    TMR5L=0x78;    T5CONbits.TMR5ON = 1;        break;
                                 case '4':            // 7.6
                                         TMR5H=0x89;    TMR5L=0x40;    T5CONbits.TMR5ON = 1;        break;
                                 case '5':               // 7.55
                                         TMR5H=0x8A;    TMR5L=0x08;    T5CONbits.TMR5ON = 1;        break;
                                 case '6':               // 7.5    
                                         TMR5H=0x8A;    TMR5L=0xD0;    T5CONbits.TMR5ON = 1;        break;
                                 case '7':            //7.45
                                         TMR5H=0x8B;    TMR5L=0x98;    T5CONbits.TMR5ON = 1;        break;
                                 case '8':           //7.4
                                         TMR5H=0x8C;    TMR5L=0x60;    T5CONbits.TMR5ON = 1;        break;
                                 case '9':           // 7.35
                                         TMR5H=0x8D;    TMR5L=0x28;    T5CONbits.TMR5ON = 1;        break;
                                 default:
                                     break;
                                }
                        break;
                case '2':           // 7.3-
/**/
                        switch(FAN_SPEED3_1s)
                             {
                             case '0':           // 7.3-6.85
                                     TMR5H=0x8D;     TMR5L=0xF0;       T5CONbits.TMR5ON = 1;        break;
                             case '1':           // 7.25
                                     TMR5H=0x8E;     TMR5L=0xB8;       T5CONbits.TMR5ON = 1;        break;
                             case '2':           // 7.20
                                     TMR5H=0x8F;     TMR5L=0x80;       T5CONbits.TMR5ON = 1;        break;
                             case '3':           // 7.15
                                     TMR5H=0x90;     TMR5L=0x48;       T5CONbits.TMR5ON = 1;        break;
                             case '4'://TX1REG='n';      // 7.1
                                     TMR5H=0x91;     TMR5L=0x10;       T5CONbits.TMR5ON = 1;        break;
                             case '5':               // 7.05
                                     TMR5H=0x91;     TMR5L=0xD8;       T5CONbits.TMR5ON = 1;        break;
                             case '6':               // 7.0    
                                     TMR5H=0x92;     TMR5L=0xA0;       T5CONbits.TMR5ON = 1;        break;
                             case '7':            //6.95
                                     TMR5H=0x93;     TMR5L=0x68;       T5CONbits.TMR5ON = 1;        break;
                             case '8':           //6.9
                                     TMR5H=0x94;     TMR5L=0x30;       T5CONbits.TMR5ON = 1;        break;
                             case '9':           // 6.85
                                     TMR5H=0x94;     TMR5L=0xF8;       T5CONbits.TMR5ON = 1;        break;
                             default:
                                 break;
                         }                    
                        break;
                case '3':           // 6.8-5.9                
/**/
                        switch(FAN_SPEED3_1s)
                             {
                             case '0':           // 6.8
                                     TMR5H=0x95;     TMR5L=0xC0;      T5CONbits.TMR5ON = 1;         break;
                             case '1':           // 6.7
                                     TMR5H=0x97;     TMR5L=0x50;      T5CONbits.TMR5ON = 1;         break;
                             case '2':           // 6.6
                                     TMR5H=0x98;     TMR5L=0xE0;      T5CONbits.TMR5ON = 1;         break;
                             case '3':           // 6.5
                                     TMR5H=0x9A;     TMR5L=0x70;      T5CONbits.TMR5ON = 1;         break;
                             case '4'://TX1REG='n';      // 6.4
                                     TMR5H=0x9C;     TMR5L=0x00;      T5CONbits.TMR5ON = 1;         break;
                             case '5':               // 6.3
                                     TMR5H=0x9D;     TMR5L=0x90;      T5CONbits.TMR5ON = 1;         break;
                             case '6':               // 6.2   
                                     TMR5H=0x9F;     TMR5L=0x20;      T5CONbits.TMR5ON = 1;         break;
                             case '7':            //6.1
                                     TMR5H=0xA0;     TMR5L=0xB0;      T5CONbits.TMR5ON = 1;         break;
                             case '8':           //6.0
                                     TMR5H=0xA2;     TMR5L=0x40;      T5CONbits.TMR5ON = 1;         break;
                             case '9':           // 5.9
                                     TMR5H=0xA3;     TMR5L=0xD0;      T5CONbits.TMR5ON = 1;         break;
                             default:
                                     break;
                            }
                        break;
                case '4'://TX1REG='n';      // 5.8-4.9                    
/**/
                        switch(FAN_SPEED3_1s)
                             {
                             case '0':           // 5.8
                                     TMR5H=0xA5;     TMR5L=0x60;      T5CONbits.TMR5ON = 1;         break;
                             case '1':           // 5.7
                                     TMR5H=0xA6;     TMR5L=0xF0;      T5CONbits.TMR5ON = 1;         break;
                             case '2':           // 5.6
                                     TMR5H=0xA8;     TMR5L=0x80;      T5CONbits.TMR5ON = 1;         break;
                             case '3':           // 5.5
                                     TMR5H=0xAA;     TMR5L=0x10;      T5CONbits.TMR5ON = 1;         break;
                             case '4':           // 5.4
                                     TMR5H=0xAB;     TMR5L=0xA0;      T5CONbits.TMR5ON = 1;         break;
                             case '5':               // 5.3
                                     TMR5H=0xAD;     TMR5L=0x30;      T5CONbits.TMR5ON = 1;         break;
                             case '6':               // 5.2    
                                     TMR5H=0xAE;     TMR5L=0xC0;      T5CONbits.TMR5ON = 1;         break;
                             case '7':              // 5.1
                                     TMR5H=0xB0;     TMR5L=0x50;      T5CONbits.TMR5ON = 1;         break;
                             case '8':              // 5.0
                                     TMR5H=0xB1;     TMR5L=0xE0;      T5CONbits.TMR5ON = 1;         break;
                             case '9':              // 4.9
                                     TMR5H=0xB3;     TMR5L=0x70;      T5CONbits.TMR5ON = 1;          break;
                             default:
                                     break;
                         }
                        break;
                case '5':               // 4.8-3.9
/**/
                        switch(FAN_SPEED3_1s)
                             {
                             case '0':           // 4.8
                                     TMR5H=0xB5;     TMR5L=0x00;      T5CONbits.TMR5ON = 1;          break;
                             case '1':           // 4.7
                                     TMR5H=0xB6;     TMR5L=0x90;      T5CONbits.TMR5ON = 1;          break;
                             case '2':           // 4.6
                                     TMR5H=0xB8;     TMR5L=0x20;      T5CONbits.TMR5ON = 1;          break;
                             case '3':           // 4.5
                                     TMR5H=0xB9;     TMR5L=0xB0;      T5CONbits.TMR5ON = 1;          break;
                             case '4'://TX1REG='n';      // 4.4
                                     TMR5H=0xBB;     TMR5L=0x40;      T5CONbits.TMR5ON = 1;          break;
                             case '5':               // 4.3
                                     TMR5H=0xBC;     TMR5L=0xD0;      T5CONbits.TMR5ON = 1;          break;
                             case '6':               // 4.2   
                                     TMR5H=0xBE;     TMR5L=0x60;      T5CONbits.TMR5ON = 1;          break;
                             case '7':            //4.1
                                     TMR5H=0xBF;     TMR5L=0xF0;      T5CONbits.TMR5ON = 1;          break;
                             case '8':           //4.0
                                     TMR5H=0xC1;     TMR5L=0x80;      T5CONbits.TMR5ON = 1;          break;
                             case '9':           // 3.9
                                     TMR5H=0xC3;     TMR5L=0x10;      T5CONbits.TMR5ON = 1;          break;
                             default:
                                 break;
                            }                    
                        break;
                case '6':               // 3.8-2.9 
/**/
                        switch(FAN_SPEED3_1s)
                             {
                             case '0':           // 3.8
                                     TMR5H=0xC4;     TMR5L=0xA0;      T5CONbits.TMR5ON = 1;          break;
                             case '1':           // 3.7
                                     TMR5H=0xC6;     TMR5L=0x30;      T5CONbits.TMR5ON = 1;           break;
                             case '2':           // 3.6
                                     TMR5H=0xC7;     TMR5L=0xC0;      T5CONbits.TMR5ON = 1;          break;
                             case '3':           // 3.5
                                     TMR5H=0xC9;     TMR5L=0x50;      T5CONbits.TMR5ON = 1;          break;
                             case '4'://TX1REG='n';      // 3.4
                                     TMR5H=0xCA;     TMR5L=0xE0;      T5CONbits.TMR5ON = 1;          break;
                             case '5':               // 3.3
                                     TMR5H=0xCC;     TMR5L=0x70;      T5CONbits.TMR5ON = 1;          break;
                             case '6':               // 3.2   
                                     TMR5H=0xCE;     TMR5L=0x00;      T5CONbits.TMR5ON = 1;          break;
                             case '7':            //3.1
                                     TMR5H=0xCF;     TMR5L=0x90;      T5CONbits.TMR5ON = 1;          break;
                             case '8':           // 3.0
                                     TMR5H=0xD1;     TMR5L=0x20;      T5CONbits.TMR5ON = 1;          break;
                             case '9':           // 2.9
                                     TMR5H=0xD2;     TMR5L=0xB0;      T5CONbits.TMR5ON = 1;          break;
                             default:
                                     break;
                            }                    
                        break;
                case '7':            //2.8-1.9
/**/
                        switch(FAN_SPEED3_1s)
                             {
                             case '0':           // 2.8
                                     TMR5H=0xD4;     TMR5L=0x40;      T5CONbits.TMR5ON = 1;          break;
                             case '1':           // 2.7
                                     TMR5H=0xD5;     TMR5L=0xD0;      T5CONbits.TMR5ON = 1;          break;
                             case '2':           // 2.6
                                     TMR5H=0xD7;     TMR5L=0x60;      T5CONbits.TMR5ON = 1;          break;
                             case '3':           // 2.5
                                     TMR5H=0xD8;     TMR5L=0xF0;      T5CONbits.TMR5ON = 1;          break;
                             case '4'://TX1REG='n';      // 2.4
                                     TMR5H=0xDA;     TMR5L=0x80;      T5CONbits.TMR5ON = 1;          break;
                             case '5':               // 2.3
                                     TMR5H=0xDC;     TMR5L=0x10;      T5CONbits.TMR5ON = 1;          break;
                             case '6':               // 2.2  
                                     TMR5H=0xDD;     TMR5L=0xA0;      T5CONbits.TMR5ON = 1;          break;
                             case '7':            // 2.1
                                     TMR5H=0xDF;     TMR5L=0x30;      T5CONbits.TMR5ON = 1;          break;
                             case '8':           // 2.0
                                     TMR5H=0xE0;     TMR5L=0xC0;      T5CONbits.TMR5ON = 1;          break;
                             case '9':           // 1.9
                                     TMR5H=0xE2;     TMR5L=0x50;      T5CONbits.TMR5ON = 1;          break;
                             default:
                                     break;
                            }
                        break;
                case '8':           //1.8-1.2
/**/
                        switch(FAN_SPEED3_1s)
                             {
                             case '0':           // 1.8
                                     TMR5H=0xE3;     TMR5L=0xE0;       T5CONbits.TMR5ON = 1;         break;
                             case '1':           // 1.75
                                     TMR5H=0xE4;     TMR5L=0xA8;       T5CONbits.TMR5ON = 1;         break;
                             case '2':           // 1.7
                                     TMR5H=0xE5;     TMR5L=0x70;       T5CONbits.TMR5ON = 1;         break;
                             case '3':           // 1.65
                                     TMR5H=0xE6;     TMR5L=0x38;       T5CONbits.TMR5ON = 1;         break;
                             case '4'://TX1REG='n';      // 1.6
                                     TMR5H=0xE7;     TMR5L=0x00;       T5CONbits.TMR5ON = 1;         break;
                             case '5':               // 1.5
                                     TMR5H=0xE8;     TMR5L=0x90;       T5CONbits.TMR5ON = 1;         break;
                             case '6':               // 1.4   
                                     TMR5H=0xEA;     TMR5L=0x20;       T5CONbits.TMR5ON = 1;         break;
                             case '7':            //1.3
                                     TMR5H=0xEB;     TMR5L=0xB0;       T5CONbits.TMR5ON = 1;         break;
                             case '8':           //1.25
                                     TMR5H=0xEC;     TMR5L=0x78;       T5CONbits.TMR5ON = 1;         break;
                             case '9':           // 1.2
                                     TMR5H=0xED;     TMR5L=0x40;       T5CONbits.TMR5ON = 1;         break;
                             default:
                                     break;
                            }
                        break;
                case '9':           // 1.1-0.2
/**/
                        switch(FAN_SPEED3_1s)
                             {
                             case '0':           // 1.1
                                     TMR5H=0xEE;    TMR5L=0xD0;        T5CONbits.TMR5ON = 1;         break;
                             case '1':           // 1.0
                                     TMR5H=0xF0;    TMR5L=0x60;        T5CONbits.TMR5ON = 1;         break;
                             case '2':           // 0.9
                                     TMR5H=0xF1;    TMR5L=0xF0;        T5CONbits.TMR5ON = 1;         break;
                             case '3':           // 0.8
                                     TMR5H=0xF3;    TMR5L=0x80;        T5CONbits.TMR5ON = 1;         break;
                             case '4'://TX1REG='n';      // 0.7
                                     TMR5H=0xF5;    TMR5L=0x10;        T5CONbits.TMR5ON = 1;         break;
                             case '5':               // 0.6
                                     TMR5H=0xF6;    TMR5L=0xA0;        T5CONbits.TMR5ON = 1;         break;
                             case '6':               // 0.5    
                                     TMR5H=0xF8;    TMR5L=0x30;        T5CONbits.TMR5ON = 1;         break;
                             case '7':            //0.4
                                     TMR5H=0xF9;    TMR5L=0xC0;        T5CONbits.TMR5ON = 1;         break;
                             case '8':           //0.3
                                     TMR5H=0xFB;    TMR5L=0x50;        T5CONbits.TMR5ON = 1;         break;
                             case '9':           // 0.2
                                     TMR5H=0xFC;    TMR5L=0xE0;        T5CONbits.TMR5ON = 1;         break;
                             default:
                              break;
                            }
                        break;
                       default:
                        break;
            }
        }  
        PIR4bits.CCP9IF = 0;
        }

        
        

    }
}

void ACTION(char Switch_Num_10s, char Switch_Num_1s, char sw_status, char speed_bit1, char speed_bit2, char parent,char finalFrameStat);
//void ACTION(char Switch_Num_10s, char Switch_Num_1s, char sw_status, char speed_bit1, char speed_bit2, char parent );
//void ACTION(char SWITCH_Num, char status, char speed_bit1, char speed_bit2, char parent );
void pin_manager();
void TMR5_Initialize();
void TMR2_Initialize();
void TMR4_Initialize();
void TMR6_Initialize();
void TMR3_Initialize();
void TMR1_Initialize();
void CCP1_Initialize();
void CCP3_Initialize();
//void CCP5_Initialize();
void CCP9_Initialize();
void EUSART_Initialize();
void copy_frame(int start,int end);
void writeUART_FULL(char *str2Write);
void writeUART(char *str2Write);

void periperal_init()
    {
     EUSART_Initialize();
      CCP1_Initialize();
      CCP3_Initialize();
//      CCP5_Initialize();
      CCP9_Initialize();
     
      TMR1_Initialize();
      TMR3_Initialize();
      TMR2_Initialize();
      TMR4_Initialize();
      TMR5_Initialize();
      TMR6_Initialize();
    }
                
        // ************************************* MAIN *********************************************** //
void main() {
    __delay_ms(2000);

    int RX_CHK_FLAG_start1 = 0 ,RX_CHK_FLAG_start2 = 0, start_flag=0;
    int RX_CHK_FLAG_end1 = 0, RX_CHK_FLAG_end2 = 0, end_flag=0;
    
    pin_manager();
    periperal_init();

    FAN = OFF;    RELAY1 = OFF;    RELAY2 = 1;    RELAY3 = 1;    RELAY4 = 1;    
    M1=ONN;       M2=ONN;    M3=ONN;    M4=ONN;    M5=ONN;       

    st[0]='%';
    st[21]='@';
    
    while(1){
      
       if(TX_FLAG==1)
        {
//           TX1REG='X';            // %%0310000@@%%0310000@@
            TX_FLAG=0;            start_flag = 0;            end_flag = 0;
            
            if(name[0]=='%' && name[1]=='%' && name[14]=='@' && name[15]=='@')
            {
                for(int k=0;k<16;k++)
                {
                    if(name[k] == '%' && name[k+1] == '%' && start_flag == 0)
                    {
                        RX_CHK_FLAG_start1 = k;
                        start_flag = 1;
                    }
                    else if(name[k] == '%' && name[k+1] == '%' && start_flag == 1)
                    {
                        RX_CHK_FLAG_start2 = k;
                    }
                    else if(name[k] == '@' && name[k+1] == '@' && end_flag == 0)
                    {
                        RX_CHK_FLAG_end1 = k;
                        end_flag = 1;
                        COPY_FLAG=1;
                        break;
                    }
                    else if(name[k] == '@' && name[k+1] == '@' && end_flag == 1)
                    {
                        RX_CHK_FLAG_end2 = k;
                        COPY_FLAG=2;
                        break;
                    }
                }
            }
            else
            {
                __delay_ms(10);     TX1REG='P';     __delay_ms(1);      TX1REG='K';     __delay_ms(1);   
                i=0;                RC1STAbits.SPEN=0;                RC1STAbits.SPEN=1;               
                for(int k = 0; k< 15; k++)
                {
                    name[k] = '#';
                }
//                TX1REG=' ';//                while(PIR1bits.TXIF==0);
//                TX1REG=' ';//                while(PIR1bits.TXIF==0);   
                COPY_FLAG=0;                    
            }
       
                        
            if(COPY_FLAG==1)            // %% sw10 Sw1 state speed10s speed1s childLok ackFlag res res res res parity @ @
            {
                copy_frame(RX_CHK_FLAG_start1,RX_CHK_FLAG_end1);
                // FORMAT -----> ACTION(switch_num_bit_10s, Switch_Num_bit_1s, sw_status, speed_bit1, speed_bit2, parent, finalFrameStat )
                ACTION(copy[2], copy[3], copy[4], copy[5], copy[6], copy[7],copy[8]);
            }
            else if(COPY_FLAG==2)
            {     
                copy_frame(RX_CHK_FLAG_start2,RX_CHK_FLAG_end2);    
                ACTION(copy[2], copy[3], copy[4], copy[5], copy[6], copy[7],copy[8]);
            }        
       }// end of if(TX_FLAG==1)


/*manual Response started */
	   
			if(child_LOCK[1]==OFF && SW1==OFF && M1==OFF)
			{
                if(man==1)
                {
                st[1]='R';      st[2]='0';      st[3]='0';      st[4]='1';      writeUART(st+1);   FAN_FLAG1=0 ;     RELAY2=ONN;
                }
				M1=1;                man=1;
	        }

	   
			if(child_LOCK[1]==OFF && SW1==ONN && M1==ONN)
			{
                
                if(man==1)
                {
                st[1]='R';      st[2]='1';      st[3]='0';      st[4]='1';      writeUART(st+1);	FAN_FLAG1=0 ;	 RELAY2=OFF;
                }
				M1=0;                man=1;
			}
	  
			if(child_LOCK[3]==OFF && SW2==OFF && M2==OFF)
			{  
                if(man==1)
                {
                st[5]='R';      st[6]='0';      st[7]='0';      st[8]='2';      writeUART(st+5);FAN_FLAG2=0 ;      RELAY3=1;
				}
                M2=1;                man=1;
			}
	   
			if(child_LOCK[3]==OFF && SW2==ONN && M2==ONN)
			{       
                if(man==1)
                {
                st[5]='R';      st[6]='1';      st[7]='0';      st[8]='2';      writeUART(st+5);  FAN_FLAG2=0 ;  RELAY3=0;   
				}
                M2=0;                man=1;
			}
	   
			if(child_LOCK[5]==OFF && SW3==OFF && M3==OFF)
			{
                if(man==1)
                {
                st[9]='R';     st[10]='0';     st[11]='0';     st[12]='3';      writeUART(st+9); FAN_FLAG3=0 ;
				RELAY4=1; 
                }
                M3=1;
                man=1;
			}
	   
			if( child_LOCK[5]==OFF && SW3==ONN && M3==ONN)
			{ 
                if(man==1)
                {
                st[9]='R';        st[10]='1';       st[11]='0';       st[12]='3';     writeUART(st+9);   FAN_FLAG3=0 ;
				RELAY4=0;
                }
                M3=0;
                man=1;
            }
    }
    
}

void copy_frame(int start, int end){    
    int k=0,sw=0;
    for(k = start; k< end; k++)
    {
        copy[sw]=name[k];
        sw++;
        name[k] = '#';
    }
//    TX1REG=' ';//    while(PIR1bits.TXIF==0);//    TX1REG=' ';//    while(PIR1bits.TXIF==0);   
    COPY_FLAG=0;    
}

void ACTION(char Switch_Num_10s, char Switch_Num_1s, char sw_status, char speed_bitHigh, char speed_bitLow, char parent,char finalFrameStat)
{
    int response_starts=0;
    int switch_status=0;
    int SwNum=0;
   
    if(Switch_Num_1s != 'T')        // %% sw10 Sw1 state speed10s speed1s childLok ackFlag res res res res parity @ @
    {
    unsigned char FanSpeedString[2], SwNumString[2];
    int FanSpeed=0;
    
    switch_status=sw_status - '0';
       
    SwNumString[0]=Switch_Num_10s;
    SwNumString[1]=Switch_Num_1s;
    SwNum=atoi(SwNumString);
    
    FanSpeedString[0] = speed_bitHigh;
    FanSpeedString[1] = speed_bitLow;
    FanSpeed = atoi(FanSpeedString);
    
    int children=parent - '0';    
    int child_lock_num=(2*(SwNum)-1); // position of child_locked frame
    child_LOCK[child_lock_num]=children;

    response_starts=((1+4*(SwNum))-4);
    st[response_starts++]='G';
    st[response_starts++]=sw_status;
    st[response_starts++]=Switch_Num_10s;
    st[response_starts]=Switch_Num_1s;    
    
    response_starts-=3;
    if(finalFrameStat=='1')
    {
    writeUART(st+response_starts);
    }
    man=0;
    }   
    
    
    
    switch(SwNum)       // char SwNum
    {
        case 1:
            FAN_FLAG1 = switch_status;
            if(switch_status==0)
            {
                PIE1bits.CCP1IE = 0;
                RELAY2 =(switch_status^1);
                pk=0;
            }
            else if(switch_status==1)
            {
                PIE1bits.CCP1IE = 1;
                dimmer1=speed_bitHigh;
                pk=1;
                FAN_SPEED1_10s = speed_bitHigh;
                FAN_SPEED1_1s= speed_bitLow;
//                  RELAY4 = 0;
            }
            M3=switch_status;
            break;
        case 2:
            FAN_FLAG2 = switch_status;
            if(switch_status==0)
            {
                PIE3bits.CCP3IE = 0;
                RELAY3 =(switch_status^1);
                ak=0;
            }
            else if(switch_status==1)   
            {
                PIE3bits.CCP3IE = 1;
                dimmer2=speed_bitHigh;
                ak=1;
                FAN_SPEED2_10s = speed_bitHigh;
                FAN_SPEED2_1s= speed_bitLow;
                 //RELAY3 = 0;
             }
            M4=switch_status;
           
            break;           
        case 3:
            FAN_FLAG3 = switch_status;
            if(switch_status==0)
            {
                tk=0;
                PIE4bits.CCP9IE = 0;
                RELAY4 =(switch_status^1);
            }
            else if(switch_status==1)
            {
                PIE4bits.CCP9IE = 1;
                dimmer3=speed_bitHigh;
                tk=1;
                FAN_SPEED3_10s = speed_bitHigh;
                FAN_SPEED3_1s= speed_bitLow;
//                  RELAY2 = 0;
            }
            M5=switch_status;
            break; 
        default:
            break;
    }
    
}
             
void writeUART(char *str2Write)
{
    int Tx_count=0;
 	//while (*str2Write != '\0' || Tx_count!=4)
  	while(Tx_count!=4)
 	{
        //while(PIR1bits.TXIF==0); 
        while (!TX1STAbits.TRMT);
 		TX1REG = *str2Write;
 		*str2Write++;
        Tx_count++;
 	}
}
  
void TMR1_Initialize(void)
{
    //Set the Timer to the options selected in the GUI

    //T1CKPS 1:1; nT1SYNC synchronize; TMR1CS FOSC/4; TMR1ON enabled;
    T1CON = 0x00;

    //T1GSS T1G; TMR1GE disabled; T1GTM disabled; T1GPOL low; T1GGO_nDONE done; T1GSPM disabled;
    T1GCON = 0x00;

        //TMR1H 29;
    TMR1H = 0x00;

    //TMR1L 112;
    TMR1L = 0x00;

    // Clearing IF flag before enabling the interrupt.
    PIR1bits.TMR1IF = 0;

    // Enabling TMR1 interrupt.
    PIE1bits.TMR1IE = 1;

    // Start TMR1
   // T1CONbits.TMR1ON = 1;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

//     Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;
}

void TMR3_Initialize(void)
{
    //Set the Timer to the options selected in the GUI

    //T1CKPS 1:1; nT1SYNC synchronize; TMR1CS FOSC/4; TMR1ON enabled;
    T3CON = 0x00;

    //T1GSS T1G; TMR1GE disabled; T1GTM disabled; T1GPOL low; T1GGO_nDONE done; T1GSPM disabled;
    T3GCON = 0x00;

        //TMR1H 29;
    TMR3H = 0x00;
 
    //TMR1L 112;
    TMR3L = 0x00;

    // Clearing IF flag before enabling the interrupt.
    PIR3bits.TMR3IF = 0;

    // Enabling TMR1 interrupt.
    PIE3bits.TMR3IE = 1;

    // Start TMR1
   // T1CONbits.TMR1ON = 1;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
//    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
//    PEIE = 1;
}

void TMR5_Initialize(void)
{
    //Set the Timer to the options selected in the GUI

    //T5CKPS 1:1; T5OSCEN disabled; nT5SYNC synchronize; TMR5CS FOSC/4; TMR5ON off; 
    T5CON = 0x00;

    //T5GSS T5G; TMR5GE disabled; T5GTM disabled; T5GPOL low; T5GGO_nDONE done; T5GSPM disabled; 
    T5GCON = 0x00;

    //TMR5H 123; 
    TMR5H = 0x00;

    //TMR5L 48; 
    TMR5L = 0x00;

    // Clearing IF flag.
    PIR3bits.TMR5IF = 0;    
    
    // Enabling TMR5 interrupt.
    PIE3bits.TMR5IE = 1;
}

void TMR2_Initialize(void)
{
//     Set TMR2 to the options selected in the User Interface

//     T2CKPS 1:1; T2OUTPS 1:1; TMR2ON off; 
    T2CON = 0x08;
//
//     PR2 39; 
//    PR2 = 0x00;
//
//     TMR2 10; 
    TMR2 = 0x00;

//     Clearing IF flag before enabling the interrupt.
    PIR1bits.TMR2IF = 0;

//     Enabling TMR2 interrupt.
    PIE1bits.TMR2IE = 1;
         GIE = 1;

//     Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;
}
//void TMR2_Initialize(void)
//{
//     // Set TMR2 to the options selected in the User Interface
//  //11 = Prescaler is 64
//    // T2CKPS 1:1; T2OUTPS 1:1; TMR2ON off; 
//    T2CON = 0x7B;
//
//    // PR2 39; 
//    PR2 = 0x00;
//
//    // TMR2 10; 
//    TMR2 = 0x00;
//
//    // Clearing IF flag before enabling the interrupt.
//    PIR1bits.TMR2IF = 0;
//
//    // Enabling TMR2 interrupt.
//    PIE1bits.TMR2IE = 1;
//     GIE = 1;
//
//    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
//    PEIE = 1;
//    
//}

void TMR4_Initialize(void)
{
    // Set TMR2 to the options selected in the User Interface

    // T2CKPS 1:2; T2OUTPS 1:1; TMR2ON off; 
    T4CON = 0x08;

    // PR2 39; 
//    PR2 = 0x00;

    // TMR2 10; 
    TMR4 = 0x00;

    // Clearing IF flag before enabling the interrupt.
    PIR3bits.TMR4IF = 0;

    // Enabling TMR2 interrupt.
    PIE3bits.TMR4IE = 1;
}

void TMR6_Initialize(void)
{
    // Set TMR6 to the options selected in the User Interface

    // T6CKPS 1:2; T6OUTPS 1:1; TMR6ON off; 
    T6CON = 0x08;

    // PR6 39; 
//    PR6 = 0x27;

    // TMR6 0; 
    TMR6 = 0x00;

    // Clearing IF flag before enabling the interrupt.
    PIR3bits.TMR6IF = 0;

    // Enabling TMR6 interrupt.
    PIE3bits.TMR6IE = 1;
}

void CCP1_Initialize()
{
    // Set the CCP1 to the options selected in the User Interface

    // MODE Every edge; EN enabled; FMT right_aligned;
    CCP1CON = 0x04;
 //   CCP1CON = 0x84;

    // RH 0;
    CCPR1H = 0x00;

    // RL 0;
    CCPR1L = 0x00;

    // Clear the CCP1 interrupt flag
    PIR1bits.CCP1IF = 0;

    // Enable the CCP1 interrupt
    PIE1bits.CCP1IE = 1;
   GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;
}

void CCP3_Initialize(void)
{
    // Set the CCP3 to the options selected in the User Interface

    // MODE Every edge; EN enabled; FMT right_aligned;
    CCP3CON = 0x84;    

    // CCPR3L 0; 
    CCPR3L = 0x00;    

    // CCPR3H 0; 
    CCPR3H = 0x00;    
    
    // Selecting Timer 3
//    CCPTMRS0bits.C3TSEL = 0x1;

    // Clear the CCP3 interrupt flag
    PIR3bits.CCP3IF = 0;

    // Enable the CCP3 interrupt
    PIE3bits.CCP3IE = 0;
}

//void CCP5_Initialize(void)
//{
//    // Set the CCP5 to the options selected in the User Interface
//
//    // MODE Every edge; EN enabled; FMT right_aligned;
//    CCP5CON = 0x84;    
//
//    // CCPR5L 0; 
//    CCPR5L = 0x00;    
//
//    // CCPR5H 0; 
//    CCPR5H = 0x00;    
//    
//    // Selecting Timer 5
////    CCPTMRS1bits.C5TSEL = 0x1;
//
//    // Clear the CCP5 interrupt flag
//    PIR3bits.CCP5IF = 0;
//
//    // Enable the CCP5 interrupt
//    PIE3bits.CCP5IE = 1;
//}

void CCP9_Initialize()
{
    // Set the CCP1 to the options selected in the User Interface

    // MODE Every falling edge; EN enabled; FMT right_aligned;
    CCP9CON = 0x84;

    // RH 0;
    CCPR9H = 0x00;

    // RL 0;
    CCPR9L = 0x00;

    // Clear the CCP1 interrupt flag
    PIR4bits.CCP9IF = 0;

    // Enable the CCP1 interrupt
    PIE4bits.CCP9IE = 0;
}

void EUSART_Initialize()
{
    // disable interrupts before changing states
    PIE1bits.RC1IE = 0;
    PIE1bits.TX1IE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE enabled; ABDEN disabled;
    BAUD1CON = 0x0A;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled;
    RC1STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave;
    TX1STA = 0x24;

    // Baud Rate = 9600; SP1BRGL 12;
    //SPBRGL = 0x0C;
    //SPBRGL = 0x19;                  // SP1BRGL is 25 (hex value=0x19) for 9600 baud on 16 MHz crystal frequency
    SP1BRGL = 0xA0;                  // SYNC =0 ; BRGH = 1 ; BRG16=1;
    // Baud Rate = 9600; SP1BRGH 1;
    SP1BRGH = 0x01;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

    // enable receive interrupt
    PIE1bits.RC1IE = 1;                    // handled into INTERRUPT_Initialize()

    // Transmit Enabled
    TX1STAbits.TXEN = 1;

    // Serial Port Enabled
    RC1STAbits.SPEN = 1;
}

void pin_manager()
 {         
     // NEW BOARD
     
     /* PORT G */     
     ANSELG=0x00;
     TRISGbits.TRISG0=1;    //ccp3
//     TRISGbits.TRISG4=1;   //ccp5
     WPUG = 0;     
     
     /* PORT F */
     ANSELF=0x00;
     TRISFbits.TRISF0=0;            // relay 1
     TRISFbits.TRISF1=0;            // relay 2
     TRISFbits.TRISF2=1;            // switch 4
     TRISFbits.TRISF3=1;            // switch 3
//     TRISFbits.TRISF4=1;
     TRISFbits.TRISF5=1;            // switch 2
//     TRISFbits.TRISF6=1;
     TRISFbits.TRISF7=1;            // switch 1
     
     /* PORT E */
     WPUE=0x00;
     ANSELE=0x00;
     TRISEbits.TRISE3=1;               // zcd RE3 input capture mode ccp9
     TRISEbits.TRISE5=0;               // pwm RE5 output
     
     /* PORT D */  
//     WPUD=0x00;
//     ANSELD=0x00;
//     TRISD=0xFF;
//     
     /* PORT C */
//     TRISCbits.TRISC0=0;
//     TRISCbits.TRISC1=0;
//     
     /* PORT B */
//     ANSELB=0x00;
//     TRISBbits.TRISB1=0;
//     TRISBbits.TRISB3=0;
//     WPUB = 0x00;
     
     /* PORT A */
     ANSELA = 0x00;
//     TRISAbits.TRISA0=0;
//     TRISAbits.TRISA1=0;
     TRISAbits.TRISA2=0;            // relay 4
     TRISAbits.TRISA3=0;            // relay 3
//     TRISAbits.TRISA4=1;
     TRISAbits.TRISA5=1;            // switch 5
     
    
      /* CCP1 */
     // PORT C - RC2 
    TRISCbits.TRISC2 = 1;              // CCP1
     
    // uart pin initialization
    TRISCbits.TRISC6 = 0;               // Tx pin = output
    TRISCbits.TRISC7 = 1;               // Rx pin = input
}


