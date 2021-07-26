/*
 * File:   main.c
 * Author: Pablo
 * Ejemplo de uso de la LCD 16x2 en modo 4 bits
 * Se utiliz� y se adaptaron las librer�as de Ligo George 
 * de la p�gina www.electrosome.com
 * Enlace: https://electrosome.com/lcd-pic-mplab-xc8/
 * Created on 31 de enero de 2020, 11:20 AM
 */
/*------------------------------------------------------------------------------
                        BITS DE CONFIGURACIÓN
------------------------------------------------------------------------------*/
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = EXTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/*------------------------------------------------------------------------------
 LIBRERIAS
----------------------------------------------------------------------------- */ 
#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "LCD.h"
#include "adc.h"

/*------------------------------------------------------------------------------
DIRECTIVAS DEL PROCESADOR
------------------------------------------------------------------------------*/
#define _XTAL_FREQ 8000000

/*------------------------------------------------------------------------------
DECLARACIONES Y VARIABLES
------------------------------------------------------------------------------*/
#define RS RC6
#define EN RC7
#define D4 RD4
#define D5 RB5
#define D6 RD6
#define D7 RD7

void cfg_io();
void cfg_adc();
void int_adc();
void cfg_usart();
void send_crct(char st[]);
void send_char(char dato);

double conv(uint8_t aa);

uint8_t V1;
uint8_t V2;

char f[10];
double v11;
double v22;
double g = 0;
char val = 0;

/*------------------------------------------------------------------------------
INTERRUPCIONES
------------------------------------------------------------------------------*/

void __interrupt() isr(void){

    if (PIR1bits.ADIF){       //Revisión bandera ADC
        int_adc();
    }
}

void int_adc(){               //Interrupción ADC
    if(ADCON0bits.CHS == 5){  //Primer ciclo de ADC (ADRESH A PUERTO)
        V1 = valadc(val);
        ADCON0bits.CHS = 6;
        }   
    else{                     //Segundo ciclo de ADC (ADRESH A PUERTO)
        V2 = valadc(val);
        ADCON0bits.CHS = 5;
        }   
    PIR1bits.ADIF = 0;        //Clear de bandera ADC
    
}

/*------------------------------------------------------------------------------
MAIN
------------------------------------------------------------------------------*/

void main(void) {
  cfg_io();                 //Llamado a configuraciones 
  cfg_adc();
  unsigned int a;
  TRISD = 0x00;
  TRISCbits.TRISC6 = 0;
  TRISCbits.TRISC7 = 0;
  Lcd_Init();
  ADCON0bits.GO = 1;
  
  while(1){
      
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("S1: S2: S3:");
    v11 = conv(V1);
    v22 = conv(V2);
    
    Lcd_Set_Cursor(2,1);
    sprintf(f, "%3.2fV",v11);
    Lcd_Write_String(f);

    
   
    TXREG = '\f';
    send_crct("HOLA");
    
    __delay_ms(2000);

    if(ADCON0bits.GO == 0){  //Proceso al acabar conversión
            __delay_us(100);     //Delay para no traslapar conversiones
            ADCON0bits.GO = 1;
         }
    return;
}
}

/*------------------------------------------------------------------------------
CONFIG GENERAL
------------------------------------------------------------------------------*/
void cfg_io(){
    ANSELH = 0x00;  //Seteo de inputs como digitales
    ANSEL = 0x20;   //Seteo de inputs RE0 y RE1 como analógicos
    
    TRISB = 0x03;   //Pines RB0, RB1, como entradas
    TRISC = 0x00;   //PORTC, PORTA, PORTD como salidas
    TRISA = 0X00;
    TRISD = 0X00;
    TRISE = 0x03;   //Entradas RE0 y RE1
}

void cfg_adc() {            //Configuración ADC
    ADCON1bits.ADFM = 0;    //Justificar a la izquierda
    ADCON1bits.VCFG0 = 0;   //Voltaje de referencia Vss y Vdd
    ADCON1bits.VCFG1 = 0;   
    
    ADCON0bits.ADCS0 = 0;   //ADC clock Fosc/2 para 1Mhz
    ADCON0bits.ADCS1 = 0;   
    ADCON0bits.CHS = 5;     //Canal 5 selecionado para inicar
    __delay_us(100);        //Delay más largo para tiempo necesario de conver.
    ADCON0bits.ADON = 1;    //Encender módulo ADC
    
}

double conv(uint8_t aa ){
    double result;
    result = aa*0.0196;
    return result;
}

void cfg_usart(){
        //Configuración de reloj
    OSCCONbits.IRCF = 0b100; //IRCF = 100 (1MHz) 
    OSCCONbits.SCS = 1;   //Reloj interno habilitado
    
    //Config TX Y RX
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    
    BAUDCTLbits.BRG16 = 1;
    
    SPBRG = 51;
    SPBRGH = 0;
    
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;
    
    TXSTAbits.TXEN = 1;
            
}

/*------------------------------------------------------------------------------
FUNCIONES
------------------------------------------------------------------------------*/
void send_crct(char st[]){
    int i = 0;              //Se declara una variable que recorrera la cadena
    while (st[i] != 0){     //Mientras st no sea igual a 0
        send_char(st[i]);  //Se enviara el caracter por caracter
        i++;                //Se aumenta en 1 el caracter a mostrar en la cadena
    }
}

void send_char(char dato){
    while(!TXIF);           //Mientras la bandera de transmisión sea 0
    TXREG = dato;           //Se envía el caracter
}