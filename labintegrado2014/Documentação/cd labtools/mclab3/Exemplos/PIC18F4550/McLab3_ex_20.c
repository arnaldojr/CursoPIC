/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de C�digo para utiliza��o com a McLab3             *
 *                               Exemplo 20                                *
 *                                                                         *
 *                               MOSAICO                                   *
 *                                                                         *
 *  TEL: (0XX11) 4992-8775               SITE: www.mosaico.com.br          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   VERS�O : 1.0                                                          *
 *   DATA : 10/05/2011                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             Descri��o geral                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ESTE EXEMPLO FOI ELABORADO PARA DEMONSTRAR O FUNCIONAMENTO TECLADO PS2
// OS C�DIGOS DAS TECLAS SE ENCONTRA NO ARQUIVO KEY.PDF, JUNTO COM OS
// EXEMPLOS

// Observa��o: ligar a chave CLK-PS2, DATA-PS2 e EN-LED

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINI��O PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUN��ES DE PERIF�RICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <timers.h>             //Timer library functions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Configura��es para grava��o                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma config FOSC = XTPLL_XT
#pragma config CPUDIV = OSC1_PLL2
#pragma config WDT = ON
#pragma config WDTPS = 128
#pragma config LVP = OFF
#pragma config PWRT = ON
#pragma config BOR = OFF
#pragma config BORV = 0
#pragma config PBADEN = OFF
#pragma config DEBUG = OFF
#pragma config PLLDIV = 1
#pragma config USBDIV = 1
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config VREGEN = OFF
#pragma config MCLRE = ON
#pragma config LPT1OSC = OFF
#pragma config CCP2MX = ON
#pragma config STVREN = OFF
#pragma config ICPRT = OFF
#pragma config XINST = OFF

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Defini��o e inicializa��o das vari�veis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco est�o definidas as vari�veis globais do programa.

enum
{
    PS2START = 0,
    PS2BIT,
    PS2PARITY,
    PS2STOP
} PS2_estado = PS2START;

unsigned char KBD_buffer;
unsigned char KBD_contador;
unsigned char KBD_paridade;

unsigned char KBD_pronto;
unsigned char KBD_codigo;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.

#define TMAX    188     // TIMEOUT DE 500ns PARA REINICIO DA M�QUINA
                        // DE ESTADO DE CAPTURA DA TECLA
                        
#define _A      0x1C    // C�DIGO DA TECLA A
#define _B      0x32    // C�DIGO DA TECLA B
#define SOLTA   0xF0    // C�DIGO DE TECLA SOLTA (VIDE ARQUIVO KEY.PDF)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declara��o dos flags de software                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de flags ajuda na programa��o e economiza mem�ria RAM.
//Este programa n�o utiliza nenhum flag de usu�rio

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Defini��o e inicializa��o dos port's                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUN��ES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TRATA_HIGH_INT(void);
void int_teclado_PS2(void);
void int_timer_2(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

#define PS2DAT  PORTBbits.RB3       // PS2 Data input pin

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SA�DAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//As sa�das devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

#define     RELE1       PORTBbits.RB4   //PORTA DO LED
                                        //0 -> DESLIGADO
                                        //1 -> LIGADO
                                        
#define     RELE2       PORTBbits.RB5   //PORTA DO LED
                                        //0 -> DESLIGADO
                                        //1 -> LIGADO
                                        
#define     LED         PORTBbits.RB0   //PORTA DO LED
                                        //0 -> APAGADO
                                        //1 -> ACESO
                                        
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Fun��o de tratamento de interrup��o externa 2          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void int_teclado_PS2(void)
{
    INTCON3bits.INT2IF = 0;
    
    // REINICIA O TIMER A CADA BORDA DE DESCIDA
    TMR2 = 0;

    switch(PS2_estado)
    {
        default:
        case PS2START:   
            if(!PS2DAT)                 // VERIFICA START BIT
            { 
                KBD_contador = 8;       // INICIALIZA CONTADOR DE BITS
                KBD_paridade = 0;       // INICIA VERIFICADOR DE PARIDADE
                T2CONbits.TMR2ON = 1;   // HABILITA TIMER
                PS2_estado = PS2BIT;    // PR�XIMO ESTADO
            }
            break;
        
        case PS2BIT:
            KBD_buffer >>= 1;           // DESCOLAMENTO PARA CAPTURA DO BIT DE DADO

            if(PS2DAT)                  // PS2DAT = 1?
            {
                KBD_buffer += 0x80;     // MARCA BIT = 1
            }
                
            KBD_paridade ^= KBD_buffer; // ATUALIZA PARIDADE
            
            if(--KBD_contador == 0)     // SE CONTADOR = 0, MUDA DE ESTADO
            {
                PS2_estado = PS2PARITY;
            }    
            break;
        
        case PS2PARITY:
            if(PS2DAT)                  // PS2DAT = 1?
            {
                KBD_paridade ^= 0x80;
            }
                
            if(KBD_paridade & 0x80)    // SE PARIDADE PAR, CONTINUA
            {
                PS2_estado = PS2STOP;
            }    
            else
            {
                PS2_estado = PS2START;
            }    
            break;
        
        case PS2STOP:    
            if(PS2DAT)                  // VERIFICA STOP BIT
            {
                KBD_codigo = KBD_buffer;// SALVA DADO
                KBD_pronto = 1;         // SETA FLAG
                T2CONbits.TMR2ON = 0;   // PARA O TIMER
            }    
            PS2_estado = PS2START;
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Fun��o de tratamento de interrup��o do timer 2         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void int_timer_2(void)
{
    PIR1bits.TMR2IF = 0;
    
    PS2_estado = PS2START;  // REINICIA A M�QUINA DE ESTADO
    T2CONbits.TMR2ON = 0;   // PARA O TIMER
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Fun��o Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void main(void)
{
    unsigned char codigo;
    enum
    {
        AGUARDA = 0,
        TECLA_PRESSIONADA,
        TECLA_LIBERADA
    } testa_codigo = AGUARDA;
    
    PORTA = 0x00;               //Limpa PORTA
    PORTB = 0x00;               //Limpa PORTB
    PORTC = 0x00;               //Limpa PORTC
    PORTD = 0x00;               //Limpa PORTD
    PORTE = 0x00;               //Limpa PORTE

    LATA = 0x00;                //Limpa PORTA
    LATB = 0x00;                //Limpa PORTB
    LATC = 0x00;                //Limpa PORTC
    LATD = 0x00;                //Limpa PORTD
    LATE = 0x00;                //Limpa PORTE

    TRISA = 0b11111111;         //CONFIG DIRE��O DOS PINOS PORTA
    TRISB = 0b11001110;         //CONFIG DIRE��O DOS PINOS PORTB
    TRISC = 0b11111111;         //CONFIG DIRE��O DOS PINOS PORTC
    TRISD = 0b11110000;         //CONFIG DIRE��O DOS PINOS PORTD
    TRISE = 0b00000111;         //CONFIG DIRE��O DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D
    
    PR2 = TMAX;
    TMR2 = 0;
    
    OpenTimer2(TIMER_INT_ON & T2_PS_1_16 & T2_POST_1_2);
    
    INTCON2bits.RBPU = 0;       //HABILITA PULL-UP DO PORTB
    INTCON2bits.INTEDG2 = 0;    //INTERRUP��O EXTERNA 2 NA BORDA DE DESCIDA
    
    INTCON3bits.INT2IE = 1;     //LIGA A INTERRUP��O EXTERNA 2
    PIE1bits.TMR2IE = 1;        //LIGA A INTERRUP��O TIMER 2
    INTCONbits.PEIE = 1;        //LIGA AS INTERRUP��ES
    INTCONbits.GIE = 1;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
    {
        ClrWdt();               // LIMPA O WDT
        
        if(KBD_pronto)          // AGUARDA A CHEGADA DE UM CARACTERE
        {
            codigo = KBD_codigo;// L� O C�DIGO DA TECLA
            KBD_pronto = 0;     // LIMPA O FLAG
        }
        
        switch(testa_codigo)    // TESTA C�DIGO DA TECLA
        {
            case AGUARDA:
                if((codigo == _A)||(codigo == _B))  // SE CHEGOU A OU B
                {                                   // PR�XIMO ESTADO
                    testa_codigo = TECLA_PRESSIONADA;
                    LED = 1;                        // LIGA O LED
                }    
                break;
                
            case TECLA_PRESSIONADA:
                if(codigo == _A)    // TESTA A TECLA PRESSIONADA
                {
                    RELE1 = 1;
                }
                
                if(codigo == _B)    // TESTA A TECLA PRESSIONADA
                {
                    RELE2 = 1;
                }
                
                if(codigo == SOLTA) // TESTA A TECLA PRESSIONADA
                {
                    testa_codigo = TECLA_LIBERADA;
                    RELE1 = 0;      // SE FOR O C�DIGO DE TECLA LIBERADA,
                    RELE2 = 0;      // DESLIGA OS REL�S E PR�XIMO ESTADO
                }  
                break;
                
            case TECLA_LIBERADA:
                if((codigo == _A)||(codigo == _B))
                {                   // TESTA A TECLA PRESSIONADA � A ANTERIOR
                    testa_codigo = AGUARDA; //PR�XIMO ESTADO
                    codigo = 0;     // LIMPA VARI�VEL C�DIDO
                    LED = 0;        // DESLIGA O LED
                }   
                break;
                
            default:
                break;
        }    
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *             ROTINA DE TRATAMENTO DE INT DE ALTA PRIORIDADE              *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma code VETOR_HIGH_PRIORITY = 0x0008
void HIGH_int(void)
{
    _asm goto TRATA_HIGH_INT _endasm
}
#pragma code

#pragma interrupt TRATA_HIGH_INT
void TRATA_HIGH_INT(void)
{
    if(PIR1bits.TMR2IF)
    {
        int_timer_2();
    }    
    
    if(INTCON3bits.INT2IF)
    {
        int_teclado_PS2();
    }    
}   // FIM DO PROGRAMA

