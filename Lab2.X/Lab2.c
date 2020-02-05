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
#define _XTAL_FREQ 4000000

int contador;
uint8_t i;
int valorold;
int valorold2;
int var;
uint8_t alarma;
int display[9] =  {0b00000000, //contiene la secuencia de 8 bits
                   0b00000001, 
                   0b00000011,
                   0b00000111, //para encenderse en el puerto y que muestre el  
                   0b00001111, // progreso del player 1
                   0b00011111, // 
                   0b00111111,
                   0b01111111,
                   0b11111111,
                   };

int display2[11] ={0b0111111,
                   0b0000110,
                   0b1011011,
                   0b1001111,
                   0b1100110,
                   0b1101101,
                   0b1111101,
                   0b0000111,
                   0b1111111,
                   0b1101111,
                   0b1110111,
                   };

void __interrupt() ISR(void){
    //Interrupcion puerto B
    if (INTCONbits.RBIF==1){ //Bandera PORTB
        if(PORTBbits.RB7==1){ 
            INTCONbits.GIE=0;
            INTCONbits.RBIE=0;
            i++;//Incremento +1 contador
            INTCONbits.GIE=1;
            INTCONbits.RBIE=1;
            INTCONbits.RBIF=0;
        }
        if(PORTBbits.RB6==1){
            INTCONbits.GIE=0;
            INTCONbits.RBIE=0;
            i--;//Incremento -1 contador
            INTCONbits.GIE=1;
            INTCONbits.RBIE=1;
            INTCONbits.RBIF=0;  
        }
    INTCONbits.RBIF=0;
    alarma=ADRESH;
    }        
    //Interrupcion ADC
    
    if (PIR1bits.ADIF==1){
        INTCONbits.GIE=0;
        INTCONbits.RBIE=0;
        PORTA=ADRESH;//PORTA toma los valores del ADC
        alarma=ADRESH;
        INTCONbits.GIE=1;
        INTCONbits.RBIE=1;
        PIR1bits.ADIF=0;   
    }
    //Interrupcion TMR0
    if (TMR0IF){
        //Enter your code here
        TMR0IF = 0;
        TMR0 = 4;
        contador++;     // Se incrementa la variable de contador cada 0.5 mS
    }
    
    
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
TRISDbits.TRISD2=0; //RB0 as output
TRISC=0;


PORTB=0;
PORTA=0;
PORTC=0;
PORTD=0;

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

//Configuracion INT PORTB on change 
INTCON=0;
INTCONbits.GIE=1;
INTCONbits.RBIE=1;
INTCONbits.RBIF=0;
IOCBbits.IOCB7=1; //RB7 y RB6 INT activadas
IOCBbits.IOCB6=1;
i=0;

while(1){
    if (alarma > i){
        PORTDbits.RD2=1;
        __delay_ms(100);
    }
    else{
        PORTDbits.RD2=0;
    }
    PORTC=i;
    __delay_ms(10);
    ADCON0bits.GO_DONE=1;
    __delay_ms(10);
    //PIR1bits.ADIF=0;
    PORTA=ADRESH;
    }
    return;
}   
