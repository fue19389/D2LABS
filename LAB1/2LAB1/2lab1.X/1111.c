//File: 2LAB1
//Dispositivo: PIC16F887
//Autor: Gerardo Fuentes
//Compilador: XC8, MPLABX V5.45

//Programa: Librerias, ADC, T0, RB
//Hardware: Leds, transistores, 7segmentos
/*------------------------------------------------------------------------------
                        BITS DE CONFIGURACIÓN
------------------------------------------------------------------------------*/
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/*------------------------------------------------------------------------------
 LIBRERIAS
----------------------------------------------------------------------------- */ 
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "adc_to_7seg.h"  //Implementación primera librería

/*------------------------------------------------------------------------------
DIRECTIVAS DEL PROCESADOR
------------------------------------------------------------------------------*/
#define _XTAL_FREQ 4000000

/*------------------------------------------------------------------------------
DECLARACIONES Y VARIABLES
------------------------------------------------------------------------------*/
void cfg_io();
void cfg_clk();
void cfg_inte();
void cfg_iocb();
void cfg_adc();
void cfg_t0();
void int_t0();
void int_iocb();
void int_adc();

char tab7seg[16]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x77,0x7c,
0x39,0x5E,0x79,0x71};// Tabla 7seg para hex y decimal
char cont;
unsigned char udisp = 0; // Posición de la primera cifra
unsigned char ddisp = 0; // Posición de la segunda cifra
void t7();               // Función para implementar librerías



/*------------------------------------------------------------------------------
INTERRUPCIONES
------------------------------------------------------------------------------*/
void __interrupt() isr(void){

    if (INTCONbits.RBIF){     //Revisión bandera RB
        int_iocb();
    }
    if (PIR1bits.ADIF){       //Revisión bandera ADC
        int_adc();
    }
    if (INTCONbits.T0IF){     //Revisión bandera T0
        int_t0();
    }
}

void int_iocb(){              //Interrupción de pull ups

    if (PORTBbits.RB0 == 0){  //Incrementar con B0
        PORTC++;
    }
    if (PORTBbits.RB1 == 0){  //Decrementar con B1
        PORTC--;
    }
    INTCONbits.RBIF = 0;      //Clear de bandera RB
}

void int_adc(){               //Interrupción ADC
    if(ADCON0bits.CHS == 5){  //Primer ciclo de ADC (ADRESH A PUERTO)
        adcdig = ADRESH;
    }
    PIR1bits.ADIF = 0;        //Clear de bandera ADC
}

void int_t0(){
    PORTA = 0X00;             //Clear del puertof 
    if (cont == 0X00){        //Primer ciclo display 7seg unidades
        PORTA = udisp;
        PORTD = 0x04;         //Enable del display 7seg unidades
        cont++;               //Incrementar variable de ciclo
    }
    else if (cont == 0X01){   //Segundo ciclo display 7seg decenas
        PORTA = ddisp;
        PORTD = 0x08;         //Enable del display 7seg decenas
        cont = 0x00;          //Incrementar variable de ciclo
    }
 
    TMR0 = 254;               //N para el TIMER0 
    INTCONbits.T0IF = 0;      //Clear la bandera de TIMER0    
}

/*------------------------------------------------------------------------------
MAIN
------------------------------------------------------------------------------*/
void main () {                
    cfg_io();                 //Llamado a configuraciones
    cfg_clk();
    cfg_inte();
    cfg_iocb();
    cfg_adc();
    cfg_t0();   
    ADCON0bits.GO = 1;        //Inicio externo del loop del ADC
/*------------------------------------------------------------------------------
LOOP PRINCIPAL
------------------------------------------------------------------------------*/
    while(1){                    //Loop principal   
        t7();                    //Call para conversión a HEX
        
        if(adcdig >= PORTC){     //Alarma si HEX supera contador
            PORTDbits.RD0 = 1;
        }
        
        if(ADCON0bits.GO == 0){  //Proceso al acabar conversión
            __delay_us(100);     //Delay para no traslapar conversiones
            ADCON0bits.GO = 1;
         }
    }
}
/*------------------------------------------------------------------------------
FUNCIONES
------------------------------------------------------------------------------*/
void t7(void){                   //Transformación ADC a 7seg
    hex();                       //Función creada en librerías
    udisp = tab7seg[num0];       //Display
    ddisp = tab7seg[num1];         
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
  
    OPTION_REGbits.nRBPU =  0 ; //Se habilita el pull up interno en PORTB
    WPUB = 0x03;                //Pull ups para los pines RB0 y RB1
    
    PORTB = 0x00;   // CLEAR de los puertos
    PORTC = 0x00;
    PORTA = 0x00;
    PORTE = 0x00;
  
}
void cfg_clk(){
    OSCCONbits.IRCF2 = 0; // IRCF = 011 (500kHz) 
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS = 1;   //Reloj interno habilitado
    
    return;
}
void cfg_inte(){  
    INTCONbits.GIE = 1;  // Habilitar interrupciones globales
    INTCONbits.T0IE = 1; // Habilitar interrupción de t0
    INTCONbits.RBIE = 1; // Habilitar interrupción de B
    INTCONbits.RBIF = 0; // Clear en bandera de B
    INTCONbits.T0IF = 0; // Clear en bandera de t0
    
    return;
}  
void cfg_iocb(){
    IOCB = 0X03 ;        // Habilitar PORTB 0 y 1 para interrupción
    INTCONbits.RBIF = 0; // Clear de la bandera B
    
    return;   
}

void cfg_t0(){
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS2 = 1;  // PS 111 = 256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
    
    TMR0 = 254;              // N de t0 para 5ms
    INTCONbits.T0IF = 0;
            
    return;
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