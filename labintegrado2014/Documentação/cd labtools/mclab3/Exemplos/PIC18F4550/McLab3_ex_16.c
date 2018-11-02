/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                               Exemplo 16                                *
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
//Exemplo de utilização do relógio de tempo real DS1302 através da interface
//I2C. Para maiores informações a respeito do integrado consultar o datasheet
//do componente.

// Observação: ligar a chave SCK(RB1), SDO(RC7), SDI(RB0) e CS-RTC

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINIÇÃO PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <delays.h>             //Delay library functions
#include <spi.h>                //SPI library functions

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
void LE_RELOGIO(unsigned char endereco);
void ESCREVE_RELOGIO(unsigned char endereco);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.
#define     END_HORA_WR         0x82    // ENDEREÇO DA HORA NO RELOGIO RTC

#define     END_MINUTO_WR       0x81    // ENDEREÇO DO MINUTO NO RELOGIO RTC

#define     END_SEGUNDO_WR      0x80    // ENDEREÇO DO SEGUNDO NO RELOGIO RTC

#define     END_HORA_RD         0x02    // ENDEREÇO DA HORA NO RELOGIO RTC

#define     END_MINUTO_RD       0x01    // ENDEREÇO DO MINUTO NO RELOGIO RTC

#define     END_SEGUNDO_RD      0x00    // ENDEREÇO DO SEGUNDO NO RELOGIO RTC

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                 Definição e inicialização das variáveis                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco estão definidas as variáveis globais do programa.
unsigned char dez_hora;
unsigned char uni_hora;
unsigned char dez_min;
unsigned char uni_min;
unsigned char dez_seg;
unsigned char uni_seg;                
unsigned char buffer;
            
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declaração dos flags de software                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de flags ajuda na programação e economiza memória RAM.

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               ENTRADAS                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

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

#define     CS_SPI      PORTAbits.RA4   // CS da memória

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                Rotina que envia um COMANDO para o LCD                   *
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
 *             Rotina que envia um DADO a ser escrito no LCD               *
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
 *                           LEITURA DO RELÓGIO                            *
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
 *                           ESCRITA NO RELÓGIO                            *
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
    TRISB = 0b00000001;         //CONFIG DIREÇÃO DOS PINOS PORTB
    TRISC = 0b11010101;         //CONFIG DIREÇÃO DOS PINOS PORTC
    TRISD = 0b00000000;         //CONFIG DIREÇÃO DOS PINOS PORTD
    TRISE = 0b00000100;         //CONFIG DIREÇÃO DOS PINOS PORTE


    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D

    OpenSPI( SPI_FOSC_4, MODE_11, SMPMID );

    while(RCONbits.NOT_TO);     //aguarda estouro do WDT
    COLUNA_1 = 1;
    CS_SPI = 0;

    Nop();
    Nop();
    Nop();
    Nop();
    
    buffer = 0x00;              //configuração do relógio
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

