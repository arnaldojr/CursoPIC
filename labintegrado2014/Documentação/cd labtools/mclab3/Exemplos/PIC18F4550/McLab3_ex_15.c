/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                               Exemplo 15                                *
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
// Este exemplo foi elaborado para explicar o funcionamento do teclado
// matricial4x4.
// O número da linha e coluna da tecla pressionada e mostrado no LCD.

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINIÇÃO PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
//Este programa não utiliza nenhuma variável de usuário

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.
//Este programa não utiliza nenhuma constante de usuário

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
void comando_lcd(unsigned char caracter);
void escreve_lcd(unsigned char caracter);
void limpa_lcd(void);
void inicializa_lcd(void);
void tela_principal(void);
void trata_linha(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                 Definição e inicialização das variáveis                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco estão definidas as variáveis globais do programa.

unsigned char filtro;       // Filtro para teclas
unsigned char num_linha;    // Armazena o número da linha ativada
unsigned char num_coluna;   // Armazena o número da coluna

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declaração dos flags de software                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de flags ajuda na programação e economiza memória RAM.
union{
  struct{
    unsigned BIT0:1;
    unsigned BIT1:1;
    unsigned BIT2:1;
    unsigned BIT3:1;
    unsigned BIT4:1;
    unsigned BIT5:1;
    unsigned BIT6:1;
    unsigned BIT7:1;
  };                        //ARMAZENA OS FLAGS DE CONTROLE
    unsigned char coluna_teclado;
}coluna_ativa;

#define     coluna1  coluna_ativa.BIT0    /* LINHA_ATIVA,4, BIT 4 DO REGISTRADOR LINHA_ATIVA
                                        REPRESENTA A LINHA 1 DO TECLADO 4x4
                                        1 -> LINHA ESTÁ ATIVADA
                                        0 -> LINHA ESTÁ DESATIVADA */

#define     coluna2  coluna_ativa.BIT1    /* LINHA_ATIVA,5, BIT 5 DO REGISTRADOR LINHA_ATIVA
                                        REPRESENTA A LINHA 2 DO TECLADO 4x4
                                        1 -> LINHA ESTÁ ATIVADA
                                        0 -> LINHA ESTÁ DESATIVADA */

#define     coluna3  coluna_ativa.BIT2    /* LINHA_ATIVA,6, BIT 6 DO REGISTRADOR LINHA_ATIVA
                                        REPRESENTA A LINHA 3 DO TECLADO 4x4
                                        1 -> LINHA ESTÁ ATIVADA
                                        0 -> LINHA ESTÁ DESATIVADA */

#define     coluna4  coluna_ativa.BIT3    /* LINHA_ATIVA,7, BIT 7 DO REGISTRADOR LINHA_ATIVA
                                        REPRESENTA A LINHA 4 DO TECLADO 4x4
                                        1 -> LINHA ESTÁ ATIVADA
                                        0 -> LINHA ESTÁ DESATIVADA */

#define     fim     coluna_ativa.BIT0    /* REPRESENTA O FINAL DA VARREDURA
                                        1 -> LINHA ESTÁ ATIVADA
                                        0 -> LINHA ESTÁ DESATIVADA */

#define     coluna  coluna_ativa.coluna_teclado

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               ENTRADAS                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.
#define     linha1  PORTDbits.RD4           /* PINO DE ENTRADA DA LINHA 1
                                            1 -> ALGUMA TECLA DESTA LINHA ESTÁ PRESSIONADA
                                            0 -> NENHUMA TECLA DESTA LINHA ESTÁ PRESSIONADA */

#define     linha2  PORTDbits.RD5           /* PINO DE ENTRADA DA LINHA 2
                                            1 -> ALGUMA TECLA DESTA LINHA ESTÁ PRESSIONADA
                                            0 -> NENHUMA TECLA DESTA LINHA ESTÁ PRESSIONADA */

#define     linha3  PORTDbits.RD6           /* PINO DE ENTRADA DA LINHA 3
                                            1 -> ALGUMA TECLA DESTA LINHA ESTÁ PRESSIONADA
                                            0 -> NENHUMA TECLA DESTA LINHA ESTÁ PRESSIONADA */

#define     linha4  PORTDbits.RD7           /* PINO DE ENTRADA DA LINHA 4
                                            1 -> ALGUMA TECLA DESTA LINHA ESTÁ PRESSIONADA
                                            0 -> NENHUMA TECLA DESTA LINHA ESTÁ PRESSIONADA */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                SAÍDAS                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As saídas devem ser associadas a nomes para facilitar a programação e
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
 *                            Tela Principal                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_principal(void)
{
    comando_lcd(0x82);    // posiciona o cursor na linha 0, coluna 0
    escreve_lcd ('T');    // imprime mensagem no lcd
    escreve_lcd ('e');
    escreve_lcd ('c');
    escreve_lcd ('l');
    escreve_lcd ('a');
    escreve_lcd ('d');
    escreve_lcd ('o');
    escreve_lcd (' ');
    escreve_lcd ('4');
    escreve_lcd ('X');
    escreve_lcd ('4');

    comando_lcd(0xC0);    // posiciona o cursor na linha 1, coluna 0
    escreve_lcd ('L');    // imprime mensagem no lcd
    escreve_lcd ('i');
    escreve_lcd ('n');
    escreve_lcd ('h');
    escreve_lcd ('a');
    escreve_lcd (':');
    escreve_lcd (' ');
    escreve_lcd (' ');
    escreve_lcd ('C');
    escreve_lcd ('o');
    escreve_lcd ('l');
    escreve_lcd ('u');
    escreve_lcd ('n');
    escreve_lcd ('a');
    escreve_lcd (':');
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *            Rotina para tratamento das colunas do Teclado 4X4            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void trata_linha(void)
{
    if(filtro)
    {
        filtro--;
        if(filtro)return;
    }
    comando_lcd(0xC6);              // posiciona o cursor na linha 1, coluna 6
    escreve_lcd(0x30 + num_linha);  //
    comando_lcd(0xCF);              // posiciona o cursor na linha 1, coluna 15
    escreve_lcd(0x30 + num_coluna); //
    PORTD = 0;
    PORTD = coluna;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                          Configurações do Pic                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
    TRISE = 0b00000100;         //CONFIG DIREÇÃO DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

    inicializa_lcd();            // configura o lcd
    tela_principal();            // imprime a tela principal no LCD

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              Rotina principal                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    coluna = 0b00001000;

    while(1)                    // rotina principal
    {
        ClrWdt();               //Inicia o watch-dog timer

        if(filtro == 100)
        {
            coluna = coluna >> 1;
            if(!coluna) coluna = 0b00001000;
            PORTD = coluna;
        }
        if(coluna4) num_coluna = 4;     // verifica qual é a linha ativa
        if(coluna3) num_coluna = 3;
        if(coluna2) num_coluna = 2;
        if(coluna1) num_coluna = 1;

        switch(PORTD & 0b11110000)
        {
            case 16:
                num_linha = 1;        // verifica qual é a linha ativa
                trata_linha();
                break;

            case 32:
                num_linha = 2;        // verifica qual é a linha ativa
                trata_linha();
                break;

            case 64:
                num_linha = 3;        // verifica qual é a linha ativa
                trata_linha();
                break;

            case 128:
                num_linha = 4;        // verifica qual é a linha ativa
                trata_linha();
                break;

            default:
                filtro = 100;
                PORTB = 0;
                comando_lcd(0xC6);     // posiciona o cursor na linha 1, coluna 6
                escreve_lcd(' ');     //
                comando_lcd(0xCF);     // posiciona o cursor na linha 1, coluna 15
                escreve_lcd(' ');     //
                PORTD = 0;
                PORTB = coluna;
                break;
        }
    }
}   // FIM DO PROGRAMA

