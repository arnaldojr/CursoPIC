/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        Programa��o em C18 - Exemplos usando o PIC18F4550 e McLab2       *
 *                               Exemplo 1                                 *
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
	ESTE SOFTWARE EST� PREPARADO PARA LER QUATRO BOT�ES E TOCAR O BUZZER COM
	DURA��O VARI�VEL CONFORME A TECLA PRESSIONADA, AL�M DE ACENDER O LED
	INDICANDO A �LTIMA TECLA PRESSIONADA.
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
#include <delays.h>           	 //Delay library functions
#include <i2c.h>           		 //I2C library functions
#include <stdlib.h>           	 //Library functions
#include <usart.h>           	 //USART library functions

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
 *              Defini��o e inicializa��o das vari�veis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//Neste bloco est�o definidas as vari�veis globais do programa.

unsigned char filtro1;
unsigned char filtro2;
unsigned char filtro3;
unsigned char filtro4;

union bits
{
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
    } ;
    struct
	{
		unsigned BITS:8;	    
    } ;    
};

union bits STATUS_BOTOESbits;				
union bits STATUS_LEDSbits;				

#define		STATUS_LEDS		STATUS_LEDSbits.BITS	//vari�vel STATUS_LEDS
#define		STATUS_BOTOES 	STATUS_BOTOESbits.BITS	//vari�vel STATUS_BOTOES

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.
#define 	MIN			0
#define 	MAX			15
#define 	t_filtro	25


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         TABELA DE CONSTANTES                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const rom unsigned char tabela[16] = {255,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUN��ES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TRATA_HIGH_INT(void);
void TRATA_INT_TIMER0(void);
void TRATA_INT_TIMER2(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SA�DAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMA��O E
//FUTURAS ALTERA��ES DO HARDWARE.


#define 	botao1 		STATUS_BOTOESbits.BIT0	//BOT�O 1
												//1 -> PRESSIONADO
												//0 -> LIBERADO

#define 	botao2 		STATUS_BOTOESbits.BIT1	//BOT�O 2
												//1 -> PRESSIONADO
												//0 -> LIBERADO

#define 	botao3		STATUS_BOTOESbits.BIT2	//BOT�O 3  
												//1 -> PRESSIONADO
												//0 -> LIBERADO

#define 	botao4 		STATUS_BOTOESbits.BIT3	//BOT�O 4
												//1 -> PRESSIONADO
												//0 -> LIBERADO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SA�DAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SA�DAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMA��O E
//FUTURAS ALTERA��ES DO HARDWARE.

#define buzzer	PORTAbits.RA5					//BUZZER
												//1 -> LIGADO
												//0 -> DESLIGADO

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *             FUN��O DE TRATAMENTO DA INTERRUP��O DO TIMER 2              *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TRATA_INT_TIMER2(void)
{
	PIR1bits.TMR2IF = 0;	//LIMPA FLAG DE INTERRUP��O

	if (buzzer != 0)		// o buzzer est� ligado ?
	{
		buzzer = 0;			// sim, ent�o desliga
	}
	else
	{
		buzzer = 1;			// n�o, ent�o liga
	}
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *             FUN��O DE TRATAMENTO DA INTERRUP��O DO TIMER 0              *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TRATA_INT_TIMER0(void)
{
	INTCONbits.TMR0IF = 0;			//LIMPA FLAG DE INTERRUP��O


	TRISB = TRISB | 0b00001111;     //CONFIG DIRE��O DOS PINOS PORTB
	
	Delay1TCY();					//DELAY DE 4 CICLOS DE M�QUINA
	Delay1TCY();					//PARA ESTABILIZA��O DO PORT
	Delay1TCY();
	Delay1TCY();

	STATUS_BOTOES = (~PORTB) & 0b00001111;	//COMPLEMENTA O PORTB, APLICA UMA
											//M�SCARA E SALVA O CONTE�DO EM
											//STATUS BOT�ES

	TRISB = TRISB & 0b11110000;		//CONFIG DIRE��O DOS PINOS PORTB

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 1                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	if (botao1)						// testa bot�o 1
	{								// bot�o 1 est� pressionado ?
		filtro1--;					// Sim, ent�o decrementa o filtro
		if (filtro1 == 0)			// acabou o filtro do bot�o 1 ?
		{
			STATUS_LEDSbits.BIT0 = 1;// marca que o bot�o est� pressionado
		}
	}
	else
	{								// bot�o 1 liberado
		filtro1 = t_filtro;			// inicia o filtro do bot�o 1
		STATUS_LEDSbits.BIT0 = 0;	// marca que o bot�o foi liberado
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 2                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	if (botao2)						// testa bot�o 2
	{								// bot�o 2 est� pressionado ?
		filtro2--;					// Sim, ent�o decrementa o filtro
		if (filtro2 == 0)			// fim do filtro do bot�o 2 ?
		{
			STATUS_LEDSbits.BIT1 = 1;// marca que o bot�o est� pressionado
		}
	}
	else
	{								// bot�o 2 liberado
		filtro2 = t_filtro;			// inicia o filtro do bot�o 2
		STATUS_LEDSbits.BIT1 = 0; 	// marca que o bot�o foi liberado
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 3                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	if (botao3)						// testa bot�o 3
	{								// bot�o 3 est� pressionado ?
		filtro3--;					// Sim, ent�o decrementa o filtro
		if (filtro3 == 0)			// fim do filtro do bot�o 3 ?
		{
			STATUS_LEDSbits.BIT2 = 1;// marca que o bot�o est� pressionado
		}
	}
	else
	{								// bot�o 3 liberado
		filtro3 = t_filtro;			// inicia o filtro do bot�o 3
		STATUS_LEDSbits.BIT2 = 0; 	// marca que o bot�o foi liberado
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 4                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	if (botao4)						// testa bot�o 4
	{								// bot�o 4 est� pressionado ?
		filtro4--;					// Sim, ent�o decrementa o filtro
		if (filtro4 == 0)			// fim do filtro do bot�o 4 ?
		{
			STATUS_LEDSbits.BIT3 = 1;// marca que o bot�o est� pressionado
		}
	}
	else
	{								// bot�o 4 liberado
		filtro4 = t_filtro;			// inicia o filtro do bot�o 4
		STATUS_LEDSbits.BIT3 = 0; 	// marca que o bot�o foi liberado
	}

	PORTB = STATUS_LEDS;			//ENVIA PARA O PORTB AS TECLAS TRATADAS
	
	if (STATUS_LEDS == 0)			//ALGUMA TECLA PRESSIONADA?
	{
		PR2 = 0xFF;					//SE SIM, CARREGUE PRS COM 255
		CloseTimer2();				//DESABILITA��O DO TIMER2
		buzzer = 0;					//DESLIGA O BUZZER
	}
	else
	{								//SE N�O
		PR2 = tabela[STATUS_LEDS];	// consulta tabela e inicializa timer2.
		OpenTimer2(TIMER_INT_ON & T2_PS_1_1 & T2_POST_1_2);//LIGA O TIMER2
	}
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

	TRISA = 0b11011111;                 //CONFIG DIRE��O DOS PINOS PORTA
	TRISB = 0b00001111;                 //CONFIG DIRE��O DOS PINOS PORTB
	TRISC = 0b11111111;		            //CONFIG DIRE��O DOS PINOS PORTC
	TRISD = 0b11111111;                 //CONFIG DIRE��O DOS PINOS PORTD
	TRISE = 0b00000111;                 //CONFIG DIRE��O DOS PINOS PORTE		

	ADCON1 = 0b00001111;                //DESLIGA CONVERSORES A/D

	OpenTimer0(TIMER_INT_ON & T0_8BIT & T0_SOURCE_INT & T0_PS_1_8);
	//CONFIGURA��O DO TIMER0

	PR2 = 0xFF;
	OpenTimer2(TIMER_INT_OFF & T2_PS_1_1 & T2_POST_1_2);
	//CONFIGURA��O DO TIMER2

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	while(RCONbits.NOT_TO);		//AGUARDA ESTOURO DO WDT

 	STATUS_LEDS = 0;			//LIMPA STATUS LEDS

	filtro1 = t_filtro;			// inicia filtro do botao1
	filtro2 = t_filtro;			// inicia filtro do botao2
	filtro3 = t_filtro;			// inicia filtro do botao3
	filtro4 = t_filtro;			// inicia filtro do botao4

	INTCONbits.TMR0IE = 1;		// liga chave da int. do timer 0
	PIE1bits.TMR2IE	= 1;		// liga chave da int. do timer 2
	INTCONbits.PEIE = 1;		// liga chave de perif�ricos
	INTCONbits.GIE = 1;			// liga chave geral
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	while(1)
	{
		ClrWdt();	//LIMPA O WDT
	}				// FIM DO PROGRAMA
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *             FUN��ES DE TRATAMENTO DE INT DE ALTA PRIORIDADE             *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma code VETOR_HIGH_PRIORITY = 0x0008	//VETOR DE ALTA PRIORIDADE
void HIGH_int (void)
{
  _asm goto TRATA_HIGH_INT _endasm
}
#pragma code

#pragma interrupt TRATA_HIGH_INT
void TRATA_HIGH_INT(void)
{
	if(PIR1bits.TMR2IF)		TRATA_INT_TIMER2();
	if(INTCONbits.TMR0IF)	TRATA_INT_TIMER0();
}
