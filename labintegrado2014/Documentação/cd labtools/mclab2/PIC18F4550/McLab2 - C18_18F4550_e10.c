/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        Programa��o em C18 - Exemplos usando o PIC18F4550 e McLab2       *
 *                               Exemplo 10                                *
 *                                                                         *
 *                   LABTOOLS - MOSAICO DIDACTIC DIVISION                  *
 *                                                                         *
 *  TEL: (0XX11) 4992-8775               SITE: www.labtools.com.br         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   VERS�O : 1.0                                                          *
 *   DATA : 01/08/2006 				                                  	   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             Descri��o geral                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
	ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DO TMR1 COMO
	CONTADOR, UTILIZADO NA PLACA MCLAB2 PARA CONTAR AS ROTA��ES DO VENTILADOR.
	O SOFTWARE CONVERTE O CANAL 0 DO CONVERSOR A/D (SENSOR DE TEMPERATURA).
	DOIS PWMs FORAM UTILIZADOS, UM PARA MODULAR A RESIST�NCIA DE AQUECIMENTO
	E OUTRO PARA A VELOCIDADE DO VENTILADOR.
	COM AS TECLAS S1 E S2 PODE-SE VARIAR O PWM DO AQUECEDOR E COM AS TECLAS
	S3 E S4 O PWM DO VENTILADOR.
	NO LCD S�O MOSTRADOS OS VALORES DO PWM DO AQUECEDOR, O N�MERO DE ROTA��ES
	POR SEGUNDO DO VENTILADOR E A TEMPERATURA DO DIODO J� CONVERTIDA EM GRAUS
	CELSIUS. AL�M DISSO, O VALOR ATUAL DA TEMPERATURA DO DIODO � TRANSMITIDO
	PERIODICAMENTE ATRAV�S DA USART.
*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                		DEFINI��O PIC			         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>        	 // Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUN��ES DE PERIF�RICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <pwm.h>          		 //PWM library functions
#include <adc.h>           		 //ADC library functions
#include <timers.h>        		 //Timer library functions
#include <delays.h>        		 //Delay library functions
#include <i2c.h>           		 //I2C library functions
#include <stdlib.h>        		 //Library functions
#include <usart.h>         		 //USART library functions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Configura��es para grava��o                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma config FOSC = HS
#pragma config CPUDIV = OSC1_PLL2
#pragma config WDT = ON
#pragma config WDTPS = 128
#pragma config LVP = OFF
#pragma config PWRT = ON
#pragma config BOR = ON
#pragma config BORV = 0

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Defini��o e inicializa��o das vari�veis Globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco est�o definidas as vari�veis globais do programa.

unsigned char	FILTRO;
unsigned char 	tempo_turbo = 30;
unsigned char	unidade;
unsigned char	dezena;	
unsigned char	centena;
unsigned char	tempo = 100;
unsigned char	temperatura = 0;
unsigned short 	cont_vent = 0;
unsigned char 	intensidade_vent = 0;
unsigned char 	intensidade_aquec = 0;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *            TABELA DE LINEARIZA��O DO SENSOR DE TEMPERATURA            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
rom const unsigned char tabela_temp[256] = { 
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 								//15
0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1, 								//31
2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9, 								//47
10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,17, 				//63
18,18,19,19,20,20,21,21,22,22,23,23,23,24,24,25, 				//79
25,26,26,27,27,28,28,29,29,30,30,31,31,32,32,33, 				//95
33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41, 				//111		
41,42,42,43,43,44,44,45,45,46,46,47,47,48,48,49, 				//127
49,50,50,51,51,52,52,53,53,54,54,55,55,56,56,57, 				//143
57,58,58,59,59,60,60,61,61,62,62,63,63,64,64,65, 				//159
65,66,66,67,67,68,68,69,69,70,70,71,71,72,72,73, 				//175
73,74,74,75,75,76,76,77,77,78,78,79,79,80,80,81, 				//191
81,82,82,83,83,84,84,85,85,86,86,87,87,88,88,089, 				//207
89,90,90,91,91,92,92,93,93,94,94,95,95,96,96,097, 				//223
97,98,98,99,99,100,100,101,101,102,102,103,103,104,104,104, 	//239
105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112 //255
};

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
}FLAGSbits;			//ARMAZENA OS FLAGS DE CONTROLE

#define	MOSTRA_RPS			FLAGSbits.BIT0	//ATUALIZA��O DA LEITURA DA ROTA��O
#define	MOSTRA_TEMPERATURA	FLAGSbits.BIT1	//ATUALIZA��O DA LEITURA DA TEMPERATURA

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUN��ES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void comando_lcd(unsigned char caracter);
void escreve_lcd(unsigned char caracter);
void limpa_lcd(void);
void inicializa_lcd(void);
void escreve_frase(const rom char *frase);
void tela_principal(void);
void TRATA_INT_TIMER2(void);
void TRATA_HIGH_INT(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

#define BT1 PORTBbits.RB0		//BOT�O 1
								//0 -> PRESSIONADO
								//1 -> LIBERADO

#define BT2 PORTBbits.RB1		//BOT�O 2
								//0 -> PRESSIONADO
								//1 -> LIBERADO

#define BT3 PORTBbits.RB2		//BOT�O 3
								//0 -> PRESSIONADO
								//1 -> LIBERADO

#define BT4 PORTBbits.RB3		//BOT�O 4
								//0 -> PRESSIONADO
								//1 -> LIBERADO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SA�DAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SA�DAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMA��O E
//FUTURAS ALTERA��ES DO HARDWARE.

#define rs		PORTEbits.RE0	// via do lcd que sinaliza recep��o de dados ou comando
#define enable	PORTEbits.RE1	// enable do lcd

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *					Rotina que envia um COMANDO para o LCD		           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void comando_lcd(unsigned char caracter)
{
	rs = 0;						// seleciona o envio de um comando
	PORTD = caracter;			// carrega o PORTD com o caracter
	enable = 1 ;				// gera pulso no enable
	Delay10TCYx(1);				// espera 10 microsegundos
	enable = 0;					// desce o pino de enable
	Delay10TCYx(4);				// espera m�nimo 40 microsegundos
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *				Rotina que envia um DADO a ser escrito no LCD	           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_lcd(unsigned char caracter)
{
	rs = 1;					// seleciona o envio de um comando
	PORTD = caracter;		// carrega o PORTD com o caracter
	enable = 1;				// gera pulso no enable
	Delay10TCYx(1);			// espera 10 microsegundos
	enable = 0;				// desce o pino de enable
	Delay10TCYx(4);			// espera m�nimo 40 microsegundos
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *						        Fun��o para limpar o LCD		           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void limpa_lcd(void)
{
	comando_lcd(0x01);			// limpa lcd	
	Delay1KTCYx(2);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *		     			Inicializa��o do Display de LCD			           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void inicializa_lcd(void)
{
	comando_lcd(0x30);			// envia comando para inicializar display
	Delay1KTCYx(4);				// espera 4 milisengundos

	comando_lcd(0x30);			// envia comando para inicializar display
	Delay10TCYx(10);			// espera 100 microsengundos

	comando_lcd(0x30);			// envia comando para inicializar display

	comando_lcd(0x38);			// liga o display, sem cursor e sem blink

	limpa_lcd();				// limpa lcd

	comando_lcd(0x0c);			// display sem cursor

	comando_lcd(0x06);			// desloca cursor para a direita
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *		        Fun��o para escrever uma frase no LCD					   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_frase(const rom char *frase)
{
	do
	{
		escreve_lcd(*frase);
	}while(*++frase);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              		  FUN��O PARA SEPARAR D�GITOS					   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void converte_bcd(unsigned char aux)
{
	unidade = 0;
	dezena = 0;
	centena = 0;
	if (aux == 0)return;	//SE AUX. FOR IGUAL A ZERO, SAI
	while(aux--)
	{
		unidade++;
		if(unidade != 10)continue;
		unidade = 0;
		dezena++;
		if (dezena != 10)continue;
		dezena = 0;
		centena++;
		if (centena != 10)continue;
		centena = 0;	
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Fun��o de tratamento de interrup��o de Timer2          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TRATA_INT_TIMER2(void)
{
	PIR1bits.TMR2IF = 0;		// limpa flag de interrup��o
	tempo--;					// DECREMENTA TEMPO
	if(!tempo)					// TEMPO = 0?
	{
		tempo = 100;			// REINICIA TEMPO
		cont_vent = ReadTimer1();	//LER O VALOR DO TIMER1
		WriteTimer1(0);				//carraga timer1 com 0
		MOSTRA_RPS = 1;			// SETA FLAG MOSTRA_RPS
	}
	temperatura = ADRESH;		// LER O RESULTADO DA CONVERS�O AD
	ConvertADC();				// INICIA UMA NOVA CONVERS�O
	MOSTRA_TEMPERATURA = 1;		// SETA FLAG MOSTRA_TEMPERATURA
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Fun��o Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void main ()
{
	PORTA = 0x00;                       //Limpa PORTA
	PORTB = 0x00;                       //Limpa PORTB
	PORTC = 0x00;		         	    //Limpa PORTC
	PORTD = 0x00;                       //Limpa PORTD
	PORTE = 0x00;                       //Limpa PORTE

	LATA = 0x00;                      	//Limpa PORTA
	LATB = 0x00;                        //Limpa PORTB
	LATC = 0x00;		     	        //Limpa PORTC
	LATD = 0x00;                        //Limpa PORTD
	LATE = 0x00;                        //Limpa PORTE

	TRISA = 0b11111111;                 //CONFIG DIRE��O DOS PINOS PORTA0
	TRISB = 0b00001111;                 //CONFIG DIRE��O DOS PINOS PORTB
	TRISC = 0b10111001;		            //CONFIG DIRE��O DOS PINOS PORTC
	TRISD = 0b00000000;                 //CONFIG DIRE��O DOS PINOS PORTD
	TRISE = 0b00000100;                 //CONFIG DIRE��O DOS PINOS PORTE

	OpenADC(ADC_FOSC_8 & ADC_LEFT_JUST & ADC_0_TAD, ADC_CH0 & ADC_INT_OFF & ADC_VREFPLUS_VDD 
			& ADC_VREFMINUS_VSS,12);
	//CONFIGURA��O DO AD
	OpenUSART(USART_TX_INT_OFF  &  USART_RX_INT_OFF  &  USART_ASYNCH_MODE &
	          USART_EIGHT_BIT   &  USART_CONT_RX     &  USART_BRGH_HIGH,25);
	//CONFIGURA��O DA USART

	OpenTimer1(TIMER_INT_OFF & T1_SOURCE_EXT & T1_16BIT_RW & T1_PS_1_1 & T1_OSC1EN_OFF &
			   T1_SYNC_EXT_ON);
	//CONFIGURA��O DO TIMER1

	OpenTimer2(TIMER_INT_ON & T2_PS_1_4 & T2_POST_1_10);
	//CINFIGURA��O DO TIMER2

//	OpenPWM1(249);
	//CONFIGURA��O DO M�DULO CCP1 -> PWM
	CCP1CON = 0B00001111;
	PR2 = 249;	

//	OpenPWM2(249);
	//CONFIGURA��O DO M�DULO CCP2 -> PWM
	CCP2CON = 0B00001111;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	SetDCPWM1(0);	//DUTY CYCLE DO PWM1 EM 0
	SetDCPWM2(0);	//DUTY CYCLE DO PWM2 EM 0

	while(RCONbits.NOT_TO);	//AGUARDA ESTOURO DO WDT

	INTCONbits.PEIE = 1;	//LIGA AS INTERRUP��ES
	INTCONbits.GIE = 1;

	inicializa_lcd();	//INICIALIZA O LCD

	MOSTRA_RPS = 0;		//LIMPA OS FLAGS
	MOSTRA_TEMPERATURA = 0;

	comando_lcd(0x80);	//IMPRIME A TELA PRINCIPAL
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

		if(MOSTRA_RPS)					//DEVE MOSTRAR O VALOR DO RPS?
		{
			MOSTRA_RPS = 0;				//LIMPA FLAG
			cont_vent = cont_vent / 7;	//DIVIDE A LEITURA DO TIMER1 POR 7
			converte_bcd((unsigned char)cont_vent);//EFETUA A CONVERS�O EM BCD
			comando_lcd(0xC7);			// posiciona o cursor na linha 1, coluna 7
			escreve_lcd (centena + 0x30);// imprime mensagem no lcd 
			escreve_lcd (dezena + 0x30);
			escreve_lcd (unidade + 0x30);
		}
		
		if(MOSTRA_TEMPERATURA)			//DEVE MOSTRAR A TEMPERATURA?
		{
			MOSTRA_TEMPERATURA = 0;		//LIMPA FLAG
			converte_bcd(tabela_temp[temperatura]);//BUSCA O VALOR CORRETO DA TEMP.
													//E EFETUA A CONVERS�O EM BCD
			comando_lcd(0xCB);			// posiciona o cursor na linha 1, coluna 11
			escreve_lcd (centena + 0x30);// imprime mensagem no lcd 
			escreve_lcd (dezena + 0x30);
			escreve_lcd (unidade + 0x30);

			while(BusyUSART());			//AGUARDA BUFFER DE TRANSMISS�O FICAR VAZIO
			WriteUSART(temperatura);	//ENVIA TEMPERATURA � USART
		}

		if(!BT1)						//BOT�O 1 PRESSIONADO?
		{	
			FILTRO--;					//DECREMENTA FILTRO DE BOT�O
			if (!FILTRO)				//FILTRO = 0?
			{
				tempo_turbo--;			//DECREMENTA TURBO
				if (!tempo_turbo)		//TURBO =0?
				{
					if(intensidade_aquec < 100) intensidade_aquec++;
					//SE INTENSIDADE_AQUEC < 100, INCREMENTA
					converte_bcd(intensidade_aquec);	//CONVERTE INTENSIDADE_AQUE EM BCD
					comando_lcd(0xC1);				// posiciona o cursor na linha 1, coluna 1
					escreve_lcd (centena + 0x30);	// imprime mensagem no lcd 
					escreve_lcd (dezena + 0x30);
					escreve_lcd (unidade + 0x30);
					SetDCPWM1((unsigned short)intensidade_aquec*10);//ATUALIZA PWM1
					tempo_turbo = 30;	//REINICIA TURBO DE TECLA
				}
			}
			continue;
		}

		if(!BT2)						//BOT�O 2 PRESSIONADO?
		{	
			FILTRO--;					//DECREMENTA FILTRO DE BOT�O
			if (!FILTRO)				//FILTRO = 0?
			{
				tempo_turbo--;			//DECREMENTA TURBO
				if (!tempo_turbo)		//TURBO =0?
				{
					if(intensidade_aquec > 0) intensidade_aquec--;
					//SE INTENSIDADE_AQUEC > 0, DECREMENTA
					converte_bcd(intensidade_aquec);	//CONVERTE INTENSIDADE_AQUE EM BCD
					comando_lcd(0xC1);				// posiciona o cursor na linha 1, coluna 1
					escreve_lcd (centena + 0x30);	// imprime mensagem no lcd 
					escreve_lcd (dezena + 0x30);
					escreve_lcd (unidade + 0x30);
					SetDCPWM1((unsigned short)intensidade_aquec*10);//ATUALIZA PWM1
					tempo_turbo = 30;	//REINICIA TURBO DE TECLA
				}
			}
			continue;
		}				

		if(!BT3)						//BOT�O 3 PRESSIONADO?
		{	
			FILTRO--;					//DECREMENTA FILTRO DE BOT�O
			if (!FILTRO)				//FILTRO = 0?
			{
				tempo_turbo--;			//DECREMENTA TURBO
				if (!tempo_turbo)		//TURBO =0?
				{
					if(intensidade_vent < 100) intensidade_vent++;
					//SE INTENSIDADE_VENT < 100, INCREMENTA
					SetDCPWM2((unsigned short)intensidade_vent*10);//ATUALIZA PWM2
					tempo_turbo = 30;	//REINICIA TURBO DE TECLA
				} 		
			}
			continue;
		}

		if(!BT4)						//BOT�O 4 PRESSIONADO?
		{	
			FILTRO--;					//DECREMENTA FILTRO DE BOT�O
			if (!FILTRO)				//FILTRO = 0?
			{
				tempo_turbo--;			//DECREMENTA TURBO
				if (!tempo_turbo)		//TURBO =0?
				{
					if(intensidade_vent > 0) intensidade_vent--;
					//SE INTENSIDADE_VENT > 0, DECREMENTA
					SetDCPWM2((unsigned short)intensidade_vent*10);//ATUALIZA PWM2
					tempo_turbo = 30;	//REINICIA TURBO DE TECLA
				}
			}
			continue;
		}
		FILTRO = 255;//REINICIA FILTRO DE BOT�O
	}
}

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
