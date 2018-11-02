/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                               Exemplo 4                                 *
 *                                                                         *
 *                               MOSAICO                                   *
 *                                                                         *
 *  TEL: (0XX11) 4992-8775               SITE: www.mosaico.com.br          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   VERSÃO : 1.0                                                          *
 *   DATA : 10/05/2011                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             Descrição geral                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// SOFTWARE QUE UTILIZA DOIS BOTÕES PARA INCREMENTAR E DECREMENTAR UM
// CONTADOR. O VALOR É MOSTRADO NO DISPLAY DE 7 SEGMENTOS.

// Observação: ligar a chave EN-LED e DS-U

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINIÇÃO PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <delays.h>             //Delay library functions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Configurações para gravação                   *
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
 *              Definição e inicialização das variáveis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco estão definidas as variáveis globais do programa.
unsigned char CONTADOR; //ARMAZENA O VALOR DA CONTAGEM
unsigned char FILTRO11; //FILTRAGEM  1 PARA O BOTÃO 1
unsigned char FILTRO21; //FILTRAGEM  1 PARA O BOTÃO 2

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.
#define     MIN         0
#define     MAX         15
#define     T_FILTRO    5

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declaração dos flags de software                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de flags ajuda na programação e economiza memória RAM.
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

#define    ST_BT1    FLAGSbits.BIT0        //STATUS DO BOTÃO 1
#define    ST_BT2    FLAGSbits.BIT1        //STATUS DO BOTÃO 2

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Definição e inicialização dos port's                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUNÇÕES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

#define     BT1     PORTDbits.RD4   //PORTA DO BOTÃO S1
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT2     PORTDbits.RD5   //PORTA DO BOTÃO S5
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SAÍDAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//As saídas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

#define     COLUNA_1    PORTDbits.RD0   //PINO PARA ATIVAR COLUNA 1 DO TECLADO
                                        //MATRICIAL
                                        //1 -> COLUNA ATIVADA
                                        //0 -> COLUNA DESATIVADA
#define     DISPLAY_1   PORTAbits.RA2   //PINO PARA ATIVAR O DISPLAY 1

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *                    ROTINA DE CONVERSÃO BINÁRIO -> DISPLAY               *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ESTA ROTINA IRÁ RETORNAR EM W, O SIMBOLO CORRETO QUE DEVE SER
// MOSTRADO NO DISPLAY PARA CADA VALOR DE INTENSIDADE. O RETORNO JÁ ESTÁ
// FORMATADO PARA AS CONDIÇÕES DE LIGAÇÃO DO DISPLAY AO PORTD.
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
{//   .GFEDCBA      POSIÇÃO CORRETA DOS SEGMENTOS
    0b00111111,     // 00 - RETORNA SÍMBOLO CORRETO 0
    0b00000110,     // 01 - RETORNA SÍMBOLO CORRETO 1
    0b01011011,     // 02 - RETORNA SÍMBOLO CORRETO 2
    0b01001111,     // 03 - RETORNA SÍMBOLO CORRETO 3
    0b01100110,     // 04 - RETORNA SÍMBOLO CORRETO 4
    0b01101101,     // 05 - RETORNA SÍMBOLO CORRETO 5
    0b01111101,     // 06 - RETORNA SÍMBOLO CORRETO 6
    0b00000111,     // 07 - RETORNA SÍMBOLO CORRETO 7
    0b01111111,     // 08 - RETORNA SÍMBOLO CORRETO 8
    0b01101111,     // 09 - RETORNA SÍMBOLO CORRETO 9
    0b01110111,     // 10 - RETORNA SÍMBOLO CORRETO A
    0b01111100,     // 11 - RETORNA SÍMBOLO CORRETO b
    0b00111001,     // 12 - RETORNA SÍMBOLO CORRETO C
    0b01011110,     // 13 - RETORNA SÍMBOLO CORRETO d
    0b01111001,     // 14 - RETORNA SÍMBOLO CORRETO E
    0b01110001};    // 15 - RETORNA SÍMBOLO CORRETO F

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Função Principal                         *
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

    TRISA = 0b11000011;         //CONFIG DIREÇÃO DOS PINOS PORTA
    TRISB = 0b00000000;         //CONFIG DIREÇÃO DOS PINOS PORTB
    TRISC = 0b11111111;         //CONFIG DIREÇÃO DOS PINOS PORTC
    TRISD = 0b00000000;         //CONFIG DIREÇÃO DOS PINOS PORTD
    TRISE = 0b00000111;         //CONFIG DIREÇÃO DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    COLUNA_1 = 1;               //ATIVA COLUNA 1 DO TECLADO MATRICIAL
    
    DISPLAY_1 = 1;              //ATIVA O DISPLAY 1

    ST_BT1 = 0;
    ST_BT2 = 0;
    CONTADOR = MIN;                  //INICIA CONTADOR = MIN
    PORTD = CONVERTE[CONTADOR];      //ATUALIZA O DISPLAY INICIALMENTE

    FILTRO11 = 0;
    FILTRO21 = 0;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    while(1)
    {
        ClrWdt();
        Delay1KTCYx(1);         //delay de 1ms
        
        DISPLAY_1 = 0;          //DESATIVA O DISPLAY 1
        TRISD = 0b11110000;     //CONFIGURA O PORTD PARA O TECLADO
        
        Nop();                  //TEMPO DE 4 CICLOS DE MÁQUINA PARA ESTABILIZAÇÃO DA
        Nop();                  //ENTRADA
        Nop();
        Nop();
        
        PORTD = 1;               //ATIVA COLUNA 1 DO TECLADO MATRICIAL
        
        if(BT1)
        {
            if(--FILTRO11 == 0)
            {
                if(CONTADOR != MIN)
                {
                    if(!ST_BT1)
                    {
                        CONTADOR--;
                        ST_BT1 = 1;
                    }
                }
            }
        }
        else
        {
            ST_BT1 = 0; // BOTÃO LIBERADO, LIMPA O FLAG
        }

        if(BT2)
        {
            if(--FILTRO21 == 0)
            {
                if(CONTADOR != MAX)
                {
                    if(!ST_BT2)
                    {
                        CONTADOR++;
                        ST_BT2 = 1;
                    }
                }
            }
        }
        else
        {
            ST_BT2 = 0; // BOTÃO LIBERADO, LIMPA O FLAG
        }
        
        TRISD = 0b00000000;         //CONFIGURA O PORTD PARA O DISPLAY DE 7 SEGMENTOS
        DISPLAY_1 = 1;              //ATIVA O DISPLAY 1

        PORTD = CONVERTE[CONTADOR]; //ESCREVE O VALOR NO PORTD
    }
}   // FIM DO PROGRAMA

