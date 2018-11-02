/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de C�digo para utiliza��o com a McLab3             *
 *                               Exemplo 5                                 *
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

// TIMER DECRESCENTE EM SEGUNDOS. O VALOR INICIAL � DETERMINADO PELA CONSTANTE
// V_INICIO E PODE ESTAR ENTRE 1 E 15 SEGUNDOS. OS BOT�ES ATIVOS S�O O DA
// LINHA 4. O BOT�O DA COLUNA 1 DISPARA O TIMER, MOSTRANDO O TEMPO RESTANTE
// NO DISPLAY. O DA COLUNA 2 PARALIZA O TIMER. O LED (RB0) � UTILIZADO PARA
// INDICAR O ESTADO ATUAL DO TIMER: ACESO=RODANDO E APAGADO=PARADO

// Observa��o: ligar a chave EN-LED e DS-U

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
unsigned char TEMPO;        //ARMAZENA O VALOR DO TEMPO
unsigned char FILTRO11;     //FILTRAGEM  1 PARA O BOT�O 1
unsigned char FILTRO21;     //FILTRAGEM  1 PARA O BOT�O 2
unsigned char TEMP1;        //REGISTRADORES AUXILIARES

union
{
    struct
    {
        unsigned BIT0:1;
        unsigned BIT1:1;
        unsigned BIT2:1;
        unsigned BIT3:1;
        unsigned BIT4:1;
        unsigned BIT5:1;
        unsigned BIT6:1;
        unsigned BIT7:1;
    };
    
    struct
    {
        unsigned BITS;
    };
}botoes;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.
#define V_INICIO    15
#define T_FILTRO    255

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declara��o dos flags de software                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de flags ajuda na programa��o e economiza mem�ria RAM.
struct
{
    unsigned BIT0:1;
    unsigned BIT1:1;
    unsigned BIT2:1;
    unsigned BIT3:1;
    unsigned BIT4:1;
    unsigned BIT5:1;
    unsigned BIT6:1;
    unsigned BIT7:1;
}FLAGSbits;            //ARMAZENA OS FLAGS DE CONTROLE

#define    ST_BT1    FLAGSbits.BIT0        //STATUS DO BOT�O 1
#define    ST_BT2    FLAGSbits.BIT1        //STATUS DO BOT�O 2
#define    F_FIM     FLAGSbits.BIT2        //FLAG DE FIM DE TEMPO
#define    HABILITA  FLAGSbits.BIT3        //FLAG DE FIM DE TEMPO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Defini��o e inicializa��o dos port's                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUN��ES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void ATUALIZA(void);
void TRATA_INT_TIMER0(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

#define     BT1     botoes.BIT4     //PORTA DO BOT�O S1
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT2     botoes.BIT5     //PORTA DO BOT�O S5
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

#define     DISPLAY_1   PORTAbits.RA2   //PINO PARA ATIVAR O DISPLAY 1

#define     LED         LATBbits.LATB0  //DEFINE PINO DO LED
                                        //0 -> LED APAGADO
                                        //1 -> LED ACESO

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *                  ROTINA DE TRATAMENTO DE INT DE TIMER0                  *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#pragma code TIMER0_interrupt = 0x0008
void TIMER0_int(void)
{
    _asm goto TRATA_INT_TIMER0 _endasm
}
#pragma code

#pragma interrupt TRATA_INT_TIMER0
void TRATA_INT_TIMER0(void)
{
    INTCONbits.TMR0IF = 0;      //LIMPA FLAG DE INTERRUP��O
    WriteTimer0(256 - 125);
    
    // faz a leitura das teclas
    DISPLAY_1 = 0;              //DESATIVA O DISPLAY 1
    TRISD = 0b11110000;         //CONFIGURA O PORTD PARA O TECLADO
    
    Nop();                      //TEMPO DE 4 CICLOS DE M�QUINA PARA ESTABILIZA��O DA
    Nop();                      //ENTRADA
    Nop();
    Nop();
    
    PORTD = 1;
    
    botoes.BITS = PORTD;
    TRISD = 0b00000000;         //CONFIGURA O PORTD PARA O DISPLAY DE 7 SEGMENTOS
    DISPLAY_1 = 1;              //ATIVA O DISPLAY 1
    ATUALIZA();

    // temporiza��o de 1 segundo
    
    if( HABILITA == 1 )
    {
        TEMP1--;
        if (TEMP1 == 0)
        {
            TEMP1 = 125;
            if(F_FIM == 1)
            return;
            TEMPO--;
            if(TEMPO == 0) F_FIM = 1;
        }  
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *                    ROTINA DE CONVERS�O BIN�RIO -> DISPLAY               *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ESTA ROTINA IR� RETORNAR EM W, O SIMBOLO CORRETO QUE DEVE SER
// MOSTRADO NO DISPLAY PARA CADA VALOR DE INTENSIDADE. O RETORNO J� EST�
// FORMATADO PARA AS CONDI��ES DE LIGA��O DO DISPLAY AO PORTD.
//          a
//     **********
//     *        *
//   f *        * b
//     *    g   *
//     **********
//     *        *
//   e *        * c
//     *    d   *
//     **********  *.

const rom unsigned char CONVERTE[16] =
{//   .GFEDCBA      POSI��O CORRETA DOS SEGMENTOS
    0b00111111,     // 00 - RETORNA S�MBOLO CORRETO 0
    0b00000110,     // 01 - RETORNA S�MBOLO CORRETO 1
    0b01011011,     // 02 - RETORNA S�MBOLO CORRETO 2
    0b01001111,     // 03 - RETORNA S�MBOLO CORRETO 3
    0b01100110,     // 04 - RETORNA S�MBOLO CORRETO 4
    0b01101101,     // 05 - RETORNA S�MBOLO CORRETO 5
    0b01111101,     // 06 - RETORNA S�MBOLO CORRETO 6
    0b00000111,     // 07 - RETORNA S�MBOLO CORRETO 7
    0b01111111,     // 08 - RETORNA S�MBOLO CORRETO 8
    0b01101111,     // 09 - RETORNA S�MBOLO CORRETO 9
    0b01110111,     // 10 - RETORNA S�MBOLO CORRETO A
    0b01111100,     // 11 - RETORNA S�MBOLO CORRETO b
    0b00111001,     // 12 - RETORNA S�MBOLO CORRETO C
    0b01011110,     // 13 - RETORNA S�MBOLO CORRETO d
    0b01111001,     // 14 - RETORNA S�MBOLO CORRETO E
    0b01110001};    // 15 - RETORNA S�MBOLO CORRETO F


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ROTINA DE ATUALIZA��O DO DISPLAY                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// ESTA ROTINA CONVERTE O VALOR DE TEMPO ATRAV�S DA ROTINA CONVERTE
// E ATUALIZA O PORTB PARA ACENDER O DISPLAY CORRETAMENTE

void ATUALIZA(void)
{
    PORTD = CONVERTE[TEMPO];     //CONVERTE CONTADOR NO N�MERO DO
                                //DISPLAY E ATUALIZA O PORTD PARA
                                //VISUALIZARMOS O VALOR DE CONTADOR
                                //NO DISPLAY
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ROTINA DE DESLIGAR O TIMER                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// ESTA ROTINA EXECUTA AS A��ES NECESS�RIAS PARA DESLIGAR O TIMER

void DESL_TIMER(void)
{
    HABILITA = 0;                      //DESLIGA TIMER
    LED = 0;                           //APAGA O LED
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       ROTINA DE LIGAR O TIMER                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// ESTA ROTINA EXECUTA AS A��ES NECESS�RIAS PARA LIGAR O TIMER

void LIGA_TIMER(void)
{
    if(HABILITA == 0)                  //TIMER J� ESTA LIGADO?
    {
        INTCONbits.TMR0IF = 0;             //LIMPA FLAG DE INTERRUP��O
        WriteTimer0(256 - 125);            //INICIA TIMER0 E TEMP1
        TEMP1 = 125;
        HABILITA = 1;                      //LIGA TIMER
        LED = 1;                           //LIGA O LED
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
    TRISC = 0b11111111;         //CONFIG DIRE��O DOS PINOS PORTC
    TRISD = 0b00000000;         //CONFIG DIRE��O DOS PINOS PORTD
    TRISE = 0b00000111;         //CONFIG DIRE��O DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    OpenTimer0(TIMER_INT_ON & T0_8BIT & T0_SOURCE_INT & T0_EDGE_FALL & T0_PS_1_64);
    
    RCON = 0x00;
    INTCONbits.PEIE = 1;        //LIGA A INTERRUP��O
    INTCONbits.GIE = 1;    

    COLUNA_1 = 1;               //ATIVA COLUNA 1 DO TECLADO MATRICIAL
    
    DISPLAY_1 = 1;              //ATIVA O DISPLAY 1
    
    ST_BT1 = 0;                 //LIMPA FLAGS
    ST_BT2 = 0;
    F_FIM = 0;
    HABILITA = 0;

    TEMPO = V_INICIO;           //INICIA TEMPO
    LATD = CONVERTE[TEMPO];     //ATUALIZA O DISPLAY INICIALMENTE

    FILTRO11 = 0;
    FILTRO21 = 0;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
    {
        ClrWdt();

        if(BT1)
        {
            if (--FILTRO11 == 0)
            {
                if (!ST_BT1)
                {
                    LIGA_TIMER();
                    ST_BT1 = 1;
                }
            }
        }
        else
        {
            ST_BT1 = 0; // BOT�O LIBERADO, LIMPA O FLAG
        }

        if(BT2)
        {
            if(--FILTRO21 == 0)
            {
                if(!ST_BT2)
                {
                    DESL_TIMER();
                    ST_BT2 = 1;
                }
            }
        }
        else
        {
            ST_BT2 = 0; // BOT�O LIBERADO, LIMPA O FLAG
        }
    }
}   // FIM DO PROGRAMA

