/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de C�digo para utiliza��o com a McLab3             *
 *                               Exemplo 7                                 *
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

// ESTE SOFTWARE EST� PREPARADO PARA LER QUATRO BOT�ES E TOCAR O BUZZER
// COM DURA��O VARI�VEL CONFORME TECLA PRESSIONADA.

// Observa��o: ligar a chave EN-LED e BUZZER

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
unsigned char filtro1;
unsigned char filtro2;
unsigned char filtro3;
unsigned char filtro4;

unsigned char status_leds = 0;            // atualiza leds conforme o bot�o pressionado

unsigned rom char tabela[16] = {255,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.
#define     MIN         0
#define     MAX         15
#define     t_filtro    25

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
void TRATA_INT_TIMER0(void);
void TRATA_INT_TIMER2(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

#define     botao1  PORTDbits.RD4   //PORTA DO BOT�O S1
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     botao2  PORTDbits.RD5   //PORTA DO BOT�O S5
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     botao3  PORTDbits.RD6   //PORTA DO BOT�O S9
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     botao4  PORTDbits.RD7   //PORTA DO BOT�O S13
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SA�DAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//As sa�das devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

#define     COLUNA_1    PORTDbits.RD0   //PINO PARA ATIVAR COLUNA 1 DO TECLADO
                                        //MATRICIAL
                                        //1 -> COLUNA ATIVADA
                                        //0 -> COLUNA DESATIVADA

#define     buzzer      LATCbits.LATC1

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *             ROTINA DE TRATAMENTO DE INT DE ALTA PRIORIDADE              *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#pragma code VETOR_HIGH_PRIORITY = 0x0008
void HIGH_int (void)
{
    _asm goto TRATA_HIGH_INT _endasm
}
#pragma code

#pragma interrupt TRATA_HIGH_INT
void TRATA_HIGH_INT(void)
{
    if(PIR1bits.TMR2IF)      TRATA_INT_TIMER2();
    if(INTCONbits.TMR0IF)    TRATA_INT_TIMER0();
}

void TRATA_INT_TIMER2(void)
{
    PIR1bits.TMR2IF = 0;     //LIMPA FLAG DE INTERRUP��O

    if (buzzer != 0)         // o buzzer est� ligado ?
    {
        buzzer = 0;          // sim, ent�o desliga
    }
    else
    {
        buzzer = 1;          // n�o, ent�o liga
    }
}

void TRATA_INT_TIMER0(void)
{
    INTCONbits.TMR0IF = 0;   //LIMPA FLAG DE INTERRUP��O

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 1                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    if(botao1)               // testa bot�o 1
    {                        // bot�o 1 est� pressionado ?
        filtro1--;           // Sim, ent�o decrementa o filtro
        if(filtro1 == 0)     // acabou o filtro do bot�o 1 ?
        {
            status_leds = status_leds | 0b00000001; // marca que o bot�o est� pressionado
        }
    }
    else
    {                        // bot�o 1 liberado
        filtro1 = t_filtro;  // inicia o filtro do bot�o 1
        status_leds = status_leds & 0b11111110;     // marca que o bot�o foi liberado
    }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 2                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    if(botao2)               // testa bot�o 2
    {                        // bot�o 2 est� pressionado ?
        filtro2--;           // Sim, ent�o decrementa o filtro
        if(filtro2 == 0)     // fim do filtro do bot�o 2 ?
        {
            status_leds = status_leds | 0b00000010; // marca que o bot�o est� pressionado
        }
    }
    else
    {                        // bot�o 2 liberado
        filtro2 = t_filtro;  // inicia o filtro do bot�o 2
        status_leds = status_leds & 0b11111101;    // marca que o bot�o foi liberado
    }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 3                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    if(botao3)               // testa bot�o 3
    {                        // bot�o 3 est� pressionado ?
        filtro3--;           // Sim, ent�o decrementa o filtro
        if(filtro3 == 0)     // fim do filtro do bot�o 3 ?
        {
            status_leds = status_leds | 0b00000100; // marca que o bot�o est� pressionado
        }
    }
    else
    {                        // bot�o 3 liberado
        filtro3 = t_filtro;  // inicia o filtro do bot�o 3
        status_leds = status_leds & 0b11111011;     // marca que o bot�o foi liberado
    }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 4                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    if(botao4)               // testa bot�o 4
    {                        // bot�o 4 est� pressionado ?
        filtro4--;           // Sim, ent�o decrementa o filtro
        if(filtro4 == 0)     // fim do filtro do bot�o 4 ?
        {
            status_leds = status_leds | 0b00001000; // marca que o bot�o est� pressionado
        }
    }
    else
    {                        // bot�o 4 liberado
        filtro4 = t_filtro;  // inicia o filtro do bot�o 4
        status_leds = status_leds & 0b11110111; // marca que o bot�o foi liberado
    }

    PORTB = status_leds;


    if(status_leds == 0)
    {
        PR2 = 0xFF;
        CloseTimer2();
        buzzer = 0;
    }
    else
    {
        PR2 = tabela[status_leds];    // consulta tabela e inicializa timer2.
        OpenTimer2(TIMER_INT_ON & T2_PS_1_1 & T2_POST_1_3);
    }
}

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
    TRISC = 0b11111101;         //CONFIG DIRE��O DOS PINOS PORTC
    TRISD = 0b11110000;         //CONFIG DIRE��O DOS PINOS PORTD
    TRISE = 0b00000111;         //CONFIG DIRE��O DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    OpenTimer0(TIMER_INT_ON & T0_8BIT & T0_SOURCE_INT & T0_PS_1_4);

    PR2 = 0x21;
    OpenTimer2(TIMER_INT_ON & T2_PS_1_1 & T2_POST_1_3);

    RCON = 0x00;

    COLUNA_1 = 1;               //ATIVA COLUNA 1 DO TECLADO MATRICIAL

    status_leds = 0;

    filtro1 = t_filtro;        // inicia filtro do botao1
    filtro2 = t_filtro;        // inicia filtro do botao2
    filtro3 = t_filtro;        // inicia filtro do botao3
    filtro4 = t_filtro;        // inicia filtro do botao4

    INTCONbits.TMR0IE = 1;     // liga chave da int. do timer 0
    PIE1bits.TMR2IE = 1;       // liga chave da int. do timer 2
    INTCONbits.PEIE = 1;       // liga chave de perif�ricos
    INTCONbits.GIE = 1;        // liga chave geral

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
    {
        ClrWdt();
    }
}   // FIM DO PROGRAMA

