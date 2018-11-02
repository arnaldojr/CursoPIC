/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de C�digo para utiliza��o com a McLab3             *
 *                               Exemplo 16                                *
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
//Exemplo de utiliza��o do rel�gio de tempo real DS1302 atrav�s da interface
//I2C. Para maiores informa��es a respeito do integrado consultar o datasheet
//do componente.

// Observa��o: ligar a chave SCK(RB1), SDO(RC7), SDI(RB0) e CS-RTC

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINI��O PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUN��ES DE PERIF�RICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <delays.h>             //Delay library functions
#include <spi.h>                //SPI library functions

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
void comando_lcd(unsigned char caracter);
void escreve_lcd(unsigned char caracter);
void limpa_lcd(void);
void inicializa_lcd(void);
void tela_principal(void);
void LE_RELOGIO(unsigned char endereco);
void ESCREVE_RELOGIO(unsigned char endereco);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.
#define     END_HORA_WR         0x82    // ENDERE�O DA HORA NO RELOGIO RTC

#define     END_MINUTO_WR       0x81    // ENDERE�O DO MINUTO NO RELOGIO RTC

#define     END_SEGUNDO_WR      0x80    // ENDERE�O DO SEGUNDO NO RELOGIO RTC

#define     END_HORA_RD         0x02    // ENDERE�O DA HORA NO RELOGIO RTC

#define     END_MINUTO_RD       0x01    // ENDERE�O DO MINUTO NO RELOGIO RTC

#define     END_SEGUNDO_RD      0x00    // ENDERE�O DO SEGUNDO NO RELOGIO RTC

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                 Defini��o e inicializa��o das vari�veis                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco est�o definidas as vari�veis globais do programa.
unsigned char dez_hora;
unsigned char uni_hora;
unsigned char dez_min;
unsigned char uni_min;
unsigned char dez_seg;
unsigned char uni_seg;                
unsigned char buffer;
            
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declara��o dos flags de software                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de flags ajuda na programa��o e economiza mem�ria RAM.

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               ENTRADAS                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                SA�DAS                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As sa�das devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

#define     COLUNA_1    PORTDbits.RD0   //PINO PARA ATIVAR COLUNA 1 DO TECLADO
                                        //MATRICIAL
                                        //1 -> COLUNA ATIVADA
                                        //0 -> COLUNA DESATIVADA

#define     rs          PORTEbits.RE0   // via do lcd que sinaliza recep��o de dados ou comando
#define     rw          PORTBbits.RB7   // via do lcd que configura leitura ou escrita no barramento
#define     enable      PORTEbits.RE1   // enable do lcd

#define     CS_SPI      PORTAbits.RA4   // CS da mem�ria

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                Rotina que envia um COMANDO para o LCD                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void comando_lcd(unsigned char caracter)
{
    TRISD = 0b00000000; // configura portd como sa�da
    rs = 0;             // seleciona o envio de um comando
    PORTD = caracter;   // carrega o PORTD com o caracter
    enable = 1;         // gera pulso no enable
    Delay10TCYx(1);     // espera 10 microsegundos
    enable = 0;         // desce o pino de enable
    TRISD = 0b11110000; // CONFIGURA O PORTD PARA O TECLADO
    Delay10TCYx(4);     // espera m�nimo 40 microsegundos
    COLUNA_1 = 1;       // ativa a coluna 1 do teclado
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *             Rotina que envia um DADO a ser escrito no LCD               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_lcd(unsigned char caracter)
{
    TRISD = 0b00000000; // configura portd como sa�da
    rs = 1;             // seleciona o envio de um dado
    PORTD = caracter;   // carrega o PORTD com o caracter
    enable = 1;         // gera pulso no enable
    Delay10TCYx(1);     // espera 10 microsegundos
    enable = 0;         // desce o pino de enable
    TRISD = 0b11110000; // CONFIGURA O PORTD PARA O TECLADO
    Delay10TCYx(4);     // espera m�nimo 40 microsegundos
    COLUNA_1 = 1;       // ativa a coluna 1 do teclado
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              Fun��o para limpar o LCD                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void limpa_lcd(void)
{
    comando_lcd(0x01);  // limpa lcd
    Delay1KTCYx(2);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                   Inicializa��o do Display de LCD                       *
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
 *                           LEITURA DO REL�GIO                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LE_RELOGIO(unsigned char endereco)
{
    CS_SPI = 1;
    Nop();                          // Delay de 4 ciclos
    Nop();                          //
    Nop();                          //
    Nop();
    WriteSPI(endereco);
    buffer = ReadSPI();
    CS_SPI = 0;       
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           ESCRITA NO REL�GIO                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ESCREVE_RELOGIO(unsigned char endereco)
{
    CS_SPI = 1;
    Nop();                          // Delay de 4 ciclos
    Nop();                          //
    Nop();                          //
    Nop();
    WriteSPI(endereco);
    WriteSPI(buffer);
    CS_SPI = 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Tela Principal                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_principal(void)
{
    comando_lcd(0x83);    // posiciona o cursor na linha 0, coluna 0
    escreve_lcd ('R');
    escreve_lcd ('e');
    escreve_lcd ('l');
    escreve_lcd ('o');
    escreve_lcd ('g');
    escreve_lcd ('i');
    escreve_lcd ('o');    // imprime mensagem no lcd 
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                          Configura��es do Pic                           *
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

    TRISA = 0b11000011;         //CONFIG DIRE��O DOS PINOS PORTA
    TRISB = 0b00000001;         //CONFIG DIRE��O DOS PINOS PORTB
    TRISC = 0b11010101;         //CONFIG DIRE��O DOS PINOS PORTC
    TRISD = 0b00000000;         //CONFIG DIRE��O DOS PINOS PORTD
    TRISE = 0b00000100;         //CONFIG DIRE��O DOS PINOS PORTE


    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

    OpenSPI( SPI_FOSC_4, MODE_11, SMPMID );

    while(RCONbits.NOT_TO);     //aguarda estouro do WDT
    COLUNA_1 = 1;
    CS_SPI = 0;

    Nop();
    Nop();
    Nop();
    Nop();
    
    buffer = 0x00;              //configura��o do rel�gio
    ESCREVE_RELOGIO(0x8F);

    buffer = 0x23;          // acerta hora
    ESCREVE_RELOGIO(END_HORA_WR);

    buffer = 0x40;          // acerta minuto
    ESCREVE_RELOGIO(END_MINUTO_WR);

    buffer = 0x00;          // acerta segundos
    ESCREVE_RELOGIO(END_SEGUNDO_WR);

    inicializa_lcd();       // configura o lcd
    tela_principal();       // imprime a tela principal no LCD

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              Rotina principal                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)                // rotina principal
    {
        ClrWdt();           //Inicia o watch-dog timer
        LE_RELOGIO(END_HORA_RD);
        dez_hora = buffer >> 4;
        dez_hora = (dez_hora & 0b00000011) + 0x30;
        uni_hora = buffer;
        uni_hora = (uni_hora & 0b00001111) + 0x30;

        LE_RELOGIO(END_MINUTO_RD);
        dez_min = buffer >> 4;
        dez_min = (dez_min & 0b00001111) + 0x30;
        uni_min = buffer;
        uni_min = (uni_min & 0b00001111) + 0x30;

        LE_RELOGIO(END_SEGUNDO_RD);
        dez_seg = buffer >> 4;
        dez_seg = (dez_seg & 0b00001111) + 0x30;
        uni_seg = buffer;
        uni_seg = (uni_seg & 0b00001111) + 0x30;
        
        comando_lcd(0xC3);      // posiciona o cursor na linha 0, coluna 3
        escreve_lcd (dez_hora); // DEZ. HORA
        escreve_lcd (uni_hora); // UNID. HORA
        escreve_lcd (':');      //
        escreve_lcd (dez_min);  // DEZ. MINUTO
        escreve_lcd (uni_min);  // UNID. MINUTO
        escreve_lcd (':');      //
        escreve_lcd (dez_seg);  // DEZ. SEGUNDO
        escreve_lcd (uni_seg);  // UNID. SEGUNDO
    }
}   // FIM DO PROGRAMA

