/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        Programa��o em C18 - Exemplos usando o PIC18F4550 e McLab2       *
 *                               Exemplo 2                                 *
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
	ESTE EXEMPLO FOI PREPARADO PARA DEMOSNTRAR O FUNCIONAMENTO DO TIMER DE
	16 BITS DO PIC (TMR1) E DA VARREDURA DE DISPLAYS MAIS BARRA DE LEDS.
	CONSISTE NUM CONTADOR DE SEGUNDOS. DOIS BOT�ES FORAM UTILIZADOS PARA
	PROGRAMAR O TEMPO DA CONTAGEM. UM OUTRO BOT�O FOI UTILIZADO PARA DISPARAR
	O CONTADOR. O TEMPORIZADOR CONSEGUE CONTAR AT� 9999 SEGUNDOS, DE FORMA QUE
	OS 4 DISPLAYS DE 7 SEGMENTOS FORAM NECESS�RIOS. A CONTAGEM � REGRESSIVA.
	UMA BARRA DE LEDS INDICA QUE O TEMPORIZADOR EST� OPERANDO. QUANDO O SISTEMA
	CHEGA A 0000 A BARRA DE LEDS � DESLIGADA AUTOMATICAMENTE.
*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                		DEFINI��O PIC			        				 *
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
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.
#define	t_filtro		250
#define turbo_tecla		60
#define delta_timer1 	(65536 - 62500)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Defini��o e inicializa��o das vari�veis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco est�o definidas as vari�veis globais do programa.

unsigned char display = 0;				// atualiza os displays
unsigned char contador = 2;				// contador de interrup��es de timer1
unsigned char filtro  = t_filtro;		// inicia filtro dos bot�es
unsigned char filtro1 = t_filtro;		// inicia filtro dos bot�es
unsigned char filtro2 = t_filtro;		// inicia filtro dos bot�es
unsigned char filtro3 = t_filtro;		// inicia filtro dos bot�es
unsigned char filtro4 = t_filtro;		// inicia filtro dos bot�es
unsigned char turbo = 1;				// inicia turbo das teclas
unsigned char unidade = 9;				// vari�vel unidade do timer de 1 seg
unsigned char dezena  = 9;				// vari�vel dezena do timer de 1 seg
unsigned char centena = 9;				// vari�vel centena do timer de 1 seg
unsigned char milhar  = 9;				// vari�vel milhar do timer de 1 seg

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *                    TABELA DE CONVERS�O BIN�RIO -> DISPLAY               *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ESTA ROTINA IR� RETORNAR EM W, O SIMBOLO CORRETO QUE DEVE SER
// MOSTRADO NO DISPLAY PARA CADA VALOR DE INTENSIDADE. O RETORNO J� EST�
// FORMATADO PARA AS CONDI��ES DE LIGA��O DO DISPLAY AO PORTD.
//	   a
//     **********
//     *        *
//   f *        * b
//     *    g   *
//     **********
//     *        *
//   e *        * c
//     *    d   *
//     **********  *.

const rom unsigned char converte[] =
{//  .GFEDCBA	 POSI��O CORRETA DOS SEGMENTOS
   0b00111111,	// 00 - RETORNA S�MBOLO CORRETO 0
   0b00000110,	// 01 - RETORNA S�MBOLO CORRETO 1
   0b01011011,	// 02 - RETORNA S�MBOLO CORRETO 2
   0b01001111,	// 03 - RETORNA S�MBOLO CORRETO 3
   0b01100110,	// 04 - RETORNA S�MBOLO CORRETO 4	
   0b01101101,	// 05 - RETORNA S�MBOLO CORRETO 5	
   0b01111101,	// 06 - RETORNA S�MBOLO CORRETO 6	
   0b00000111,	// 07 - RETORNA S�MBOLO CORRETO 7	
   0b01111111,	// 08 - RETORNA S�MBOLO CORRETO 8	
   0b01101111,	// 09 - RETORNA S�MBOLO CORRETO 9	
   0b00000000}; // BLANK	

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declara��o dos flags de software                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de flags ajuda na programa��o e economiza mem�ria RAM.
//Este programa n�o utiliza nenhum flag de usu�rio

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUN��ES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TRATA_HIGH_INT(void);
void TRATA_INT_TIMER0(void);
void TRATA_INT_TIMER1(void);
void decrementa_timer(void);
void incrementa_timer(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SA�DAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMA��O E
//FUTURAS ALTERA��ES DO HARDWARE.


#define BT_UP			PORTBbits.RB1	//BT_UP
										//0 -> PRESSIONADO
										//1 -> LIBERADO

#define BT_DOWN 		PORTBbits.RB2	//BT_DOWN
										//0 -> PRESSIONADO
										//1 -> LIBERADO

#define BT_START_STOP	PORTBbits.RB3	//BT_START_STOP
										//0 -> PRESSIONADO
										//1 -> LIBERADO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SA�DAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SA�DAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMA��O E
//FUTURAS ALTERA��ES DO HARDWARE.

#define	ESTADO_TIMER	PORTBbits.RB0	// LED INDICATIVO DE TIMER LIGADO

#define	disp0			PORTBbits.RB4	// sele��o do display unidade	(0)
#define	disp1 			PORTBbits.RB5	// sele��o do display dezena	(1)
#define	disp2 			PORTBbits.RB6	// sele��o do display centena	(2)
#define	disp3 			PORTBbits.RB7	// sele��o do display milhar	(3)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  Fun��o de decremento do Timer                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void decrementa_timer(void)
{
	unidade --;				//DECREMENTA UNIDADE
	if (unidade == 0xFF)	//UNIDADE = 255?
	{
		unidade = 9;		//UNIDADE = 9
		dezena --;			//DECREMENTA DEZENA
	}
	if (dezena == 0xFF)		//DEZENA = 255?
	{
		dezena = 9;			//DEZENA = 9
		centena --;			//DECREMENTA CENTENA
	}
	if (centena == 0xFF)	//CENTENA = 255?
	{
		centena = 9;		//CENTENA = 9
		milhar --;			//DECREMENTA MILHAR
	}
	if (milhar == 0xFF)		//MILHAR = 255?
	{
		milhar = 9;			//MILHAR = 9
	}
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  Fun��o de incremento do Timer                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void incrementa_timer(void)
{
	unidade ++;				//INCREMENTA UNIDADE
	if (unidade == 10)		//UNIDADE = 10?
	{
		unidade = 0;		//UNIDADE = 0
		dezena ++;			//INCREMENTA DEZENA
	}
	if (dezena == 10)		//DEZENA = 10?
	{
		dezena = 0;			//DEZENA = 0
		centena ++;			//INCREMENTA CENTENA
	}
	if (centena == 10)		//CENTENA = 10?
	{
		centena = 0;		//CENTENA = 0
		milhar ++;			//INCREMENTA MILHAR
	}
	if (milhar == 10)		//MILHAR = 10?
	{
		milhar = 0;			//MILHAR = 0
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Fun��o de tratamento de interrup��o de Timer1          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// Esta interrupcao ocorrera a cada 0,5 segundos.

void TRATA_INT_TIMER1(void)
{
	PIR1bits.TMR1IF = 0;			// limpa flag de interrup��o
	WriteTimer1(delta_timer1);		// carraga timer1
	contador --;					// decrementa contador de interrup��es
	if (contador == 0)
	{
		WriteTimer1(delta_timer1);	// carraga timer1
		contador = 2;				// carrega contador de int
		decrementa_timer();
		if ((unidade == 0)&&(dezena == 0)&&(centena == 0)&&(milhar == 0))// timer est� zerado?
		{
			ESTADO_TIMER = 0;		// sim, apaga o led e
			CloseTimer1();			// desliga timer1
		}
	}
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Rotina de Tratamento de interrup��o de TMR0            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// Esta interrup��o ocorrer� a cada 256us.

void TRATA_INT_TIMER0(void)
{
	INTCONbits.TMR0IF = 0;	//LIMPA FLAG DE INTERRUP��O
	switch(display)			// in�cio da varredura dos display�s
	{
		case 0:
		display++;			// incrementa a vari�vel de varredura
		disp3 = 0;			// liga o display 3
		PORTD = converte[unidade];	// atualiza o portd
		disp0 = 1;			// liga o display 0
		break;				// sai

		case 1:
		display++;			// incrementa a vari�vel de varredura
		disp0 = 0;			// desliga o display 0
		PORTD = converte[dezena];	// atualiza o portd
		disp1 = 1;			// liga o display 1
		break;				// sai

		case 2:
		display++;			// incrementa a vari�vel de varredura
		disp1 = 0;			// desliga o display 1
		PORTD = converte[centena];	// atualiza o portd
		disp2 = 1;			// liga o display 2
		break;				// sai

		case 3:
		display = 0;		// inicia em 0 a vari�vel de varredura
		disp2 = 0;			// desliga o display 2
		PORTD = converte[milhar];	// atualiza o portd
		disp3 = 1;			// liga o display 3
		break;				// sai
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

	TRISA = 0b11111111;                 //CONFIG DIRE��O DOS PINOS PORTA
	TRISB = 0b00001110;                 //CONFIG DIRE��O DOS PINOS PORTB
	TRISC = 0b11111111;		            //CONFIG DIRE��O DOS PINOS PORTC
	TRISD = 0b00000000;                 //CONFIG DIRE��O DOS PINOS PORTD
	TRISE = 0b00000111;                 //CONFIG DIRE��O DOS PINOS PORTE		

	ADCON1 = 0b00001111;                //DESLIGA CONVERSORES A/D

	OpenTimer0(TIMER_INT_ON & T0_8BIT & T0_SOURCE_INT & T0_PS_1_2);
	//CONFIGURA��O DO TIMER0
	OpenTimer1(TIMER_INT_OFF & T1_SOURCE_INT & T1_8BIT_RW & T1_PS_1_8);
	//CONFIGURA��O DO TIMER1

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	while(RCONbits.NOT_TO);	//AGUARDA ESTOURO DO WDT

	ESTADO_TIMER = 0;		//LIMPA ESTADO_TIMER

	INTCONbits.PEIE = 1;	//LIGA CHAVE DOS PERIF�RICOS
	INTCONbits.GIE = 1;		//LIGA A CHAVE GERAL
	CloseTimer1();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Loop principal                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	while(1)
	{
		ClrWdt();			//LIMPA O WDT
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 1                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		if(!BT_UP)							// testa bot�o 1
		{									// bot�o 1 est� pressionado ?
			if(ESTADO_TIMER == 0)			// o timer est� parado?
			{
				filtro --;					// decrementa o filtro
				if(filtro == 0) 			// fim do filtro do bot�o?
				{
					turbo --;				// decrementa o turbo da tecla
					if(turbo == 0)			// sim, fim do turbo do bot�o ?
					{
						turbo = turbo_tecla;// carrega o turbo
						incrementa_timer(); // incrementa o timer
					}
				}
			}
			continue;
		}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 2                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		if(!BT_DOWN)						// testa bot�o 2
		{									// bot�o 2 est� pressionado ?
			if(ESTADO_TIMER == 0)			// o timer est� parado?
			{
				filtro --;					// decrementa o filtro
				if(filtro == 0) 			// fim do filtro do bot�o?
				{
					turbo --;				// decrementa o turbo da tecla
					if(turbo == 0)			// sim, fim do turbo do bot�o ?
					{	
						turbo = turbo_tecla;// carrega o turbo
						decrementa_timer(); // decrementa o timer
					}
				}
			}
			continue;
		}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Bot�o 3                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		if(!BT_START_STOP)					// testa bot�o 3
		{									// bot�o 3 est� pressionado ?	
			if(filtro != 0)					// o timer est� parado?
			{
				filtro --;					// decrementa o filtro
				if(filtro == 0) 			// fim do filtro do bot�o?
				{
					if(ESTADO_TIMER != 0)	// sim, o timer est� ligado ?
					{
						ESTADO_TIMER = 0;	// indica timer parado
						CloseTimer1();		// desliga o tratamento de interrup��o de timer1
					}
					else
					{
						if((unidade!=0)||(dezena!=0)||(centena!=0)||(milhar!=0))// timer est� zerado?
						{
							ESTADO_TIMER = 1;			// n�o, indica que o timer est� contando
							contador = 2;				// carrega contador auxiliar
							WriteTimer1(delta_timer1);	// carraga timer1
							PIR1bits.TMR1IF = 0;		// limpa flag de int tmr1
							OpenTimer1(TIMER_INT_ON & T1_SOURCE_INT & T1_8BIT_RW & T1_PS_1_8);
							// liga o tratamento de interrup��o de timer1
						}
					}
				}
				continue;
			}
		}
		filtro = t_filtro;	//RECARREGA FILTRO DE BOT�ES
		turbo = 1;
	}
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *             ROTINA DE TRATAMENTO DE INT DE ALTA PRIORIDADE              *
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
	if(INTCONbits.TMR0IF)	TRATA_INT_TIMER0();
	if(PIR1bits.TMR1IF)		TRATA_INT_TIMER1();
}

