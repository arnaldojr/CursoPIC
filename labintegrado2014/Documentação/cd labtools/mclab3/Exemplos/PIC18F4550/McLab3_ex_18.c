/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                               Exemplo 18                                *
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

// ESTE EXEMPLO FOI ELABORADO PARA DEMONSTRAR O FUNCIONAMENTO DA INTERFACE
// DE COMUNICAÇÃO SERIAL RS-485. O VALOR HEXADECIMAL CORRESPONDENTE AS ENTRADAS
// DO KIT "TRANSMISSOR" SERÁ EXIBIDO NO LCD DO KIT "RECEPTOR", AMBOS INTERLIGADOS
// ATRAVÉS DA INTERFACE. A VARREDURA E TRANSMISSÃO DO VALOR CORRESPONDENTE A CADA
// ENTRADA DO KIT "TRANSMISSOR" SÃO EFETUADAS A CADA 100ms E A RECEPÇÃO DO DADO
// POR PARTE DO KIT "RECEPTOR" NA ROTINA PRINCIPAL DO PROGRAMA. A VELOCIDADE DA
// COMUNICAÇÃO È 9600BPS.

// UTILIZE A DIRETIVA MODO_OPERACAO PARA COMPILAR O CÒDIGO NA VERSÃO TRANSMISSOR
// OU RECEPTOR

// Observação: ligar a chave RX-485, EN-LED

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINIÇÃO PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <adc.h>                //ADC library functions
#include <timers.h>             //Timer library functions
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

#define TRANSMISSOR         0
#define RECEPTOR            1

#define MODO_OPERACAO       RECEPTOR

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Definição e inicialização das variáveis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco estão definidas as variáveis globais do programa.

unsigned char contador_varredura;           // Variável mutiplicadora do tempo
                                            // de varredura

unsigned char valor_ad;                     // Variável para armazenamento
                                            // dos valor do ad

unsigned char hexa_high, hexa_low;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.

#if( MODO_OPERACAO == TRANSMISSOR )
    #define T_VARREDURA    20           // 20x5ms = 100ms para
                                        // varredura das entradas
#endif

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

#define DIR             LATEbits.LATE2  // Pino de direcionamento do fluxo de dados
                                        // na interface RS-485

#define LED_VARREDURA   LATBbits.LATB0      // Led de sinalização do instante de varredura

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
 *              Função de tratamento de interrupção de Timer2          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#if( MODO_OPERACAO == TRANSMISSOR )
    void timer2_isr( void )
    {
        contador_varredura--;
        if( !contador_varredura )       // Alcançamos o tempo de varredura?
        {
            // Sim. Recarregamos multiplicador e atualizamos LED_VARREDURA
            contador_varredura = T_VARREDURA;
            LED_VARREDURA = ~LED_VARREDURA;

            ConvertADC();           //Inicia conversão AD
            while(BusyADC());       //Aguarda Fim da conversão AD
            valor_ad = ADRESH;

            while(BusyUSART());
            WriteUSART( valor_ad );
        }
        PIR1bits.TMR2IF  = 0;           // Limpa flag de interrupção do Timer2
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
 *             Função para escrever uma frase no LCD                       *
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
     *                   Inicialização do Timer 2                              *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    void inicializa_timer( void )
    {
        /*** Timer2 - ~5ms: base de tempo para varredura das entradas ***/

        T2CON = 0x71;               // Timer2 parado, prescale 1:4, postcale
                                    // 1:16, fonte de clock interno
        TMR2  = 0x00;               // Zera contador
        PR2   = 78;                 // Carrega período do Timer2

        PIR1bits.TMR2IF  = 0;       // Limpa flag de interrupção do Timer2
        PIE1bits.TMR2IE  = 1;       // Habilita interrupção Timer2

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
    TRISE = 0b00000000;         //CONFIG DIREÇÃO DOS PINOS PORTE

    //CONFIGURAÇÃO DO AD

    #if( MODO_OPERACAO == TRANSMISSOR )
        OpenADC(ADC_FOSC_8 & ADC_LEFT_JUST & ADC_0_TAD, ADC_CH1 & ADC_INT_OFF & ADC_VREFPLUS_VDD
        & ADC_VREFMINUS_VSS,12);
    #else
        ADCON1 = 0b00001111;
    #endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    RCON = 0x00;

    inicializa_lcd();           // configura o lcd
    tela_principal();           // imprime a tela principal no LCD

    //CONFIGURAÇÃO DA USART
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE &
              USART_EIGHT_BIT  & USART_CONT_RX    & USART_BRGH_HIGH,25);

    #if( MODO_OPERACAO == TRANSMISSOR )
        // Carrega valor referência para varredura
        contador_varredura = T_VARREDURA;
        
        inicializa_timer();         // Inicializa Timer 2
        INTCONbits.PEIE = 1;    //LIGA AS INTERRUPÇÕES
        INTCONbits.GIE = 1;
        DIR = 1; 
    #else
        DIR = 0;                    // Modo recepção
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

