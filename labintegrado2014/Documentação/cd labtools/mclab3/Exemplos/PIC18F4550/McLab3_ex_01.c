/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de C�digo para utiliza��o com a McLab3             *
 *                               Exemplo 1                                 *
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

// Este software est� preparado para demonstrar o controle dos pinos de I/O    
// este primeiro programa demonstrar� o estado de um bot�o por meio de um led.

// Observa��o: ligar a chave EN-LED

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINI��O PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUN��ES DE PERIF�RICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Configura��es para grava��o                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma config FOSC = HS
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
//Este programa n�o utiliza nenhuma vari�vel de usu�rio

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.
//Este programa n�o utiliza nenhuma constante de usu�rio

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

#define BOTAO PORTDbits.RD4     //PORTA DO BOT�O S1
                                //1 -> PRESSIONADO
                                //0 -> LIBERADO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SA�DAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//As sa�das devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

#define     LED         PORTBbits.RB0   //PORTA DO LED
                                        //0 -> APAGADO
                                        //1 -> ACESO

#define     COLUNA_1    PORTDbits.RD0   //PINO PARA ATIVAR COLUNA 1 DO TECLADO
                                        //MATRICIAL
                                        //1 -> COLUNA ATIVADA
                                        //0 -> COLUNA DESATIVADA

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Fun��o Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void main(void)
{
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

    TRISA = 0b11000011;         //CONFIG DIRE��O DOS PINOS PORTA
    TRISB = 0b00000000;         //CONFIG DIRE��O DOS PINOS PORTB
    TRISC = 0b11111111;         //CONFIG DIRE��O DOS PINOS PORTC
    TRISD = 0b11110000;         //CONFIG DIRE��O DOS PINOS PORTD
    TRISE = 0b00000111;         //CONFIG DIRE��O DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    COLUNA_1 = 1;                //ATIVA COLUNA 1 DO TECLADO MATRICIAL

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
    {
        ClrWdt();
        if(BOTAO) LED = 1;  // testa bot�o. Se bot�o = 0, ent�o led = 1
        else LED = 0;       // caso contr�rio, led = 0
    }
}   // FIM DO PROGRAMA

