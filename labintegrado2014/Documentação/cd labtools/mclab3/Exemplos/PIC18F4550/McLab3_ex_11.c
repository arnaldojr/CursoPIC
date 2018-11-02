/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                               Exemplo 11                                *
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

// ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DO MÓDULO PWM
// DO PIC18F4550. ELE MONITORA OS QUATRO BOTÕES E CONFORME O BOTÃO SELECIONADO
// APLICA UM VALOR DIFERENTE NO PWM, FAZENDO ASSIM UM CONTROLE SOBRE A
// VELOCIDADE DO VENTILADOR. NO LCD É MOSTRADO O VALOR ATUAL DO DUTY CYCLE.

// Observação: ligar a chave EN-LED e VENT

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINIÇÃO PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <pwm.h>                //PWM library functions
#include <timers.h>             //Timer library functions
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
unsigned char FILTRO11; //FILTRAGEM  1 PARA O BOTÃO 1
unsigned char FILTRO12; //FILTRAGEM  2 PARA O BOTÃO 1
unsigned char FILTRO21; //FILTRAGEM  1 PARA O BOTÃO 2
unsigned char FILTRO22; //FILTRAGEM  2 PARA O BOTÃO 2
unsigned char FILTRO31; //FILTRAGEM  1 PARA O BOTÃO 3
unsigned char FILTRO32; //FILTRAGEM  2 PARA O BOTÃO 3
unsigned char FILTRO41; //FILTRAGEM  1 PARA O BOTÃO 4
unsigned char FILTRO42; //FILTRAGEM  2 PARA O BOTÃO 4

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.
#define T_FILTRO    25

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

#define ST_BT1  FLAGSbits.BIT0  //STATUS DO BOTÃO 1
#define ST_BT2  FLAGSbits.BIT1  //STATUS DO BOTÃO 2
#define ST_BT3  FLAGSbits.BIT2  //STATUS DO BOTÃO 3
#define ST_BT4  FLAGSbits.BIT3  //STATUS DO BOTÃO 4

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Definição e inicialização dos port's                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUNÇÕES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void comando_lcd(unsigned char caracter);
void escreve_lcd(unsigned char caracter);
void limpa_lcd(void);
void inicializa_lcd(void);
void tela_principal(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//As entradas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

#define     BT1     PORTDbits.RD4   //PORTA DO BOTÃO S1
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT2     PORTDbits.RD5   //PORTA DO BOTÃO S5
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT3     PORTDbits.RD6   //PORTA DO BOTÃO S9
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT4     PORTDbits.RD7   //PORTA DO BOTÃO S13
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

#define     rs          PORTEbits.RE0   // via do lcd que sinaliza recepção de dados ou comando
#define     rw          PORTBbits.RB7   // via do lcd que configura leitura ou escrita no barramento
#define     enable      PORTEbits.RE1   // enable do lcd

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Rotina que envia um COMANDO para o LCD               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void comando_lcd(unsigned char caracter)
{
    TRISD = 0b00000000; // configura portd como saída
    rs = 0;             // seleciona o envio de um comando
    PORTD = caracter;   // carrega o PORTD com o caracter
    enable = 1;         // gera pulso no enable
    Delay10TCYx(1);     // espera 10 microsegundos
    enable = 0;         // desce o pino de enable
    TRISD = 0b11110000; // CONFIGURA O PORTD PARA O TECLADO
    Delay10TCYx(4);     // espera mínimo 40 microsegundos
    COLUNA_1 = 1;       // ativa a coluna 1 do teclado
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                Rotina que envia um DADO a ser escrito no LCD            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_lcd(unsigned char caracter)
{
    TRISD = 0b00000000; // configura portd como saída
    rs = 1;             // seleciona o envio de um dado
    PORTD = caracter;   // carrega o PORTD com o caracter
    enable = 1;         // gera pulso no enable
    Delay10TCYx(1);     // espera 10 microsegundos
    enable = 0;         // desce o pino de enable
    TRISD = 0b11110000; // CONFIGURA O PORTD PARA O TECLADO
    Delay10TCYx(4);     // espera mínimo 40 microsegundos
    COLUNA_1 = 1;       // ativa a coluna 1 do teclado
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              Função para limpar o LCD                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void limpa_lcd(void)
{
    comando_lcd(0x01);  // limpa lcd
    Delay1KTCYx(2);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Inicialização do Display de LCD                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void inicializa_lcd(void)
{
    rw = 0;             // pino rw em 0
    comando_lcd(0x30);  // envia comando para inicializar display
    Delay1KTCYx(4);     // espera 4 milisengundos

    comando_lcd(0x30);  // envia comando para inicializar display
    Delay10TCYx(10);    // espera 100 microsengundos

    comando_lcd(0x30);  // envia comando para inicializar display

    comando_lcd(0x38);  // liga o display, sem cursor e sem blink

    limpa_lcd();        // limpa lcd

    comando_lcd(0x0c);  // display sem cursor

    comando_lcd(0x06);  // desloca cursor para a direita
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               Tela Principal                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_principal(void)
{
    limpa_lcd();            // limpa lcd
    comando_lcd(0x86);      // posiciona o cursor na linha 0, coluna 6
    escreve_lcd ('M');      // imprime mensagem no lcd
    escreve_lcd ('C');
    escreve_lcd ('L');
    escreve_lcd ('A');
    escreve_lcd ('B');
    escreve_lcd ('3');

    comando_lcd(0xC4);      // posiciona o cursor na linha 1, coluna 4
    escreve_lcd ('P');      // imprime mensagem no lcd
    escreve_lcd ('W');
    escreve_lcd ('M');
    escreve_lcd (':');
    escreve_lcd (' ');
    escreve_lcd ('O');
    escreve_lcd ('F');
    escreve_lcd ('F');
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Tela Tecla 1                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_tecla_1(void)
{
    comando_lcd(0xC9);      // posiciona o cursor na linha 1, coluna 9
    escreve_lcd ('O');      // imprime mensagem no lcd
    escreve_lcd ('F');
    escreve_lcd ('F');
    escreve_lcd (' ');
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Tela Tecla 2                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_tecla_2(void)
{
    comando_lcd(0xC9);      // posiciona o cursor na linha 1, coluna 9
    escreve_lcd ('5');      // imprime mensagem no lcd
    escreve_lcd ('0');
    escreve_lcd ('%');
    escreve_lcd (' ');
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Tela Tecla 3                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_tecla_3(void)
{
    comando_lcd(0xC9);      // posiciona o cursor na linha 1, coluna 9
    escreve_lcd ('7');      // imprime mensagem no lcd
    escreve_lcd ('5');
    escreve_lcd ('%');
    escreve_lcd (' ');
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Tela Tecla 4                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_tecla_4(void)
{
    comando_lcd(0xC9);      // posiciona o cursor na linha 1, coluna 9
    escreve_lcd ('1');      // imprime mensagem no lcd
    escreve_lcd ('0');
    escreve_lcd ('0');
    escreve_lcd ('%');
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
    TRISD = 0b00000000;         //CONFIG DIREÇÃO DOS PINOS PORTD
    TRISE = 0b00000100;         //CONFIG DIREÇÃO DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

    T2CON = 0x06;
    OpenPWM2(255);

    SetDCPWM2(0);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    COLUNA_1 = 1;               //ATIVA COLUNA 1 DO TECLADO MATRICIAL

    inicializa_lcd();           // configura o lcd
    tela_principal();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
    {
        ClrWdt();

        FILTRO11 = 0;           //BT1
        FILTRO12 = T_FILTRO;    //BT1

        FILTRO21 = 0;           //BT2
        FILTRO22 = T_FILTRO;    //BT2

        FILTRO31 = 0;           //BT3
        FILTRO32 = T_FILTRO;    //BT3

        FILTRO41 = 0;           //BT4
        FILTRO42 = T_FILTRO;    //BT4

        while(BT1)
        {
            ClrWdt();
            if(--FILTRO11 == 0)
            {
                FILTRO12--;
                if(FILTRO12 == 0)
                {
                    if(!ST_BT1)
                    {
                        tela_tecla_1();
                        SetDCPWM2(0);
                        ST_BT1 = 1;
                    }
                }
            }
        }
        ST_BT1 = 0; // BOTÃO LIBERADO, LIMPA O FLAG

        while(BT2)
        {
            ClrWdt();
            if(--FILTRO21 == 0)
            {
                FILTRO22--;
                if(FILTRO22 == 0)
                {
                    if (!ST_BT2)
                    {
                        tela_tecla_2();
                        SetDCPWM2(512);
                        ST_BT2 = 1;
                    }
                }
            }
        }
        ST_BT2 = 0; // BOTÃO LIBERADO, LIMPA O FLAG

        while(BT3)
        {
            ClrWdt();
            if(--FILTRO31 == 0)
            {
                FILTRO32--;
                if(FILTRO32 == 0)
                {
                    if(!ST_BT3)
                    {
                        tela_tecla_3();
                        SetDCPWM2(767);
                        ST_BT3 = 1;
                    }
                }
            }
        }
        ST_BT3 = 0; // BOTÃO LIBERADO, LIMPA O FLAG

        while(BT4)
        {
            ClrWdt();
            if(--FILTRO41 == 0)
            {
                FILTRO42--;
                if(FILTRO42 == 0)
                {
                    if(!ST_BT4)
                    {
                        tela_tecla_4();
                        SetDCPWM2(1023);
                        ST_BT4 = 1;
                    }
                }
            }
        }
        ST_BT4 = 0; // BOTÃO LIBERADO, LIMPA O FLAG

    }// FIM LAÇO PRINCIPAL
}    // FIM DO PROGRAMA
