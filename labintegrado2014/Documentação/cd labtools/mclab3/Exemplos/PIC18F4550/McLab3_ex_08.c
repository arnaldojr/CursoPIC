/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                               Exemplo 8                                 *
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

// ESTE EXEMPLO FOI PREPARADO PARA DEMOSNTRAR O FUNCIONAMENTO DO TIMER DE
// 16 BITS DO PIC (TMR1) E DA VARREDURA DE DISPLAYS MAIS BARRA DE LEDS.
// CONSISTE NUM CONTADOR DE SEGUNDOS. DOIS BOTÕES FORAM UTILIZADOS PARA
// PROGRAMAR O TEMPO DA CONTAGEM. UM OUTRO BOTÃO FOI UTILIZADO PARA DISPARAR
// O CONTADOR. O TEMPORIZADOR CONSEGUE CONTAR ATÉ 9999 SEGUNDOS, DE FORMA QUE
// OS 4 DISPLAYS DE 7 SEGMENTOS FORAM NECESSÁRIOS. A CONTAGEM É REGRESSIVA.
// UMA BARRA DE LEDS INDICA QUE O TEMPORIZADOR ESTÁ OPERANDO. QUANDO O SISTEMA
// CHEGA A 0000 A BARRA DE LEDS É DESLIGADA AUTOMATICAMENTE.

// BOTÕES DA COLUNA 4 -> LINHA 1 (S1) - (INCREMENTA)
//                       LINHA 2 (S5) - (DECREMENTA)
//                       LINHA 3 (S9) - (LIGA CONTAGEM)
//                       LINHA 4 (S13) - (DESLIGA CONTAGEM)

// Observação: ligar a chave EN-LED, DS-U, DS-DZ, DS-CEN, DS-MI

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
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.
#define t_filtro        240
#define turbo_tecla     2
#define delta_timer1    (65536 - 62500)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Definição e inicialização das variáveis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco estão definidas as variáveis globais do programa.

unsigned char display = 0;          // atualiza os displays
unsigned char contador = 2;         // contador de interrupções de timer1
unsigned char filtro  = t_filtro;   // inicia filtro dos botões
unsigned char filtro1 = t_filtro;   // inicia filtro dos botões
unsigned char filtro2 = t_filtro;   // inicia filtro dos botões
unsigned char filtro3 = t_filtro;   // inicia filtro dos botões
unsigned char filtro4 = t_filtro;   // inicia filtro dos botões
unsigned char turbo = 1;            // inicia turbo das teclas
unsigned char unidade = 9;          // variável unidade do timer de 1 seg
unsigned char dezena  = 9;          // variável dezena do timer de 1 seg
unsigned char centena = 9;          // variável centena do timer de 1 seg
unsigned char milhar  = 9;          // variável milhar do timer de 1 seg

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

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *                    TABELA DE CONVERSÃO BINÁRIO -> DISPLAY               *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ESTA ROTINA IRÁ RETORNAR EM W, O SIMBOLO CORRETO QUE DEVE SER
// MOSTRADO NO DISPLAY PARA CADA VALOR DE INTENSIDADE. O RETORNO JÁ ESTÁ
// FORMATADO PARA AS CONDIÇÕES DE LIGAÇÃO DO DISPLAY AO PORTD.
//       a
//     **********
//     *        *
//   f *        * b
//     *    g   *
//     **********
//     *        *
//   e *        * c
//     *    d   *
//     **********  *.

const rom unsigned char converte[] =
{//  .GFEDCBA     POSIÇÃO CORRETA DOS SEGMENTOS
   0b00111111,    // 00 - RETORNA SÍMBOLO CORRETO 0
   0b00000110,    // 01 - RETORNA SÍMBOLO CORRETO 1
   0b01011011,    // 02 - RETORNA SÍMBOLO CORRETO 2
   0b01001111,    // 03 - RETORNA SÍMBOLO CORRETO 3
   0b01100110,    // 04 - RETORNA SÍMBOLO CORRETO 4
   0b01101101,    // 05 - RETORNA SÍMBOLO CORRETO 5
   0b01111101,    // 06 - RETORNA SÍMBOLO CORRETO 6
   0b00000111,    // 07 - RETORNA SÍMBOLO CORRETO 7
   0b01111111,    // 08 - RETORNA SÍMBOLO CORRETO 8
   0b01101111,    // 09 - RETORNA SÍMBOLO CORRETO 9
   0b00000000};   // BLANK

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
void TRATA_INT_TIMER1(void);
void decrementa_timer(void);
void incrementa_timer(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SAÍDAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMAÇÃO E
//FUTURAS ALTERAÇÕES DO HARDWARE.

#define     BT_UP       botoes.BIT4     //PORTA DO BOTÃO S1
                                        //1 -> PRESSIONADO
                                        //0 -> LIBERADO

#define     BT_DOWN     botoes.BIT5     //PORTA DO BOTÃO S5
                                        //1 -> PRESSIONADO
                                        //0 -> LIBERADO

#define     BT_START    botoes.BIT6     //PORTA DO BOTÃO S9
                                        //1 -> PRESSIONADO
                                        //0 -> LIBERADO

#define     BT_STOP     botoes.BIT7     //PORTA DO BOTÃO S13
                                        //1 -> PRESSIONADO
                                        //0 -> LIBERADO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SAÍDAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SAÍDAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMAÇÃO E
//FUTURAS ALTERAÇÕES DO HARDWARE.

#define     ESTADO_TIMER    LATBbits.LATB0  //LED INDICATIVO DE TIMER LIGADO
#define     COLUNA_1    PORTDbits.RD0       //PINO PARA ATIVAR COLUNA 1 DO TECLADO
                                            //MATRICIAL
                                            //1 -> COLUNA ATIVADA
                                            //0 -> COLUNA DESATIVADA
#define     disp0           LATAbits.LATA5  // seleção do display milhar  (3)
#define     disp1           LATAbits.LATA4  // seleção do display centena (2)
#define     disp2           LATAbits.LATA3  // seleção do display dezena  (1)
#define     disp3           LATAbits.LATA2  // seleção do display unidade (0)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  Função de decremento do Timer                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void decrementa_timer(void)
{
    unidade--;              //DECREMENTA UNIDADE
    if (unidade == 0xFF)    //UNIDADE = 255?
    {
        unidade = 9;        //UNIDADE = 9
        dezena--;           //DECREMENTA DEZENA
    }
    if (dezena == 0xFF)     //DEZENA = 255?
    {
        dezena = 9;         //DEZENA = 9
        centena--;          //DECREMENTA CENTENA
    }
    if (centena == 0xFF)    //CENTENA = 255?
    {
        centena = 9;        //CENTENA = 9
        milhar--;           //DECREMENTA MILHAR
    }
    if (milhar == 0xFF)     //MILHAR = 255?
    {
        milhar = 9;         //MILHAR = 9
    }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  Função de incremento do Timer                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void incrementa_timer(void)
{
    unidade++;              //INCREMENTA UNIDADE
    if (unidade == 10)      //UNIDADE = 10?
    {
        unidade = 0;        //UNIDADE = 0
        dezena++;           //INCREMENTA DEZENA
    }
    if (dezena == 10)       //DEZENA = 10?
    {
        dezena = 0;         //DEZENA = 0
        centena++;          //INCREMENTA CENTENA
    }
    if (centena == 10)      //CENTENA = 10?
    {
        centena = 0;        //CENTENA = 0
        milhar++;           //INCREMENTA MILHAR
    }
    if (milhar == 10)       //MILHAR = 10?
    {
        milhar = 0;         //MILHAR = 0
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Função de tratamento de interrupção de Timer1          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// Esta interrupcao ocorrera a cada 0,5 segundos.

void TRATA_INT_TIMER1(void)
{
    PIR1bits.TMR1IF = 0;            // limpa flag de interrupção
    WriteTimer1(delta_timer1);      // carraga timer1
    contador--;                     // decrementa contador de interrupções
    if(contador == 0)
    {
        contador = 2;               // carrega contador de int
        decrementa_timer();
        if ((unidade == 0)&&(dezena == 0)&&(centena == 0)&&(milhar == 0))// timer está zerado?
        {
            ESTADO_TIMER = 0;       // sim, apaga o led e
            CloseTimer1();          // desliga timer1
        }
    }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Rotina de Tratamento de interrupção de TMR0            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// Esta interrupção ocorrerá a cada 256us.

void TRATA_INT_TIMER0(void)
{
    INTCONbits.TMR0IF = 0;  //LIMPA FLAG DE INTERRUPÇÃO
    
    //desliga os displys
    disp0 = 0;
    disp1 = 0;
    disp2 = 0;
    disp3 = 0;
    
    TRISD = 0b11110000;         //CONFIGURA O PORTD PARA O TECLADO
    
    Nop();                      //TEMPO DE 4 CICLOS DE MÁQUINA PARA ESTABILIZAÇÃO DA
    Nop();                      //ENTRADA
    Nop();
    Nop();
    COLUNA_1 = 1;
    botoes.BITS = PORTD;
    TRISD = 0b00000000;         //CONFIGURA O PORTD PARA O DISPLAY DE 7 SEGMENTOS
    
    switch(display)         // início da varredura dos display´s
    {
        case 0:
        display++;          // incrementa a variável de varredura
        PORTD = converte[milhar];   // atualiza o portd
        disp0 = 1;          // liga o display 0
        break;              // sai

        case 1:
        display++;          // incrementa a variável de varredura
        PORTD = converte[centena];  // atualiza o portd
        disp1 = 1;          // liga o display 1
        break;              // sai

        case 2:
        display++;          // incrementa a variável de varredura
        PORTD = converte[dezena];   // atualiza o portd
        disp2 = 1;          // liga o display 2
        break;              // sai

        case 3:
        display = 0;        // inicia em 0 a variável de varredura
        PORTD = converte[unidade];  // atualiza o portd
        disp3 = 1;          // liga o display 3
        break;              // sai
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
    TRISC = 0b11111111;         //CONFIG DIREÇÃO DOS PINOS PORTC
    TRISD = 0b00000000;         //CONFIG DIREÇÃO DOS PINOS PORTD
    TRISE = 0b00000111;         //CONFIG DIREÇÃO DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

    OpenTimer0(TIMER_INT_ON & T0_8BIT & T0_SOURCE_INT & T0_PS_1_2);
    //CONFIGURAÇÃO DO TIMER0
    OpenTimer1(T1_OSC1EN_OFF & TIMER_INT_OFF & T1_SOURCE_INT & T1_8BIT_RW & T1_PS_1_8);
    //CONFIGURAÇÃO DO TIMER1

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    while(RCONbits.NOT_TO); //AGUARDA ESTOURO DO WDT

    INTCONbits.PEIE = 1;    //LIGA CHAVE DOS PERIFÉRICOS
    INTCONbits.GIE = 1;     //LIGA A CHAVE GERAL
    
    ESTADO_TIMER = 0;       //LIMPA ESTADO_TIMER
    CloseTimer1();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
    {
        ClrWdt();           //LIMPA O WDT
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 1                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
        if(BT_UP)                           // testa botão 1
        {                                   // botão 1 está pressionado ?
            if(ESTADO_TIMER == 0)           // o timer está parado?
            {
                filtro1 --;                 // decrementa o filtro
                if(filtro1 == 0)            // fim do filtro do botão?
                {
                    turbo --;               // decrementa o turbo da tecla
                    if(turbo == 0)          // sim, fim do turbo do botão ?
                    {
                        turbo = turbo_tecla;// carrega o turbo
                        incrementa_timer(); // incrementa o timer
                    }
                }
            }
            continue;
        }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 2                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        if(BT_DOWN)                         // testa botão 2
        {                                   // botão 2 está pressionado ?
            if(ESTADO_TIMER == 0)           // o timer está parado?
            {
                filtro2 --;                 // decrementa o filtro
                if(filtro2 == 0)            // fim do filtro do botão?
                {
                    turbo --;               // decrementa o turbo da tecla
                    if(turbo == 0)          // sim, fim do turbo do botão ?
                    {
                        turbo = turbo_tecla;// carrega o turbo
                        decrementa_timer(); // decrementa o timer
                    }
                }
            }
            continue;
        }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 3                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        if(BT_START)                        // testa botão 3
        {                                   // botão 3 está pressionado ?
            if (ESTADO_TIMER == 0)
            {
                filtro3 --;                 // decrementa o filtro
                if(filtro3 == 0)            // fim do filtro do botão?
                {
                        if((unidade!=0)||(dezena!=0)||(centena!=0)||(milhar!=0))// timer está zerado?
                        {
                            ESTADO_TIMER = 1;           // não, indica que o timer está contando
                            contador = 2;               // carrega contador auxiliar
                            WriteTimer1(delta_timer1);  // carraga timer1
                            PIR1bits.TMR1IF = 0;        // limpa flag de int tmr1
                            OpenTimer1(T1_OSC1EN_OFF & TIMER_INT_ON & T1_SOURCE_INT & T1_8BIT_RW & T1_PS_1_8);
                            // liga o tratamento de interrupção de timer1
                        }
                }
                continue;
            }
        }

        if(BT_STOP)                         // testa botão 4
        {
            ESTADO_TIMER = 0;               // indica timer parado
            CloseTimer1();                  // desliga o tratamento de interrupção de timer1
        }
    }
}   // FIM DO PROGRAMA

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *             ROTINA DE TRATAMENTO DE INT DE ALTA PRIORIDADE              *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma code VETOR_HIGH_PRIORITY = 0x0008    //VETOR DE ALTA PRIORIDADE
void HIGH_int (void)
{
    _asm goto TRATA_HIGH_INT _endasm
}
#pragma code

#pragma interrupt TRATA_HIGH_INT
void TRATA_HIGH_INT(void)
{
    if(INTCONbits.TMR0IF)   TRATA_INT_TIMER0();
    if(PIR1bits.TMR1IF)     TRATA_INT_TIMER1();
}
