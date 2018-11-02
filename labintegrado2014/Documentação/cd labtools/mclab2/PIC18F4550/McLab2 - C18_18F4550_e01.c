/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        Programação em C18 - Exemplos usando o PIC18F4550 e McLab2       *
 *                               Exemplo 1                                 *
 *                                                                         *
 *                   LABTOOLS - MOSAICO DIDACTIC DIVISION                  *
 *                                                                         *
 *  TEL: (0XX11) 4992-8775               SITE: www.labtools.com.br         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   VERSÃO : 1.0                                                          *
 *   DATA : 01/08/2006 				                                  	   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             Descrição geral                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
	ESTE SOFTWARE ESTÁ PREPARADO PARA LER QUATRO BOTÕES E TOCAR O BUZZER COM
	DURAÇÃO VARIÁVEL CONFORME A TECLA PRESSIONADA, ALÉM DE ACENDER O LED
	INDICANDO A ÚLTIMA TECLA PRESSIONADA.
*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                		DEFINIÇÃO PIC			         				 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>        	 // Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <pwm.h>          		 //PWM library functions
#include <adc.h>           		 //ADC library functions
#include <timers.h>        		 //Timer library functions
#include <delays.h>           	 //Delay library functions
#include <i2c.h>           		 //I2C library functions
#include <stdlib.h>           	 //Library functions
#include <usart.h>           	 //USART library functions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Configurações para gravação                   *
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
 *              Definição e inicialização das variáveis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//Neste bloco estão definidas as variáveis globais do programa.

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

#define		STATUS_LEDS		STATUS_LEDSbits.BITS	//variável STATUS_LEDS
#define		STATUS_BOTOES 	STATUS_BOTOESbits.BITS	//variável STATUS_BOTOES

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.
#define 	MIN			0
#define 	MAX			15
#define 	t_filtro	25


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         TABELA DE CONSTANTES                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const rom unsigned char tabela[16] = {255,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUNÇÕES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TRATA_HIGH_INT(void);
void TRATA_INT_TIMER0(void);
void TRATA_INT_TIMER2(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SAÍDAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMAÇÃO E
//FUTURAS ALTERAÇÕES DO HARDWARE.


#define 	botao1 		STATUS_BOTOESbits.BIT0	//BOTÃO 1
												//1 -> PRESSIONADO
												//0 -> LIBERADO

#define 	botao2 		STATUS_BOTOESbits.BIT1	//BOTÃO 2
												//1 -> PRESSIONADO
												//0 -> LIBERADO

#define 	botao3		STATUS_BOTOESbits.BIT2	//BOTÃO 3  
												//1 -> PRESSIONADO
												//0 -> LIBERADO

#define 	botao4 		STATUS_BOTOESbits.BIT3	//BOTÃO 4
												//1 -> PRESSIONADO
												//0 -> LIBERADO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SAÍDAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SAÍDAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMAÇÃO E
//FUTURAS ALTERAÇÕES DO HARDWARE.

#define buzzer	PORTAbits.RA5					//BUZZER
												//1 -> LIGADO
												//0 -> DESLIGADO

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *             FUNÇÃO DE TRATAMENTO DA INTERRUPÇÃO DO TIMER 2              *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TRATA_INT_TIMER2(void)
{
	PIR1bits.TMR2IF = 0;	//LIMPA FLAG DE INTERRUPÇÃO

	if (buzzer != 0)		// o buzzer está ligado ?
	{
		buzzer = 0;			// sim, então desliga
	}
	else
	{
		buzzer = 1;			// não, então liga
	}
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// *             FUNÇÃO DE TRATAMENTO DA INTERRUPÇÃO DO TIMER 0              *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void TRATA_INT_TIMER0(void)
{
	INTCONbits.TMR0IF = 0;			//LIMPA FLAG DE INTERRUPÇÃO


	TRISB = TRISB | 0b00001111;     //CONFIG DIREÇÃO DOS PINOS PORTB
	
	Delay1TCY();					//DELAY DE 4 CICLOS DE MÁQUINA
	Delay1TCY();					//PARA ESTABILIZAÇÃO DO PORT
	Delay1TCY();
	Delay1TCY();

	STATUS_BOTOES = (~PORTB) & 0b00001111;	//COMPLEMENTA O PORTB, APLICA UMA
											//MÁSCARA E SALVA O CONTEÚDO EM
											//STATUS BOTÕES

	TRISB = TRISB & 0b11110000;		//CONFIG DIREÇÃO DOS PINOS PORTB

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 1                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	if (botao1)						// testa botão 1
	{								// botão 1 está pressionado ?
		filtro1--;					// Sim, então decrementa o filtro
		if (filtro1 == 0)			// acabou o filtro do botão 1 ?
		{
			STATUS_LEDSbits.BIT0 = 1;// marca que o botão está pressionado
		}
	}
	else
	{								// botão 1 liberado
		filtro1 = t_filtro;			// inicia o filtro do botão 1
		STATUS_LEDSbits.BIT0 = 0;	// marca que o botão foi liberado
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 2                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	if (botao2)						// testa botão 2
	{								// botão 2 está pressionado ?
		filtro2--;					// Sim, então decrementa o filtro
		if (filtro2 == 0)			// fim do filtro do botão 2 ?
		{
			STATUS_LEDSbits.BIT1 = 1;// marca que o botão está pressionado
		}
	}
	else
	{								// botão 2 liberado
		filtro2 = t_filtro;			// inicia o filtro do botão 2
		STATUS_LEDSbits.BIT1 = 0; 	// marca que o botão foi liberado
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 3                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	if (botao3)						// testa botão 3
	{								// botão 3 está pressionado ?
		filtro3--;					// Sim, então decrementa o filtro
		if (filtro3 == 0)			// fim do filtro do botão 3 ?
		{
			STATUS_LEDSbits.BIT2 = 1;// marca que o botão está pressionado
		}
	}
	else
	{								// botão 3 liberado
		filtro3 = t_filtro;			// inicia o filtro do botão 3
		STATUS_LEDSbits.BIT2 = 0; 	// marca que o botão foi liberado
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Tratamento do Botão 4                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	if (botao4)						// testa botão 4
	{								// botão 4 está pressionado ?
		filtro4--;					// Sim, então decrementa o filtro
		if (filtro4 == 0)			// fim do filtro do botão 4 ?
		{
			STATUS_LEDSbits.BIT3 = 1;// marca que o botão está pressionado
		}
	}
	else
	{								// botão 4 liberado
		filtro4 = t_filtro;			// inicia o filtro do botão 4
		STATUS_LEDSbits.BIT3 = 0; 	// marca que o botão foi liberado
	}

	PORTB = STATUS_LEDS;			//ENVIA PARA O PORTB AS TECLAS TRATADAS
	
	if (STATUS_LEDS == 0)			//ALGUMA TECLA PRESSIONADA?
	{
		PR2 = 0xFF;					//SE SIM, CARREGUE PRS COM 255
		CloseTimer2();				//DESABILITAÇÃO DO TIMER2
		buzzer = 0;					//DESLIGA O BUZZER
	}
	else
	{								//SE NÃO
		PR2 = tabela[STATUS_LEDS];	// consulta tabela e inicializa timer2.
		OpenTimer2(TIMER_INT_ON & T2_PS_1_1 & T2_POST_1_2);//LIGA O TIMER2
	}
}	

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Função Principal                         *
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

	TRISA = 0b11011111;                 //CONFIG DIREÇÃO DOS PINOS PORTA
	TRISB = 0b00001111;                 //CONFIG DIREÇÃO DOS PINOS PORTB
	TRISC = 0b11111111;		            //CONFIG DIREÇÃO DOS PINOS PORTC
	TRISD = 0b11111111;                 //CONFIG DIREÇÃO DOS PINOS PORTD
	TRISE = 0b00000111;                 //CONFIG DIREÇÃO DOS PINOS PORTE		

	ADCON1 = 0b00001111;                //DESLIGA CONVERSORES A/D

	OpenTimer0(TIMER_INT_ON & T0_8BIT & T0_SOURCE_INT & T0_PS_1_8);
	//CONFIGURAÇÃO DO TIMER0

	PR2 = 0xFF;
	OpenTimer2(TIMER_INT_OFF & T2_PS_1_1 & T2_POST_1_2);
	//CONFIGURAÇÃO DO TIMER2

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	while(RCONbits.NOT_TO);		//AGUARDA ESTOURO DO WDT

 	STATUS_LEDS = 0;			//LIMPA STATUS LEDS

	filtro1 = t_filtro;			// inicia filtro do botao1
	filtro2 = t_filtro;			// inicia filtro do botao2
	filtro3 = t_filtro;			// inicia filtro do botao3
	filtro4 = t_filtro;			// inicia filtro do botao4

	INTCONbits.TMR0IE = 1;		// liga chave da int. do timer 0
	PIE1bits.TMR2IE	= 1;		// liga chave da int. do timer 2
	INTCONbits.PEIE = 1;		// liga chave de periféricos
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
// *             FUNÇÕES DE TRATAMENTO DE INT DE ALTA PRIORIDADE             *
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
