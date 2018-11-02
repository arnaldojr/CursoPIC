/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de C�digo para utiliza��o com a McLab3             *
 *                               Exemplo 13                                *
 *                                                                         *
 *                                MOSAICO                                  *
 *                                                                         *
 *  TEL: (0XX11) 4992-8775               SITE: www.mosaico.com.br          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   VERS�O : 1.0                                                          *
 *   DATA : 10/05/2011                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             Descri��o geral                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DA LEITURA/ESCRITA
// NA MEM�RIA E2PROM SERIAL EXTERNA, UTILIZANDO O MASTER SPI.
// AS FUN��ES DOS BOT�ES NA MATRIZ S�O:
//        LINHA 1 (S1) -> DECREMENTA VALOR
//        LINHA 2 (S5) -> INCREMENTA VALOR
//        LINHA 4 (S9) -> ESCRITA DO VALOR
//        LINHA 3 (S13)-> LEITURA DO VALOR

// Observa��o: ligar a chave SCK(RB1), SDO(RC7), SDI(RB0) e CS-EEPROM

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
unsigned char FILTRO11; //FILTRAGEM  1 PARA O BOT�O 1
unsigned char FILTRO12; //FILTRAGEM  2 PARA O BOT�O 1
unsigned char FILTRO21; //FILTRAGEM  1 PARA O BOT�O 2
unsigned char FILTRO22; //FILTRAGEM  2 PARA O BOT�O 2
unsigned char FILTRO31; //FILTRAGEM  1 PARA O BOT�O 3
unsigned char FILTRO32; //FILTRAGEM  2 PARA O BOT�O 3
unsigned char FILTRO41; //FILTRAGEM  1 PARA O BOT�O 4
unsigned char FILTRO42; //FILTRAGEM  2 PARA O BOT�O 4

unsigned char    valor_dados = 0;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.
#define T_FILTRO    25

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

#define ST_BT1  FLAGSbits.BIT0        //STATUS DO BOT�O 1
#define ST_BT2  FLAGSbits.BIT1        //STATUS DO BOT�O 2
#define ST_BT3  FLAGSbits.BIT2        //STATUS DO BOT�O 3
#define ST_BT4  FLAGSbits.BIT3        //STATUS DO BOT�O 4

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
void escreve_eeprom_SPI(unsigned int endereco, unsigned char dado);
unsigned char le_eeprom_SPI(unsigned int endereco);
void atualiza_linha_1(void);
void atualiza_linha_2(unsigned char aux);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

#define     BT1     PORTDbits.RD4   //PORTA DO BOT�O S1
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT2     PORTDbits.RD5   //PORTA DO BOT�O S5
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT3     PORTDbits.RD6   //PORTA DO BOT�O S9
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT4     PORTDbits.RD7   //PORTA DO BOT�O S13
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

#define     rs          PORTEbits.RE0   // via do lcd que sinaliza recep��o de dados ou comando
#define     rw          PORTBbits.RB7   // via do lcd que configura leitura ou escrita no barramento
#define     enable      PORTEbits.RE1   // enable do lcd

#define     CS_SPI      PORTAbits.RA5   // CS da mem�ria

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
 *                            Tela Principal                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void atualiza_linha_1(void)
{
    comando_lcd(0x83);      // posiciona o cursor na linha 0, coluna 0
    escreve_lcd ('M');      // imprime mensagem no lcd
    escreve_lcd ('A');
    escreve_lcd ('S');
    escreve_lcd ('T');
    escreve_lcd ('E');
    escreve_lcd ('R');
    escreve_lcd (' ');
    escreve_lcd ('S');
    escreve_lcd ('P');
    escreve_lcd ('I');
}

void atualiza_linha_2(unsigned char aux)
{
    unsigned char hexa_high;
    unsigned char hexa_low;

    hexa_low = aux & 0b00001111;
    hexa_high = (aux >> 4) & 0b00001111;

    comando_lcd(0xC6);  // posiciona o cursor na linha 1, coluna 6
    if(hexa_high <= 9) escreve_lcd(hexa_high + 0x30);// imprime mensagem no lcd
    else escreve_lcd(hexa_high + 0x37);

    if(hexa_low <= 9) escreve_lcd(hexa_low + 0x30);  // imprime mensagem no lcd
    else escreve_lcd(hexa_low + 0x37);

    escreve_lcd('h');   // imprime mensagem no lcd
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              ESCRITA DA EEPROM                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
**===========================================================================
**         void escreve_eeprom_SPI(unsigned int endereco, unsigned char dado)
**
**         Escreve um dado na mem�ria externa 25LC512
**===========================================================================
*/
void escreve_eeprom_SPI(unsigned int endereco, unsigned char dado)
{
    unsigned char endereco_high;
    unsigned char endereco_low;
    
    endereco_high = endereco >> 8;  // Separa endereco em byte alto e baixo
    endereco_low = endereco;
    
    CS_SPI = 0;
    WriteSPI(0x06);                 // Habilita a escrita
    CS_SPI = 1;
    
    Nop();                          // Delay de 4 ciclos
    Nop();                          //
    Nop();                          //
    Nop();
    
    CS_SPI = 0;
    WriteSPI(0x02);             // Comando de escrita
    WriteSPI(endereco_high);    // Envia byte alto do endere�o
    WriteSPI(endereco_low);     // Envia byte baixo do endere�o
    WriteSPI(dado);             // Envia o dado a ser escrito
    CS_SPI = 1;
    
    Delay1KTCYx(10);            // Aguarda o fim da escrita
}

/*
**===========================================================================
**         unsigned char le_eeprom_SPI(unsigned int endereco)
**
**         L� um dado na mem�ria externa 25LC512
**===========================================================================
*/
unsigned char le_eeprom_SPI(unsigned int endereco)
{
    unsigned char endereco_high;
    unsigned char endereco_low;
    unsigned char dado;
    
    endereco_high = endereco >> 8;  // Separa endereco em byte alto e baixo
    endereco_low = endereco;
    
    CS_SPI = 0;
    WriteSPI(0x03);             // Comando de leitura
    WriteSPI(endereco_high);    // Envia byte alto do endere�o
    WriteSPI(endereco_low);     // Envia byte baixo do endere�o
    dado = ReadSPI();           // Escreve dado
    CS_SPI = 1;
    
    Delay1KTCYx(10);            // Aguarda o fim da escrita
    
    return(dado);
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
    TRISB = 0b00000001;         //CONFIG DIRE��O DOS PINOS PORTB
    TRISC = 0b01010101;         //CONFIG DIRE��O DOS PINOS PORTC
    TRISD = 0b00000000;         //CONFIG DIRE��O DOS PINOS PORTD
    TRISE = 0b00000100;         //CONFIG DIRE��O DOS PINOS PORTE

    ADCON1 = 0b00001111;        //DESLIGA CONVERSORES A/D
    
    CS_SPI = 1;


    OpenSPI( SPI_FOSC_4, MODE_00, SMPMID );

   // while(RCONbits.NOT_TO);     //aguarda estouro do WDT

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


    inicializa_lcd();           // configura o lcd

    atualiza_linha_1();
    valor_dados = le_eeprom_SPI(0x0100);
    atualiza_linha_2(valor_dados);
    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
    {
        ClrWdt();

        FILTRO11 = 0;           //BT1
        FILTRO21 = 0;           //BT2
        FILTRO31 = 0;           //BT3
        FILTRO41 = 0;           //BT4
        
        while(BT1)
        {
            ClrWdt();
            if(--FILTRO11 == 0)
            {
                if(!ST_BT1)
                {
                    valor_dados++;
                    atualiza_linha_2(valor_dados);
                    ST_BT1 = 1;
                }
            }
        }
        ST_BT1 = 0; // BOT�O LIBERADO, LIMPA O FLAG

        while(BT2)
        {
            ClrWdt();
            if(--FILTRO21 == 0)
            {
                if(!ST_BT2)
                {
                    valor_dados--;
                    atualiza_linha_2(valor_dados);
                    ST_BT2 = 1;
                }
            }
        }
        ST_BT2 = 0; // BOT�O LIBERADO, LIMPA O FLAG

        while(BT3)
        {
            ClrWdt();
            if(--FILTRO31 == 0)
            {
                if(!ST_BT3)
                {
                    escreve_eeprom_SPI(0x0100,valor_dados);
                    atualiza_linha_2(valor_dados);
                    ST_BT3 = 1;
                }
            }
        }
        ST_BT3 = 0; // BOT�O LIBERADO, LIMPA O FLAG

        while(BT4)
        {
            ClrWdt();
            if(--FILTRO41 == 0)
            {
                if(!ST_BT4)
                {
                    //fun��o
                    valor_dados = le_eeprom_SPI(0x0100);
                    atualiza_linha_2(valor_dados);
                    ST_BT4 = 1;
                }
            }
        }
        ST_BT4 = 0; // BOT�O LIBERADO, LIMPA O FLAG

    }
}   // FIM DO PROGRAMA
