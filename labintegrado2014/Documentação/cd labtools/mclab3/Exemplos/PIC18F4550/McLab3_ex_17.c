/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                               Exemplo 17                                *
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

// ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DO TMR1 COMO
// CONTADOR, UTILIZADO NA PLACA MCLAB2 PARA CONTAR AS ROTAÇÕES DO VENTILADOR.
// O SOFTWARE CONVERTE O CANAL 0 DO CONVERSOR A/D (SENSOR DE TEMPERATURA).
// DOIS PWMs FORAM UTILIZADOS, UM PARA MODULAR A RESISTÊNCIA DE AQUECIMENTO
// E OUTRO PARA A VELOCIDADE DO VENTILADOR.
// COM AS TECLAS S1 E S2 PODE-SE VARIAR O PWM DO AQUECEDOR E COM AS TECLAS
// S3 E S4 O PWM DO VENTILADOR.
// NO LCD SÃO MOSTRADOS OS VALORES DO PWM DO AQUECEDOR, O NÚMERO DE ROTAÇÕES
// POR SEGUNDO DO VENTILADOR E A TEMPERATURA DO DIODO JÁ CONVERTIDA EM GRAUS
// CELSIUS. ALÉM DISSO, O VALOR ATUAL DA TEMPERATURA DO DIODO É TRANSMITIDO
// PERIODICAMENTE ATRAVÉS DA USART.

// Observação: ligar a chave RX-232, TEMP e VENT, 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINIÇÃO PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <pwm.h>                //PWM library functions
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Definição e inicialização das variáveis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco estão definidas as variáveis globais do programa.

unsigned char    FILTRO;
unsigned char    tempo_turbo = 30;
unsigned char    unidade;
unsigned char    dezena;
unsigned char    centena;
unsigned char    tempo = 100;
unsigned short   temperatura = 0;
unsigned short   cont_vent = 0;
unsigned char    intensidade_vent = 0;
unsigned char    intensidade_aquec = 0;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.
//Este programa não utiliza nenhuma constante de usuário

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

#define    MOSTRA_RPS            FLAGSbits.BIT0    //ATUALIZAÇÃO DA LEITURA DA ROTAÇÃO
#define    MOSTRA_TEMPERATURA    FLAGSbits.BIT1    //ATUALIZAÇÃO DA LEITURA DA TEMPERATURA

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
void escreve_frase(const rom char *frase);
void tela_principal(void);
void converte_bcd(unsigned char aux);
void TRATA_INT_TIMER2(void);
void TRATA_HIGH_INT(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

#define     BT1     PORTDbits.RD4   //PORTA DO BOTÃO
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT2     PORTDbits.RD5   //PORTA DO BOTÃO
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT3     PORTDbits.RD6   //PORTA DO BOTÃO
                                    //1 -> PRESSIONADO
                                    //0 -> LIBERADO

#define     BT4     PORTDbits.RD7   //PORTA DO BOTÃO
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
 *             Função para escrever uma frase no LCD                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_frase(const rom char *frase)
{
    do
    {
        escreve_lcd(*frase);
    }while(*++frase);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       FUNÇÃO PARA SEPARAR DÍGITOS                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void converte_bcd(unsigned char aux)
{
    unidade = 0;
    dezena = 0;
    centena = 0;
    if(aux == 0)return;    //SE AUX. FOR IGUAL A ZERO, SAI
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
 *              Função de tratamento de interrupção de Timer2          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TRATA_INT_TIMER2(void)
{
    PIR1bits.TMR2IF = 0;        // limpa flag de interrupção
    tempo--;                    // DECREMENTA TEMPO
    if(!tempo)                  // TEMPO = 0?
    {
        tempo = 100;            // REINICIA TEMPO
        cont_vent = ReadTimer1();    //LER O VALOR DO TIMER1
        WriteTimer1(0);              //carraga timer1 com 0
        MOSTRA_RPS = 1;              // SETA FLAG MOSTRA_RPS
    }
    temperatura = ADRESH;        // LER O RESULTADO DA CONVERSÃO AD
    ConvertADC();                // INICIA UMA NOVA CONVERSÃO
    MOSTRA_TEMPERATURA = 1;      // SETA FLAG MOSTRA_TEMPERATURA
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
    TRISC = 0b10111001;         //CONFIG DIREÇÃO DOS PINOS PORTC
    TRISD = 0b00000000;         //CONFIG DIREÇÃO DOS PINOS PORTD
    TRISE = 0b00000100;         //CONFIG DIREÇÃO DOS PINOS PORTE

    OpenADC(ADC_FOSC_8 & ADC_LEFT_JUST & ADC_0_TAD, ADC_CH0 & ADC_INT_OFF & ADC_VREFPLUS_VDD
            & ADC_VREFMINUS_VSS,12);
    //CONFIGURAÇÃO DO AD
    OpenUSART(USART_TX_INT_OFF  &  USART_RX_INT_OFF  &  USART_ASYNCH_MODE &
              USART_EIGHT_BIT   &  USART_CONT_RX     &  USART_BRGH_HIGH,25);
    //CONFIGURAÇÃO DA USART

    OpenTimer1(TIMER_INT_OFF & T1_SOURCE_EXT & T1_16BIT_RW & T1_PS_1_1 & T1_OSC1EN_OFF &
               T1_SYNC_EXT_ON);
    //CONFIGURAÇÃO DO TIMER1

    OpenTimer2(TIMER_INT_ON & T2_PS_1_4 & T2_POST_1_10);
    //CINFIGURAÇÃO DO TIMER2

    //CONFIGURAÇÃO DO MÓDULO CCP1 -> PWM
    CCP1CON = 0B00001100;
    PR2 = 249;

    //CONFIGURAÇÃO DO MÓDULO CCP2 -> PWM
    CCP2CON = 0B00001111;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    RCON = 0x00;

    COLUNA_1 = 1;       //ATIVA COLUNA 1 DO TECLADO MATRICIAL
    
    SetDCPWM1(0);       //DUTY CYCLE DO PWM1 EM 0
    SetDCPWM2(0);       //DUTY CYCLE DO PWM2 EM 0

    INTCONbits.PEIE = 1;    //LIGA AS INTERRUPÇÕES
    INTCONbits.GIE = 1;

    inicializa_lcd();       //INICIALIZA O LCD

    MOSTRA_RPS = 0;         //LIMPA OS FLAGS
    MOSTRA_TEMPERATURA = 0;

    comando_lcd(0x80);      //IMPRIME A TELA PRINCIPAL
    escreve_frase("Aquec. RPS TEMP");
    comando_lcd(0xC0);
    escreve_frase(" 000%  000 000 C");
    comando_lcd(0xCE);
    escreve_lcd(0xDF);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
    {
        ClrWdt();

        if(MOSTRA_RPS)                      //DEVE MOSTRAR O VALOR DO RPS?
        {
            MOSTRA_RPS = 0;                 //LIMPA FLAG
            cont_vent = cont_vent / 7;      //DIVIDE A LEITURA DO TIMER1 POR 7
            converte_bcd((unsigned char)cont_vent); //EFETUA A CONVERSÃO EM BCD
            comando_lcd(0xC7);              // posiciona o cursor na linha 1, coluna 7
            escreve_lcd(centena + 0x30);    // imprime mensagem no lcd
            escreve_lcd(dezena + 0x30);
            escreve_lcd(unidade + 0x30);
        }

        if(MOSTRA_TEMPERATURA)              //DEVE MOSTRAR A TEMPERATURA?
        {
            MOSTRA_TEMPERATURA = 0;         //LIMPA FLAG
            temperatura = (temperatura * 250)/255;      //CONVERTE O VALOR EM AD PARA TEMPERATURA
            converte_bcd((unsigned char)temperatura);   //BUSCA O VALOR CORRETO DA TEMP.
                                                        //E EFETUA A CONVERSÃO EM BCD
            comando_lcd(0xCB);              // posiciona o cursor na linha 1, coluna 11
            escreve_lcd(centena + 0x30);    // imprime mensagem no lcd
            escreve_lcd(dezena + 0x30);
            escreve_lcd(unidade + 0x30);

            while(BusyUSART());             //AGUARDA BUFFER DE TRANSMISSÃO FICAR VAZIO
            WriteUSART(temperatura);        //ENVIA TEMPERATURA À USART
        }

        if(BT1)                             //BOTÃO 1 PRESSIONADO?
        {
            FILTRO--;                       //DECREMENTA FILTRO DE BOTÃO
            if(!FILTRO)                     //FILTRO = 0?
            {
                tempo_turbo--;              //DECREMENTA TURBO
                if(!tempo_turbo)            //TURBO =0?
                {
                    if(intensidade_aquec < 100)
                    {
                        intensidade_aquec++;
                        //SE INTENSIDADE_AQUEC < 100, INCREMENTA
                        converte_bcd(intensidade_aquec);    //CONVERTE INTENSIDADE_AQUE EM BCD
                        comando_lcd(0xC1);                  // posiciona o cursor na linha 1, coluna 1
                        escreve_lcd(centena + 0x30);        // imprime mensagem no lcd
                        escreve_lcd(dezena + 0x30);
                        escreve_lcd(unidade + 0x30);
                        SetDCPWM1((unsigned short)intensidade_aquec*10);    //ATUALIZA PWM1
                        tempo_turbo = 30;   //REINICIA TURBO DE TECLA
                    }
                }
            }
            continue;
        }

        if(BT2)                             //BOTÃO 2 PRESSIONADO?
        {
            FILTRO--;                       //DECREMENTA FILTRO DE BOTÃO
            if(!FILTRO)                     //FILTRO = 0?
            {
                tempo_turbo--;              //DECREMENTA TURBO
                if(!tempo_turbo)            //TURBO =0?
                {
                    if(intensidade_aquec > 0)
                    {
                    intensidade_aquec--;
                    //SE INTENSIDADE_AQUEC > 0, DECREMENTA
                    converte_bcd(intensidade_aquec);    //CONVERTE INTENSIDADE_AQUE EM BCD
                    comando_lcd(0xC1);                  // posiciona o cursor na linha 1, coluna 1
                    escreve_lcd(centena + 0x30);        // imprime mensagem no lcd
                    escreve_lcd(dezena + 0x30);
                    escreve_lcd(unidade + 0x30);
                    SetDCPWM1((unsigned short)intensidade_aquec*10);    //ATUALIZA PWM1
                    tempo_turbo = 30;                   //REINICIA TURBO DE TECLA
                    }
                }
            }
            continue;
        }

        if(BT3)                             //BOTÃO 3 PRESSIONADO?
        {
            FILTRO--;                       //DECREMENTA FILTRO DE BOTÃO
            if(!FILTRO)                     //FILTRO = 0?
            {
                tempo_turbo--;              //DECREMENTA TURBO
                if(!tempo_turbo)            //TURBO =0?
                {
                    if(intensidade_vent < 100) intensidade_vent++;
                    //SE INTENSIDADE_VENT < 100, INCREMENTA
                    SetDCPWM2((unsigned short)intensidade_vent*10); //ATUALIZA PWM2
                    tempo_turbo = 30;       //REINICIA TURBO DE TECLA
                }
            }
            continue;
        }

        if(BT4)                             //BOTÃO 4 PRESSIONADO?
        {
            FILTRO--;                       //DECREMENTA FILTRO DE BOTÃO
            if(!FILTRO)                     //FILTRO = 0?
            {
                tempo_turbo--;              //DECREMENTA TURBO
                if(!tempo_turbo)            //TURBO =0?
                {
                    if(intensidade_vent > 0) intensidade_vent--;
                    //SE INTENSIDADE_VENT > 0, DECREMENTA
                    SetDCPWM2((unsigned short)intensidade_vent*10); //ATUALIZA PWM2
                    tempo_turbo = 30;       //REINICIA TURBO DE TECLA
                }
            }
            continue;
        }
        FILTRO = 255;                       //REINICIA FILTRO DE BOTÃO
    }
}   // FIM DO PROGRAMA

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *             ROTINA DE TRATAMENTO DE INT DE ALTA PRIORIDADE              *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma code VETOR_HIGH_PRIORITY = 0x0008
void HIGH_int(void)
{
    _asm goto TRATA_HIGH_INT _endasm
}
#pragma code

#pragma interrupt TRATA_HIGH_INT
void TRATA_HIGH_INT(void)
{
    if(PIR1bits.TMR2IF) TRATA_INT_TIMER2();
}
