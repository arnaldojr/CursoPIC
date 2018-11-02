/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                               Exemplo 14                                *
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
/*
 ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DA USART DO PIC.
 O SOFTWARE CONVERTE O CANAL 1 DO CONVERSOR A/D (POTENCIÔMETRO P2) E MOSTRA
 NO DISPLAY O VALOR CONVERTIDO EM DECIMAL E HEXADECIMAL.
 ALÉM DE MOSTRAR O VALOR NO DISPLAY, O SOFTWARE TRANSMITE PELA USART O VALOR
 DA CONVERSÃO. OS VALORES RECEBIDOS PELA USART TAMBÉM SÃO MOSTRADOS NO LCD
 COMO CARACTERES ASCII.
*/

// Observação: ligar a chave RX-232

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINIÇÃO PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <adc.h>                //ADC library functions
#include <delays.h>             //Delay library functions
#include <usart.h>              //USART library functions

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

unsigned char unidade;
unsigned char dezena;
unsigned char centena;
unsigned char hexa_low;
unsigned char hexa_high;

unsigned char converte;
char usart_rx;

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
void converte_bcd(unsigned char aux);
void converte_hexadecimal(unsigned char aux);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

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
    comando_lcd(0x80);    // posiciona o cursor na linha 0, coluna 0
    escreve_lcd ('U');    // imprime mensagem no lcd
    escreve_lcd ('S');
    escreve_lcd ('A');
    escreve_lcd ('R');
    escreve_lcd ('T');
    escreve_lcd (':');
    escreve_lcd ('9');
    escreve_lcd ('6');
    escreve_lcd ('0');
    escreve_lcd ('0');
    escreve_lcd (',');
    escreve_lcd ('8');
    escreve_lcd (',');
    escreve_lcd ('n');
    escreve_lcd (',');
    escreve_lcd ('1');

    comando_lcd(0xC0);    // posiciona o cursor na linha 1, coluna 4
    escreve_lcd ('T');    // imprime mensagem no lcd
    escreve_lcd ('X');
    escreve_lcd (':');
    escreve_lcd (' ');
    escreve_lcd (' ');
    escreve_lcd (' ');
    escreve_lcd ('d');
    escreve_lcd (' ');
    escreve_lcd (' ');
    escreve_lcd (' ');
    escreve_lcd ('h');
    escreve_lcd (' ');
    escreve_lcd ('R');
    escreve_lcd ('X');
    escreve_lcd (':');
    escreve_lcd (' ');

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               Converte BCD                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void converte_bcd(unsigned char aux)
{
    unidade = 0;
    dezena = 0;
    centena = 0;
    if (aux == 0)return;
    while(aux--)
    {
        unidade++;
        if(unidade != 10)continue;
        unidade = 0;
        dezena++;
        if(dezena != 10)continue;
        dezena = 0;
        centena++;
        if(centena != 10)continue;
        centena = 0;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Converte hexadecimal                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void converte_hexadecimal(unsigned char aux)
{
    hexa_low = aux & 0b00001111;
    hexa_high = (aux >> 4) & 0b00001111;
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
    TRISC = 0b10111101;         //CONFIG DIREÇÃO DOS PINOS PORTC
    TRISD = 0b00000000;         //CONFIG DIREÇÃO DOS PINOS PORTD
    TRISE = 0b00000100;         //CONFIG DIREÇÃO DOS PINOS PORTE

    OpenADC(ADC_FOSC_8 & ADC_LEFT_JUST & ADC_0_TAD, ADC_CH1 & ADC_INT_OFF & ADC_VREFPLUS_VDD
            & ADC_VREFMINUS_VSS,12);
    //CONFIGURAÇÃO DO AD
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE &
              USART_EIGHT_BIT  & USART_CONT_RX    & USART_BRGH_HIGH,25);
    //CONFIGURAÇÃO DA USART 9600BPS

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    inicializa_lcd();            // configura o lcd

    tela_principal();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    ConvertADC();                           //Inicia conversão AD
    while(1)
    {
        ClrWdt();
        if(BusyADC())
        {
            converte = ADRESH;
            converte_bcd(converte);
            comando_lcd(0xC3);              // posiciona o cursor na linha 1, coluna 4
            escreve_lcd(centena + 0x30);    // imprime mensagem no lcd
            escreve_lcd(dezena + 0x30);
            escreve_lcd(unidade + 0x30);

            converte_hexadecimal(ADRESH);
            comando_lcd(0xC8);              // posiciona o cursor na linha 1, coluna 4
            if(hexa_high <= 9) escreve_lcd (hexa_high + 0x30);  // imprime mensagem no lcd
            else escreve_lcd(hexa_high + 0x37);

            if(hexa_low <= 9) escreve_lcd (hexa_low + 0x30);    // imprime mensagem no lcd
            else escreve_lcd(hexa_low + 0x37);

            while(BusyUSART());     //AGUARDA O BUFFER DE TRANSMISSÃO FICAR VAZIO

            WriteUSART(converte);
            ConvertADC();           //Inicia conversão AD
        }

        if(DataRdyUSART())
        {
            usart_rx = ReadUSART();
            comando_lcd(0xCF);      // posiciona o cursor na linha 1, coluna 15
            escreve_lcd(usart_rx);
            ConvertADC();           //Inicia conversão AD
        }

    }// FIM LAÇO PRINCIPAL
}    // FIM DO PROGRAMA
