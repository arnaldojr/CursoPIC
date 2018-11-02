/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        Programa��o em C18 - Exemplos usando o PIC18F4550 e McLab2       *
 *                               Exemplo 6                                 *
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
	ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DO M�DULO PWM
	DO PIC18F452. ELE MONITORA OS QUATRO BOT�ES E CONFORME O BOT�O SELECIONADO
	APLICA UM VALOR DIFERENTE NO PWM, FAZENDO ASSIM UM CONTROLE SOBRE A
	VELOCIDADE DO VENTILADOR. NO LCD � MOSTRADO O VALOR ATUAL DO DUTY CYCLE.
*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                		DEFINI��O PIC			         				 *
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
//Este programa n�o utiliza nenhuma vari�vel de usu�rio

unsigned short	FILTRO1;		//FILTRO BOT�O 1
unsigned short	FILTRO2;		//FILTRO BOT�O 2
unsigned short	FILTRO3;		//FILTRO BOT�O 3
unsigned short	FILTRO4;		//FILTRO BOT�O 4

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declara��o dos flags de software                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de flags ajuda na programa��o e economiza mem�ria RAM.
//Este programa n�o utiliza nenhum flag de usu�rio

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

#define	ST_BT1		FLAGSbits.BIT0		//STATUS DO BOT�O 1
#define	ST_BT2		FLAGSbits.BIT1		//STATUS DO BOT�O 2
#define	ST_BT3		FLAGSbits.BIT2		//STATUS DO BOT�O 3
#define	ST_BT4		FLAGSbits.BIT3		//STATUS DO BOT�O 4

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUN��ES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void comando_lcd(unsigned char caracter);
void escreve_lcd(unsigned char caracter);
void limpa_lcd(void);
void inicializa_lcd(void);
void escreve_frase(const rom char *frase);
void tela_principal(void);

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
 *	                 		  Tela Principal					           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_principal(void)
{
	limpa_lcd();				// limpa lcd
	comando_lcd(0x86);			// posiciona o cursor na linha 0, coluna 0
								// imprime mensagem no lcd 
	escreve_frase("PWM");
	
	comando_lcd(0xC6);	// posiciona o cursor na linha 0, coluna 0
						// imprime mensagem no lcd 
	escreve_frase("OFF ");	
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	                 		  Tela Tecla 1  					           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_tecla_1(void)
{
	comando_lcd(0xC6);	// posiciona o cursor na linha 1, coluna 6
						// imprime mensagem no lcd 
	escreve_frase("OFF ");	
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	                 		  Tela Tecla 2  					           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_tecla_2(void)
{
	comando_lcd(0xC6);	// posiciona o cursor na linha 1, coluna 6
						// imprime mensagem no lcd 
	escreve_frase("50% ");
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	                 		  Tela Tecla 3  					           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_tecla_3(void)
{
	comando_lcd(0xC6);	// posiciona o cursor na linha 1, coluna 6
						// imprime mensagem no lcd 
	escreve_frase("75% ");
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	                 		  Tela Tecla 4  					           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_tecla_4(void)
{
	comando_lcd(0xC6);	// posiciona o cursor na linha 1, coluna 6
						// imprime mensagem no lcd 
	escreve_frase("100%");
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


	TRISA = 0b11111111;                 //CONFIG DIRE��O DOS PINOS PORTA
	TRISB = 0b00001111;                 //CONFIG DIRE��O DOS PINOS PORTB
	TRISC = 0b11111101;		            //CONFIG DIRE��O DOS PINOS PORTC
	TRISD = 0b00000000;                 //CONFIG DIRE��O DOS PINOS PORTD
	TRISE = 0b00000100;                 //CONFIG DIRE��O DOS PINOS PORTE		

	ADCON1 = 0b00001111;                //DESLIGA CONVERSORES A/D

	while(RCONbits.NOT_TO);				//AGUARDA ESTOURO DO WDT

//	OpenPWM2(255);
	CCP2CON = 0b00001111;
	PR2 = 255;
	//CONFIGURA M�DULO CCP2 COMO PWM
	OpenTimer2(TIMER_INT_OFF & T2_PS_1_16 & T2_POST_1_1);
	//CONFIGURA O TIMER 2
	SetDCPWM2(0);
	//DESLIGA O MOTOR

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	inicializa_lcd();
	tela_principal();
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	while(1)
	{
		ClrWdt();				//LIMPA O WDT

		if(!BT1)					//BOT�O 1 PRESSIONADO?
		{	
			if (--FILTRO1 == 0)		//TERMINOU FILTRO DE BOT�O?
			{
				if (!ST_BT1)		//A��O DO BOT�O FOI EXECUTADA?
				{
					tela_tecla_1();	//IMPRIME MENSAGEM DE BOT�O 1
					ST_BT1 = 1;		//SETA FLAG DE ESTADO DO BOT�O 1
					SetDCPWM2(0);	//DUTY CYCLE EM 0%
				}
			}		
			continue;
		}		
		else ST_BT1 = 0; // BOT�O LIBERADO, LIMPA O FLAG
		

		if(!BT2)					//BOT�O 2 PRESSIONADO?
		{	
			if (--FILTRO2 == 0)		//TERMINOU FILTRO DE BOT�O?
			{
				if (!ST_BT2)		//A��O DO BOT�O FOI EXECUTADA?
				{
					tela_tecla_2();	//IMPRIME MENSAGEM DE BOT�O 2
					ST_BT2 = 1;		//SETA FLAG DE ESTADO DO BOT�O 2
					SetDCPWM2(512);	//DUTY CYCLE EM 50%
				}
			}
			continue;
		}		
		else ST_BT2 = 0; // BOT�O LIBERADO, LIMPA O FLAG
		
		if(!BT3)					//BOT�O 3 PRESSIONADO?
		{	
			if (--FILTRO3 == 0)		//TERMINOU FILTRO DE BOT�O?
			{
				if (!ST_BT3)		//A��O DO BOT�O FOI EXECUTADA?
				{
					tela_tecla_3();	//IMPRIME MENSAGEM DE BOT�O 3
					ST_BT3 = 1;		//SETA FLAG DE ESTADO DO BOT�O 3
					SetDCPWM2(767);	//DUTY CYCLE EM 75%
				}
			}		
			continue;
		}		
		else ST_BT3 = 0; // BOT�O LIBERADO, LIMPA O FLAG
		


		if(!BT4)					//BOT�O 4 PRESSIONADO?
		{	
			if (--FILTRO4 == 0)		//TERMINOU FILTRO DE BOT�O?
			{
				if (!ST_BT4)		//A��O DO BOT�O FOI EXECUTADA?
				{
					tela_tecla_4();	//IMPRIME MENSAGEM DE BOT�O 4
					ST_BT4 = 1;		//SETA FLAG DE ESTADO DO BOT�O 4
					SetDCPWM2(1023);//DUTY CYCLE EM 100%
				}	
			}		
			continue;
		}		
		else ST_BT4 = 0; // BOT�O LIBERADO, LIMPA O FLAG

		FILTRO1 = 400;			//RECARREGA FILTRO BT1
		FILTRO2 = 400;			//RECARREGA FILTRO BT2
		FILTRO3 = 400;			//RECARREGA FILTRO BT3
		FILTRO4 = 400;			//RECARREGA FILTRO BT4
		
	}// FIM LA�O PRINCIPAL
}
