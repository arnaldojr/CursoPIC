/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        Programa��o em C18 - Exemplos usando o PIC18F4550 e McLab2       *
 *                               Exemplo 7                                 *
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
	ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DA LEITURA/ESCRITA
  	TANTO NA MEM�RIA DE DADOS QUANTO NA MEM�RIA DE PROGRAMA.
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

unsigned char	FILTRO;		//FILTRO BOT�O

char mem[8];

unsigned char 	tempo_turbo = 30;
unsigned char	cont_mem_dados;
unsigned short	cont_mem_prog;
unsigned char 	x;

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

#define	TIPO_MEM	FLAGSbits.BIT0	

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUN��ES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void comando_lcd(unsigned char caracter);
void escreve_lcd(unsigned char caracter);
void limpa_lcd(void);
void inicializa_lcd(void);
void escreve_frase(const rom char *frase);
void atualiza_linha_1(void);
void atualiza_linha_2(void);
void write_EEPROM(unsigned char endereco, unsigned char dado);
unsigned char read_EEPROM(unsigned char endereco);
void write_FLASH(unsigned addr, char *buffer);
unsigned short read_FLASH(unsigned endereco);

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
 *         		  FUN��O PARA ATUALIZAR A LINHA 1 DO LCD		           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void atualiza_linha_1(void)
{
	limpa_lcd();				// limpa lcd
	comando_lcd(0x80);			// posiciona o cursor na linha 0, coluna 0
								// imprime mensagem no lcd 
	escreve_frase("M.DADOS  M.PROG.");
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *         		  FUN��O PARA ATUALIZAR A LINHA 2 DO LCD		           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void atualiza_linha_2(void)
{
	union
	{
    	struct
		{
    		unsigned short low:8;
	    	unsigned short high:8;
    	} ;
    	struct
		{
			unsigned short all;	    
    	} ;    
	}dado;	

	unsigned char aux;
	
	comando_lcd(0xC1);				//POSICIONA CURSOR NA LINHA 1, COLUNA 1

	if (TIPO_MEM) escreve_lcd('>');	//SE TIPO DE MEM�RIA = 1, IMPRIME '>'
	else escreve_lcd(' ');			//SE TIPO DE MEM�RIA = 0, IMPRIME ' '

	aux = cont_mem_dados >> 4;		//CONVERTE cont_mem_dados EM HEXADECIMAL
	if(aux < 10) aux = aux + 0x30;
	else aux = aux + 0x37;
	escreve_lcd(aux);

	aux = cont_mem_dados & 0b00001111;
	if(aux < 10) aux = aux + 0x30;
	else aux = aux + 0x37;
	escreve_lcd(aux);

	if (TIPO_MEM) escreve_lcd('<');	//SE TIPO DE MEM�RIA = 1, IMPRIME '<'
	else escreve_lcd(' ');			//SE TIPO DE MEM�RIA = 0, IMPRIME ' '

	dado.all = cont_mem_prog;

	escreve_lcd(' ');
	escreve_lcd(' ');
	escreve_lcd(' ');
	
	if (!TIPO_MEM) escreve_lcd('>');	//SE TIPO DE MEM�RIA = 0, IMPRIME '>'
	else escreve_lcd(' ');				//SE TIPO DE MEM�RIA = 1, IMPRIME ' '

	aux = dado.high >> 4;				//CONVERTE cont_mem_prog EM HEXADECIMAL
	if(aux < 10) aux = aux + 0x30;
	else aux = aux + 0x37;
	escreve_lcd(aux);

	aux = dado.high & 0b00001111;
	if(aux < 10) aux = aux + 0x30;
	else aux = aux + 0x37;
	escreve_lcd(aux);

	aux = dado.low >> 4;
	if(aux < 10) aux = aux + 0x30;
	else aux = aux + 0x37;
	escreve_lcd(aux);

	aux = dado.low & 0b00001111;
	if(aux < 10) aux = aux + 0x30;
	else aux = aux + 0x37;
	escreve_lcd(aux);
	
	if (!TIPO_MEM) escreve_lcd('<');	//SE TIPO DE MEM�RIA = 0, IMPRIME '<'
	else escreve_lcd(' ');				//SE TIPO DE MEM�RIA = 1, IMPRIME ' '
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *             		  Fun��o de escrita da EEPROM interna				   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void write_EEPROM(unsigned char endereco, unsigned char dado)
{
	EEDATA = dado;				//carrega dado
	EEADR = endereco;			//carrega endere�o

	EECON1bits.CFGS = 0;		//habilita acesso a EEPROM
	EECON1bits.EEPGD = 0;		//aponta para mem�ria de dados
	EECON1bits.WREN = 1;		//habilita escrita
	INTCONbits.GIE = 0;			//desabilita todas as interrup��es
	EECON2 = 0x55;				//escreve 0x55 em EECON2 (obrigat�rio)
	EECON2 = 0xAA;				//escreve 0xAA em EECON2 (obrigat�rio)	
	EECON1bits.WR = 1;			//inicia a escrita
	INTCONbits.GIE = 1;			//habilita todas as interrup��es
	while(EECON1bits.WR);		//aguarda bit WR de EECON1 = 0
	EECON1bits.WREN = 0;		//desabilita escrita
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *             		  Fun��o de leitura da EEPROM interna				   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

unsigned char read_EEPROM(unsigned char endereco)
{
	EEADR = endereco;			//carrega endere�o

	EECON1bits.CFGS = 0;		//habilita acesso a EEPROM
	EECON1bits.EEPGD = 0;		//aponta para mem�ria de dados
	EECON1bits.RD = 1;			//habilita leitura

	return(EEDATA);				//retorna dado dispon�vel em EEDATA
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *             		  Fun��o de escrita da FLASH interna				   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// OBS: ESTA FUN��O, O ENDERE�O DEVE TER FINAL 0 OU 8
// EX.: 0x500, 0x508, 0x60, 0x68 ...

void write_FLASH(unsigned addr, char *buffer)
{
	unsigned char i;
	
	TBLPTR = addr;				//carrega endere�o
	
	EECON1bits.EEPGD = 1; 		//aponta para mem�ria FLASH
	EECON1bits.CFGS = 0; 		//habilita acesso a FLASH
	EECON1bits.WREN = 1; 		//habilita escrita
	EECON1bits.FREE = 1;		//apaga FLASH a partir do endere�o apontado no TBLPTR
	INTCONbits.GIE = 0; 		//desabilita todas as interrup��es
	EECON2 = 0x55;				//escreve 0x55 em EECON2 (obrigat�rio)
	EECON2 = 0xAA;				//escreve 0xAA em EECON2 (obrigat�rio)
	EECON1bits.WR = 1; 			//inicializa escrita da FLASH
	INTCONbits.GIE = 1; 		//habilita todas as interrup��es
	while(EECON1bits.WR);
	EECON1bits.WREN = 0; 		//desabilita escrita
	
	for(i = 0;i < 8;i++)
	{
		TABLAT = buffer[i];		//carrega dado em TABLAT

		_asm
		TBLWTPOSTINC			//escreve dado carregado em TABLAT e incrementa TBLPTR
		_endasm

		if(((i + 1) % 8) == 0)	//identifica os grupos de 8 bytes e habilita a escrita
		{ 
			TBLPTR -= 8;		//retorna 8 posi��es em TBLPTR

			EECON1bits.EEPGD = 1; 		//aponta para mem�ria FLASH
			EECON1bits.CFGS = 0; 		//habilita acesso a FLASH
			EECON1bits.WREN = 1; 		//habilita escrita
			EECON1bits.FREE = 0;		//somente escrita da FLASH
			INTCONbits.GIE = 0; 		//desabilita todas as interrup��es
			EECON2 = 0x55;				//escreve 0x55 em EECON2 (obrigat�rio)
			EECON2 = 0xAA;				//escreve 0xAA em EECON2 (obrigat�rio)
			EECON1bits.WR = 1; 			//inicializa escrita da FLASH
			INTCONbits.GIE = 1; 		//habilita todas as interrup��es
			while(EECON1bits.WR);			
			EECON1bits.WREN = 0; 		//desabilita escrita
			
			TBLPTR += 8;		//retorna � posi��o original em TBLPTR
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *             		  Fun��o de leitura da FLASH interna				   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

unsigned short read_FLASH(unsigned endereco)
{
	union
	{
    	struct
		{
    		unsigned short low:8;
	    	unsigned short high:8;
    	} ;
    	struct
		{
			unsigned short all;	    
    	} ;    
	}dado;				

	TBLPTR = endereco;		//carrega endere�o

	_asm					//l� posi��o apontada pelo TBLPTR, envia
	TBLRDPOSTINC			//dado para TABLAT e incremanta TBLPTR
	_endasm
	
	dado.low = TABLAT;		//salva dado em dado.low
	
	_asm					//l� posi��o apontada pelo TBLPTR, envia
	TBLRDPOSTINC			//dado para TABLAT e incremanta TBLPTR
	_endasm
	
	dado.high = TABLAT;		//salva dado em dado.high
	
	return(dado.all);		//retorna dado dispon�vel em dado.all
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
	TRISC = 0b11111111;		            //CONFIG DIRE��O DOS PINOS PORTC
	TRISD = 0b00000000;                 //CONFIG DIRE��O DOS PINOS PORTD
	TRISE = 0b00000100;                 //CONFIG DIRE��O DOS PINOS PORTE		

	ADCON1 = 0b00001111;                //DESLIGA CONVERSORES A/D

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicializa��o do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	while(RCONbits.NOT_TO);
	inicializa_lcd();			// configura o lcd

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	cont_mem_dados = read_EEPROM(0x10);	//L� MEM�RIA EEPROM
	cont_mem_prog = read_FLASH(0x800);	//L� MEM�RIA FLASH

	for(x=0;x<8;x++) mem[x]=0;			//INICIA BUFFER PARA GRAVA��O
										//DA MEM�RIA FLASH

	atualiza_linha_1();					//ATUALIZA LINHA 1
	atualiza_linha_2();					//ATUALIZA LINHA 2

	while(1)
	{
		ClrWdt();						//LIMPA O WDT

		if(!BT1)						//SE BOT�O 1 PRESSIONADO
		{	
			if (FILTRO)					//A��O DO BOT�O FOI EXECUTADA?
			{
				FILTRO--;				//DECREMENTA FILTRO				
				if(!FILTRO)				//TERMINOU FILTRO?	
				{
					if(TIPO_MEM) TIPO_MEM = 0;	//SE TIPO DE MEM = 1, TIPO DE MEM = 0
					else TIPO_MEM = 1;			//SE N�O, TIPO DE MEM = 1
					atualiza_linha_2();			//ATUALIZA LINHA 2 DO LCD
				}
			}
			continue;
		}		
		
		if(!BT2)						//SE BOT�O 2 PRESSIONADO
		{	
			FILTRO--;					//DECREMENTA FILTRO				
			if (!FILTRO)				//TERMINOU FILTRO?	
			{
				tempo_turbo--;
				if (!tempo_turbo)
				{
					if (TIPO_MEM) cont_mem_dados++;	//SE TIPO DE MEM = 1, INCREMENTA cont_mem_dados
					else cont_mem_prog++;			//SE N�O, INCREMENTA cont_mem_prog
					atualiza_linha_2();				//ATUALIZA LINHA 2 DO LCD
					tempo_turbo = 30;				//REINICIA TURBO DE TECLA
				}
			}
			continue;
		}		
		
		if(!BT3)						//SE BOT�O 3 PRESSIONADO
		{	
			FILTRO--;					//DECREMENTA FILTRO				
			if (!FILTRO)				//TERMINOU FILTRO?	
			{
				tempo_turbo--;
				if (!tempo_turbo)
				{
					if (TIPO_MEM) cont_mem_dados--;	//SE TIPO DE MEM = 1, DECREMENTA cont_mem_dados
					else cont_mem_prog--;			//SE N�O, DECREMENTA cont_mem_prog
					atualiza_linha_2();				//ATUALIZA LINHA 2 DO LCD
					tempo_turbo = 30;				//REINICIA TURBO DE TECLA
				}
			}		
			continue;
		}		


		if(!BT4)						//SE BOT�O 4 PRESSIONADO
		{	
			if (FILTRO)					//A��O DO BOT�O FOI EXECUTADA?
			{
				FILTRO--;				//DECREMENTA FILTRO							
				if(!FILTRO)				//TERMINOU FILTRO?	
				{
					write_EEPROM(0x10, cont_mem_dados);	//GRAVA NA EEPROM E FLASH
					mem[0] = cont_mem_prog;
					mem[1] = cont_mem_prog >> 8;
					write_FLASH(0x800, &mem);
				}
			}
			continue;
		}		

		FILTRO = 255;			//RECARREGA FILTRO DE BOT�ES

	}// FIM LA�O PRINCIPAL
}
