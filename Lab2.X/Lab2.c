#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
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

#include <xc.h>
#include <stdint.h> //incluimos 
#include "ADC.h"
#define _XTAL_FREQ 4000000

int contador;
uint8_t i;
int valorold;
int valorold2;
int var;
uint8_t alarma;
char change=0;
int display2[16] ={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0X39, 0X5E, 0X79, 0X71};
int d1;
int d2;

void __interrupt() ISR(void){
    //Interrupcion puerto B
     //Bandera PORTB
    INTCONbits.T0IE=0;
    
    if(PORTBbits.RB7==1 && INTCONbits.RBIF==1){ 
        INTCONbits.GIE=0;
        INTCONbits.RBIE=0;
        i++;//Incremento +1 contador
        INTCONbits.GIE=1;
        INTCONbits.RBIE=1;
        INTCONbits.RBIF=0;
    }
    if(PORTBbits.RB6==1 && INTCONbits.RBIF==1){
        INTCONbits.GIE=0;
        INTCONbits.RBIE=0;  
        i--;//Incremento -1 contador
        INTCONbits.GIE=1;
        INTCONbits.RBIE=1;
        INTCONbits.RBIF=0;
    }
    //Interrupcion ADC 
    if (PIR1bits.ADIF==1){
        INTCONbits.GIE=0;
        INTCONbits.RBIE=0;
        //PORTA toma los valores del 
        d1 = display2[ADRESH/16];
        d2 =display2[ADRESH%16];
        alarma=ADRESH;   
        
    }
    
    if(INTCONbits.T0IF == 1){ // interrupción del timer0
        TMR0 = 200; //Define el valor precagado del timer0
        change = 1; //Activa el cambio de transistor
        
    }
    
    INTCONbits.T0IE=1;
    INTCONbits.T0IF=0;
    PIR1bits.ADIF=0; 
    
    
}
void main(void) {

ANSEL=0;
ANSELH=0;
ANSELHbits.ANS8 =1;//Entrada analogica RB2

TRISA=0;
TRISBbits.TRISB2=1; //RB2 as input
TRISBbits.TRISB7=1; //RB7 as input
TRISBbits.TRISB6=1; //RB6 as input 
TRISD=0;
TRISC=0;

PORTB=0;
PORTA=0;
PORTC=0;
PORTD=0;

ADC_conf(0);
/*
//Encender interrupcion ADC
PIE1bits.ADIE=1;
PIR1bits.ADIF=1;
//Configuracion ADC
ADCON0bits.ADCS=01;
//Selecion del canal analogico
ADCON0bits.CHS0=0;
ADCON0bits.CHS1=0;
ADCON0bits.CHS2=0;
ADCON0bits.CHS3=1;
ADCON0bits.GO_nDONE=0;
//Activacion del ADC
ADCON0bits.ADON=1;
//Tipo de justificado
ADCON1bits.ADFM=0;
//Refencia VSS y VDD
ADCON1bits.VCFG0=0;
ADCON1bits.VCFG1=0;
*/

//Configuracion INT PORTB on change 
INTCON=0;
INTCONbits.GIE=1;
INTCONbits.RBIE=1;
INTCONbits.RBIF=0;

INTCONbits.T0IE=1;
INTCONbits.T0IF=0;

OPTION_REG=0;
OPTION_REGbits.PS0=0;
OPTION_REGbits.PS1=0;
OPTION_REGbits.PS2=1;

TMR0=200;

IOCBbits.IOCB7=1; //RB7 y RB6 INT activadas
IOCBbits.IOCB6=1;
i=0;

while(1){
    if (alarma > i){
        PORTDbits.RD2=1;
    }
    else{
        PORTDbits.RD2=0;
    }
    __delay_ms(10);
    ADCON0bits.GO_DONE=1;
    __delay_ms(10);
    if (change == 1){
        if(PORTDbits.RD1==1){
            PORTDbits.RD0=0;
            PORTDbits.RD1=0;
            PORTC=display2[ADRESH/16];
            PORTDbits.RD0=1;
            }
        else{
            PORTDbits.RD0=0;
            PORTDbits.RD1=0;
            PORTC=display2[ADRESH%16];
            PORTDbits.RD1=1;
        }
        change=0;
    }
    PORTA=i;
    //PIR1bits.ADIF=0;
    }
    return;
}   
