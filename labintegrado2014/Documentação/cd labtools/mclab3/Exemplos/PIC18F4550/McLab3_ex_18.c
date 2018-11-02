/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de C�digo para utiliza��o com a McLab3             *
 *                               Exemplo 18                                *
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

// ESTE EXEMPLO FOI ELABORADO PARA DEMONSTRAR O FUNCIONAMENTO DA INTERFACE
// DE COMUNICA��O SERIAL RS-485. O VALOR HEXADECIMAL CORRESPONDENTE AS ENTRADAS
// DO KIT "TRANSMISSOR" SER� EXIBIDO NO LCD DO KIT "RECEPTOR", AMBOS INTERLIGADOS
// ATRAV�S DA INTERFACE. A VARREDURA E TRANSMISS�O DO VALOR CORRESPONDENTE A CADA
// ENTRADA DO KIT "TRANSMISSOR" S�O EFETUADAS A CADA 100ms E A RECEP��O DO DADO
// POR PARTE DO KIT "RECEPTOR" NA ROTINA PRINCIPAL DO PROGRAMA. A VELOCIDADE DA
// COMUNICA��O � 9600BPS.

// UTILIZE A DIRETIVA MODO_OPERACAO PARA COMPILAR O C�DIGO NA VERS�O TRANSMISSOR
// OU RECEPTOR

// Observa��o: ligar a chave RX-485, EN-LED

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINI��O PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUN��ES DE PERIF�RICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <adc.h>                //ADC library functions
#include <timers.h>             //Timer library functions
#include <delays.h>             //Delay library functions
#include <usart.h>              //USART library functions

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

#define TRANSMISSOR         0
#define RECEPTOR            1

#define MODO_OPERACAO       RECEPTOR

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Defini��o e inicializa��o das vari�veis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco est�o definidas as vari�veis globais do programa.

unsigned char contador_varredura;           // Vari�vel mutiplicadora do tempo
                                            // de varredura

unsigned char valor_ad;                     // Vari�vel para armazenamento
                                            // dos valor do ad

unsigned char hexa_high, hexa_low;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.

#if( MODO_OPERACAO == TRANSMISSOR )
    #define T_VARREDURA    20           // 20x5ms = 100ms para
                                        // varredura das entradas
#endif

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
#if( MODO_OPERACAO == TRANSMISSOR )
    void HIGH_int(void);
    void isr_high(void);
    void timer2_isr(void);
    void inicializa_timer(void);
#endif

void comando_lcd(unsigned char caracter);
void escreve_lcd(unsigned char caracter);
void limpa_lcd(void);
void inicializa_lcd(void);
void escreve_frase(const rom char *frase);
void tela_principal(void);
void converte_hexadecimal(unsigned char aux);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

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

#define DIR             LATEbits.LATE2  // Pino de direcionamento do fluxo de dados
                                        // na interface RS-485

#define LED_VARREDURA   LATBbits.LATB0      // Led de sinaliza��o do instante de varredura

#if( MODO_OPERACAO == TRANSMISSOR )
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *             ROTINA DE TRATAMENTO DE INT DE ALTA PRIORIDADE              *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    #pragma code VETOR_INT_HIGH = 0x0008
    void HIGH_int(void)
    {
        _asm
        goto isr_high
        _endasm
    }
    #pragma code

    #pragma interrupt isr_high
    void isr_high(void)
    {
        if( PIR1bits.TMR2IF && PIE1bits.TMR2IE )
        {
            timer2_isr();
        }
    }
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Fun��o de tratamento de interrup��o de Timer2          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#if( MODO_OPERACAO == TRANSMISSOR )
    void timer2_isr( void )
    {
        contador_varredura--;
        if( !contador_varredura )       // Alcan�amos o tempo de varredura?
        {
            // Sim. Recarregamos multiplicador e atualizamos LED_VARREDURA
            contador_varredura = T_VARREDURA;
            LED_VARREDURA = ~LED_VARREDURA;

            ConvertADC();           //Inicia convers�o AD
            while(BusyADC());       //Aguarda Fim da convers�o AD
            valor_ad = ADRESH;

            while(BusyUSART());
            WriteUSART( valor_ad );
        }
        PIR1bits.TMR2IF  = 0;           // Limpa flag de interrup��o do Timer2
    }
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Converte hexadecimal                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void converte_hexadecimal(unsigned char aux)
{
    hexa_high = (aux >> 4) & 0b00001111;
    if( hexa_high > 9 )
        hexa_high += 55;
    else
        hexa_high += 48;

    hexa_low = aux & 0b00001111;
    if( hexa_low > 9 )
        hexa_low += 55;
    else
        hexa_low += 48;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Rotina que envia um COMANDO para o LCD               *
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
 *                Rotina que envia um DADO a ser escrito no LCD            *
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
 *             Fun��o para escrever uma frase no LCD                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_frase(const rom char *frase)
{
    do
    {
        escreve_lcd(*frase);
    }while(*++frase);
}

#if ( MODO_OPERACAO == TRANSMISSOR )
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *                   Inicializa��o do Timer 2                              *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    void inicializa_timer( void )
    {
        /*** Timer2 - ~5ms: base de tempo para varredura das entradas ***/

        T2CON = 0x71;               // Timer2 parado, prescale 1:4, postcale
                                    // 1:16, fonte de clock interno
        TMR2  = 0x00;               // Zera contador
        PR2   = 78;                 // Carrega per�odo do Timer2

        PIR1bits.TMR2IF  = 0;       // Limpa flag de interrup��o do Timer2
        PIE1bits.TMR2IE  = 1;       // Habilita interrup��o Timer2

        T2CONbits.TMR2ON = 1;       // Liga Timer2
    }
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Tela Principal                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_principal(void)
{
    comando_lcd(0x80);            // posiciona o cursor na linha 0, coluna 0

    #if( MODO_OPERACAO == TRANSMISSOR )
        escreve_frase("   TRANSMISSOR  ");
    #else
        escreve_frase("    RECEPTOR    ");
    #endif

    comando_lcd(0xC0);            // posiciona o cursor na linha 1, coluna 0
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
    TRISC = 0b10111101;         //CONFIG DIRE��O DOS PINOS PORTC
    TRISD = 0b00000000;         //CONFIG DIRE��O DOS PINOS PORTD
    TRISE = 0b00000000;         //CONFIG DIRE��O DOS PINOS PORTE

    //CONFIGURA��O DO AD

    #if( MODO_OPERACAO == TRANSMISSOR )
        OpenADC(ADC_FOSC_8 & ADC_LEFT_JUST & ADC_0_TAD, ADC_CH1 & ADC_INT_OFF & ADC_VREFPLUS_VDD
        & ADC_VREFMINUS_VSS,12);
    #else
        ADCON1 = 0b00001111;
    #endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    RCON = 0x00;

    inicializa_lcd();           // configura o lcd
    tela_principal();           // imprime a tela principal no LCD

    //CONFIGURA��O DA USART
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE &
              USART_EIGHT_BIT  & USART_CONT_RX    & USART_BRGH_HIGH,25);

    #if( MODO_OPERACAO == TRANSMISSOR )
        // Carrega valor refer�ncia para varredura
        contador_varredura = T_VARREDURA;
        
        inicializa_timer();         // Inicializa Timer 2
        INTCONbits.PEIE = 1;    //LIGA AS INTERRUP��ES
        INTCONbits.GIE = 1;
        DIR = 1; 
    #else
        DIR = 0;                    // Modo recep��o
    #endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              Rotina Principal                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
    {
        ClrWdt();               //LIMPA O WDT

        #if( MODO_OPERACAO == RECEPTOR )
            if( DataRdyUSART() )
            {
                converte_hexadecimal( ReadUSART() );
            }
        #else
            converte_hexadecimal( valor_ad );
        #endif

        comando_lcd(0xC5);            // posiciona o cursor na linha 1, coluna 5
        escreve_frase("0x");
        escreve_lcd(hexa_high);
        escreve_lcd(hexa_low);
    }
}   // FIM DO PROGRAMA

