/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                               Exemplo 7                                 *
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

// ESTE SOFTWARE ESTÁ PREPARADO PARA LER QUATRO BOTÕES E TOCAR O BUZZER
// COM DURAÇÃO VARIÁVEL CONFORME TECLA PRESSIONADA.

// Observação: ligar a chave EN-LED e BUZZER

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINIÇÃO PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <timers.h>             //Timer library functions

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
unsigned char filtro1;
unsigned char filtro2;
unsigned char filtro3;
unsigned char filtro4;

unsigned char status_leds = 0;            // atualiza leds conforme o botão pressionado

unsigned rom char tabela[16] = {255,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.
#define     MIN         0
#define     MAX         15
#define     t_filtro    25

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declaração dos flags de software                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de flags ajuda na programação e economiza memória RAM.
//Este programa não utiliza nenhum flag de usuário

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Definição e inicialização dos port's                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUNÇÕES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TRATA_HIGH_INT(void);
void TRATA_INT_TIMER0(void);
void TRATA_INT_TIMER2(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

#define     botao1  PORTDbits.RD4   //PORTA DO BOTÃO S1
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     botao2  PORTDbits.RD5   //PORTA DO BOTÃO S5
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     botao3  PORTDbits.RD6   //PORTA DO BOTÃO S9
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     botao4  PORTDbits.RD7   //PORTA DO BOTÃO S13
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
    PIR1bits.TMR2IF = 0;     //LIMPA FLAG DE INTERRUPÇÃO

    if (buzzer != 0)         // o buzzer está ligado ?
    {
        buzzer = 0;          // sim, então desliga
    }
    else
    {
        buzzer = 1;          // não, então liga
    }
}

void TRATA_INT_TIMER0(void)
{
    INTCONbits.TMR0IF = 0;   //LIMPA FLAG DE INTERRUPÇÃO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 1                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    if(botao1)               // testa botão 1
    {                        // botão 1 está pressionado ?
        filtro1--;           // Sim, então decrementa o filtro
        if(filtro1 == 0)     // acabou o filtro do botão 1 ?
        {
            status_leds = status_leds | 0b00000001; // marca que o botão está pressionado
        }
    }
    else
    {                        // botão 1 liberado
        filtro1 = t_filtro;  // inicia o filtro do botão 1
        status_leds = status_leds & 0b11111110;     // marca que o botão foi liberado
    }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 2                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    if(botao2)               // testa botão 2
    {                        // botão 2 está pressionado ?
        filtro2--;           // Sim, então decrementa o filtro
        if(filtro2 == 0)     // fim do filtro do botão 2 ?
        {
            status_leds = status_leds | 0b00000010; // marca que o botão está pressionado
        }
    }
    else
    {                        // botão 2 liberado
        filtro2 = t_filtro;  // inicia o filtro do botão 2
        status_leds = status_leds & 0b11111101;    // marca que o botão foi liberado
    }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 3                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    if(botao3)               // testa botão 3
    {                        // botão 3 está pressionado ?
        filtro3--;           // Sim, então decrementa o filtro
        if(filtro3 == 0)     // fim do filtro do botão 3 ?
        {
            status_leds = status_leds | 0b00000100; // marca que o botão está pressionado
        }
    }
    else
    {                        // botão 3 liberado
        filtro3 = t_filtro;  // inicia o filtro do botão 3
        status_leds = status_leds & 0b11111011;     // marca que o botão foi liberado
    }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 4                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    if(botao4)               // testa botão 4
    {                        // botão 4 está pressionado ?
        filtro4--;           // Sim, então decrementa o filtro
        if(filtro4 == 0)     // fim do filtro do botão 4 ?
        {
            status_leds = status_leds | 0b00001000; // marca que o botão está pressionado
        }
    }
    else
    {                        // botão 4 liberado
        filtro4 = t_filtro;  // inicia o filtro do botão 4
        status_leds = status_leds & 0b11110111; // marca que o botão foi liberado
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
    TRISC = 0b11111101;         //CONFIG DIREÇÃO DOS PINOS PORTC
    TRISD = 0b11110000;         //CONFIG DIREÇÃO DOS PINOS PORTD
    TRISE = 0b00000111;         //CONFIG DIREÇÃO DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
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
    INTCONbits.PEIE = 1;       // liga chave de periféricos
    INTCONbits.GIE = 1;        // liga chave geral

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
    {
        ClrWdt();
    }
}   // FIM DO PROGRAMA

