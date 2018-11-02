/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        Programa��o em C18 - Exemplos usando o PIC18F4550 e McLab2       *
 *                               Exemplo 9                                 *
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
	ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DA USART DO PIC.
	O SOFTWARE CONVERTE O CANAL 1 DO CONVERSOR A/D (POTENCI�METRO P2) E MOSTRA
	NO DISPLAY O VALOR CONVERTIDO EM DECIMAL E HAXADECIMAL.
	AL�M DE MOSTRAR O VALOR NO DISPLAY, O SOFTWARE TRANSMITE PELA USART O VALOR
	DA CONVERS�O. OS VALORES RECEBIDOS PELA USART TAMB�M S�O MOSTRADOS NO LCD
	COMO CARACTERES ASCII.

*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                				DEFINI��O PIC			   			     *
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
 *              Defini��o e inicializa��o das vari�veis Globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco est�o definidas as vari�veis globais do programa.

unsigned char	unidade;
unsigned char	dezena;	
unsigned char	centena;
unsigned char	hexa_low;
unsigned char	hexa_high;

unsigned char	converte;
char			usart_rx;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.

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
void comando_lcd(unsigned char caracter);
void escreve_lcd(unsigned char caracter);
void limpa_lcd(void);
void inicializa_lcd(void);
void escreve_frase(const rom char *frase);
void tela_principal(void);
void converte_bcd(unsigned char aux);
void converte_hexadecimal(unsigned char aux);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                ENTRADAS                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programa��o e
//futuras altera��es do hardware.

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
	comando_lcd(0x80);	// posiciona o cursor na linha 0, coluna 0
	escreve_frase("USART:9600,8,n,1");	//IMPRIME MENSAGEM
	comando_lcd(0xC0);	// posiciona o cursor na linha 1, coluna 0
	escreve_frase("TX:   d   h RX: ");	//IMPRIME MENSAGEM
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

void converte_hexadecimal(unsigned char aux)
{
	hexa_low = aux & 0b00001111;
	hexa_high = (aux >> 4) & 0b00001111;
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
	TRISB = 0b11111111;                 //CONFIG DIRE��O DOS PINOS PORTB
	TRISC = 0b10111111;		            //CONFIG DIRE��O DOS PINOS PORTC
	TRISD = 0b00000000;                 //CONFIG DIRE��O DOS PINOS PORTD
	TRISE = 0b00000100;                 //CONFIG DIRE��O DOS PINOS PORTE	

	OpenADC(ADC_FOSC_8 & ADC_LEFT_JUST & ADC_0_TAD, ADC_CH1 & ADC_INT_OFF & ADC_VREFPLUS_VDD & ADC_VREFMINUS_VSS,12);
	//CONFIGURA��O DO AD
	OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE &
	          USART_EIGHT_BIT  & USART_CONT_RX    & USART_BRGH_HIGH,25);
	//CONFIGURA��O DA USART

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	while(RCONbits.NOT_TO);		//aguarda estouro do WDT

	inicializa_lcd();			// configura o lcd

	tela_principal();			//IMPRIME A TELA PRINCIPAL
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
   	ConvertADC();				//Inicia convers�o AD
	
	while(1)
	{
		ClrWdt();				//LIMPA O WDT

		if(BusyADC())			//SE CONVERS�O AD FINALIZADA
		{
			converte = ADRESH;	//GRAVA VALOR DA CONVERS�O EM CONVERTE
			converte_bcd(converte);//FAZ A CONVERS�O EM BCD
			comando_lcd(0xC3);				// posiciona o cursor na linha 1, coluna 3
			escreve_lcd (centena + 0x30);	// imprime mensagem no lcd 
			escreve_lcd (dezena + 0x30);
			escreve_lcd (unidade + 0x30);

			converte_hexadecimal(ADRESH);	//SEPARA RESULTADO EM 2 NIBBLES
			comando_lcd(0xC8);				// posiciona o cursor na linha 1, coluna 8
			if(hexa_high <= 9)	escreve_lcd (hexa_high + 0x30);	// imprime mensagem no lcd 
			else escreve_lcd(hexa_high + 0x37);

			if(hexa_low <= 9)	escreve_lcd (hexa_low + 0x30);	// imprime mensagem no lcd 
			else escreve_lcd(hexa_low + 0x37);

			while(BusyUSART());		//AGUARDA O BUFFER DE TRANSMISS�O FICAR VAZIO

			WriteUSART(converte);	//ENVIA CONVERTE � USART
	      	ConvertADC();			//Inicia convers�o AD
		}

		if(DataRdyUSART())			//SE DADO DISPON�VEL NO BUFFER DE RECEP��O
		{
			usart_rx = ReadUSART();	//SALVAR DADO EM USART_RX	
			comando_lcd(0xCF);		// posiciona o cursor na linha 1, coluna 15
			escreve_lcd(usart_rx);	//ENVIA USART_RX	
			ConvertADC();			//Inicia convers�o AD
		}
		
	}// FIM LA�O PRINCIPAL
}
