/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        Programa��o em C18 - Exemplos usando o PIC18F4550 e McLab2       *
 *                               Exemplo 8                                 *
 *                                                                         *
 *                   LABTOOLS - MOSAICO DIDACTIC DIVISION                  *
 *                                                                         *
 *  TEL: (0XX11) 4992-8775               SITE: www.labtools.com.br         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   VERS�O : 1.0                                                          *
 *   DATA : 11/03/2009 				                                  	   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             Descri��o geral                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
	ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DA LEITURA/ESCRITA
	NA MEM�RIA E2PROM SERIAL EXTERNA, UTILIZANDO O I2C POR SOFTWARE.
	
	OBS.: USAR A PLACA ADAPTADORA USB COM A MEM�RIA EXTERNA
*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                				DEFINI��O PIC			       			 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>       // Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUN��ES DE PERIF�RICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <pwm.h>			//PWM library functions
#include <adc.h>           	//ADC library functions
#include <timers.h>        	//Timer library functions
#include <delays.h>        	//Delay library functions
//#include <i2c.h>           	//I2C library functios (software)
#include <stdlib.h>        	//Library functions
#include <usart.h>         	//USART library functions

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

unsigned char	FILTRO1;		//FILTRO BOT�O 1
unsigned char	FILTRO2;		//FILTRO BOT�O 2
unsigned char	FILTRO3;		//FILTRO BOT�O 3
unsigned char	FILTRO4;		//FILTRO BOT�O 4

unsigned char	valor_dados = 0;
unsigned char 	tempo_turbo = 30;
char var;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de constantes facilita a programa��o e a manuten��o.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declara��o dos flags de software                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A defini��o de flags ajuda na programa��o e economiza mem�ria RAM.
//Este programa n�o utiliza nenhum flag de usu�rio

struct {
    unsigned BIT0:1;
    unsigned BIT1:1;
    unsigned BIT2:1;
    unsigned BIT3:1;
    unsigned BIT4:1;
    unsigned BIT5:1;
    unsigned BIT6:1;
    unsigned BIT7:1;
}FLAGSbits;			//ARMAZENA OS FLAGS DE CONTROLE

#define	ST_BT3		FLAGSbits.BIT0		//STATUS DO BOT�O 3
#define	ST_BT4		FLAGSbits.BIT1		//STATUS DO BOT�O 4

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
void I2C_EEPROM_WRITE(unsigned char endereco, unsigned char dado);
unsigned char I2C_EEPROM_READ(unsigned char endereco);
void atualiza_linha_1(void);
void atualiza_linha_2(unsigned char aux);

//------------------------------------------------------------------------
//                 Fun��es de I2C por software (PROT�TIPO)
//------------------------------------------------------------------------

#define		ReadACK(void)	IdleI2C(void)//MACRO PARA A LEITURA DO ACK

void T_mbit(void);
void set_SDA(void);
void clr_SDA(void);
void set_SCL(void);
void clr_SCL(void);
void StartI2C(void);
void StopI2C(void);
void AckI2C(void);
void NackI2C(void);
unsigned char IdleI2C(void);
void WriteI2C(unsigned char data_out);
unsigned char ReadI2C(void);

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

#define	SDA			PORTCbits.RC2		// VIA SDA DO I2C POR SOFTWARE
#define	SDA_TRIS	TRISCbits.TRISC2

#define	SCL			PORTAbits.RA2		// VIA SCK DO I2C POR SOFTWARE
#define	SCL_TRIS	TRISAbits.TRISA2

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
	comando_lcd(0x80);	// posiciona o cursor na linha 0, coluna 0
	escreve_frase("MASTER I2C  (SW)");	//IMPRIME MENSAGEM	
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *         		  FUN��O PARA ATUALIZAR A LINHA 2 DO LCD		           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void atualiza_linha_2(unsigned char aux)
{
	unsigned char hexa_high;
	unsigned char hexa_low;

	hexa_low = aux & 0b00001111;
	hexa_high = (aux >> 4) & 0b00001111;

	comando_lcd(0xC6);	// posiciona o cursor na linha 1, coluna 6
	if(hexa_high <= 9)	escreve_lcd (hexa_high + 0x30);	// imprime mensagem no lcd 
	else escreve_lcd(hexa_high + 0x37);

	if(hexa_low <= 9)	escreve_lcd (hexa_low + 0x30);	// imprime mensagem no lcd 
	else escreve_lcd(hexa_low + 0x37);

	escreve_lcd ('h');	// imprime mensagem no lcd 
}

//------------------------------------------------------------------------
//                      Fun��es de I2C por software
//------------------------------------------------------------------------

/*************************************************************************\
 * T_mbit							 	                                 *
 * Rotina que gera o tempo de meio Tbit do barramento I2C                *
 * Par�metros: void							 	                         *
 * Retorno:    void							 	                         *
\*************************************************************************/

void T_mbit(void)
{
	Nop();
}	

/*************************************************************************\
 * set_SDA							 	                                 *
 * Rotina que seta via de dados do barramento I2C - mant�m pino em alta  *
 * imped�ncia e quem garante n�vel � o pull-up                           *
 * Par�metros: void							 	                         *
 * Retorno:    void							 	                         *
\*************************************************************************/

void set_SDA(void)
{
    SDA_TRIS = 1;
	Nop();
    Nop();
    Nop();
    Nop();
}

/*************************************************************************\
 * clr_SDA							 	                                 *
 * Rotina que zera via de dados do barramento I2C - configura pino como  *
 * sa�da e realiza bit clear                                             *
 * Par�metros: void							 	                         *
 * Retorno:    void							 	                         *
\*************************************************************************/

void clr_SDA(void)
{
    SDA_TRIS = 0;
    Nop();
    Nop();
    Nop();
    Nop();
    SDA = 0;
}

/*************************************************************************\
 * set_SCL							 	                                 *
 * Rotina que seta clock do barramento I2C - mant�m pino em alta imped�n-*
 * cia e quem garante n�vel � o pull-up                                  *
 * Par�metros: void							 	                         *
 * Retorno:    void							 	                         *
\*************************************************************************/

void set_SCL( void )
{
    SCL_TRIS = 1;
    Nop();
    Nop();
    Nop();
    Nop();
}

/*************************************************************************\
 * clr_SCL							 	                                 *
 * Rotina que zera clock do barramento I2C - configura pino como sa�da e *
 * realiza bit clear                                                     *
 * Par�metros: void							 	                         *
 * Retorno:    void							 	                         *
\*************************************************************************/

void clr_SCL(void)
{
    SCL_TRIS = 0;
    Nop();
    Nop();
    Nop();
    Nop();
    SCL = 0;
}

/*************************************************************************\
 * StartI2C							 	                                 *
 * Rotina de start I2C 						                             *
 * Par�metros: void							 	                         *
 * Retorno:    void							 	                         *
\*************************************************************************/

void StartI2C(void)
{
	clr_SCL();
	set_SDA();
	T_mbit();
	set_SCL();
	T_mbit();
	clr_SDA();
	T_mbit();
	clr_SCL();
}

/*************************************************************************\
 * StopI2C							 	                                 *
 * Rotina para para parar a I2C					                         *
 * Par�metros: void							                             *
 * Retorno:    void							                             *
\*************************************************************************/

void StopI2C(void)
{
	clr_SCL();
	T_mbit();	
	clr_SDA();
	T_mbit();	
	set_SCL();
	T_mbit();	
	set_SDA();
}

/*************************************************************************\
 * AckI2C      					 	                                 *
 * Rotina para enviar ACK via I2C                                       *	
 * Par�metros: void							                             *
 * Retorno:    void							                             *
\*************************************************************************/

void AckI2C(void)
{
	clr_SDA();
	T_mbit();	
	set_SCL();
	T_mbit();	
	clr_SCL();
	T_mbit();	
	set_SDA();
}

/*************************************************************************\
 * NackI2C      					 	                                 *
 * Rotina para enviar NACK via I2C                                       *	
 * Par�metros: void							                             *
 * Retorno:    void							                             *
\*************************************************************************/

void NackI2C(void)
{
	set_SDA();
	T_mbit();	
	set_SCL();
	T_mbit();	
	clr_SCL();
}

/*************************************************************************\
 * IdleI2C    					 	                                     *
 * Rotina para ler o ACK do perif�rico no barramento I2C                 *	
 * Par�metros: void							                             *
 * Retorno:    unsigned char							                 *
\*************************************************************************/

unsigned char IdleI2C(void)
{
    unsigned char ret;
    
    set_SDA();
    set_SCL();
    T_mbit();
    ret = SDA;
    T_mbit();
    clr_SCL();
    return(ret);
}

/*************************************************************************\
 * WriteI2C							 	 	                             *
 * Rotina de escrita na I2C					 	                         *
 * Par�metros: dado a ser escrito					 	                 *
 * Retorno:    void							 	                         *
\*************************************************************************/

void WriteI2C(unsigned char data_out)
{
    unsigned char b;
    unsigned char bit_x;
      
    bit_x = 0x80;
    
    for(b = 0 ; b < 8 ; b++)
    {
        if (data_out & bit_x)
            set_SDA();
        else
            clr_SDA();
        bit_x >>= 1;
        
        T_mbit();
        set_SCL();
        T_mbit();
        clr_SCL();
    }
    
    set_SDA();
}

/*************************************************************************\
 * ReadI2C  							 	                             *
 * Rotina de leitura de um byte via I2C			                         *
 * Par�metros: void							                             *
 * Retorno:    byte lido no barramento                                   *
\*************************************************************************/

unsigned char ReadI2C(void)
{
    unsigned char b;
    unsigned char data_in;
    
    data_in = 0x00;
    
    set_SDA();
    for(b = 0 ; b < 8 ; b++)
    {
        set_SCL();
        T_mbit();
        data_in |= ((unsigned char)SDA << (7 - b)) ;
        T_mbit();
        clr_SCL();
        T_mbit();
    }
    
    return (data_in);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                Fun��o para escrita na EEPROM externa I2C                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void I2C_EEPROM_WRITE(unsigned char endereco, unsigned char dado)
{
	StartI2C();				// INICIA COMUNICA��O
	WriteI2C(0b10100000);	// controle de escrita
	IdleI2C();				// espera fim do evento I2C
	WriteI2C(endereco);		// ENVIO DO ENDERE�O
	IdleI2C();				// espera fim do evento I2C
	WriteI2C(dado);			// controle de leitura
	IdleI2C();				// espera fim do evento I2C	
	StopI2C();				// fim de comunica��o
	Delay100TCYx(100);		// AGUARDA 10ms
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                Fun��o para leitura na EEPROM externa I2C                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

unsigned char I2C_EEPROM_READ(unsigned char endereco)
{
	unsigned char dado;
	StartI2C();				// INICIA A COMUNICA��O
	WriteI2C(0b10100000);	// controle de escrita
	IdleI2C();				// espera fim do evento I2C
	WriteI2C(endereco);		// ENVIO DO ENDERE�O
	IdleI2C();				// espera fim do evento I2C
	StartI2C();				// ENVIO DO RESTART

	WriteI2C(0b10100001);	// controle de leitura
	IdleI2C();				// espera fim do evento I2C
	dado = ReadI2C();
	NackI2C();
	StopI2C();				// fim de comunica��o
	return(dado);//RETORNA DADO
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
	
	while(RCONbits.NOT_TO);		// aguarda estouro do WDT
	inicializa_lcd();			// configura o lcd

	ST_BT3 = 0;
	ST_BT4 = 0;

	atualiza_linha_1();			// ATUALIZA LINHA 1 DO LCD

	valor_dados = I2C_EEPROM_READ(0x10);//L� O ENDERE�O 0x10 DA MEM�RIA
	atualiza_linha_2(valor_dados);		//ATUALIZA LINHA 2 DO LCD
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	while(1)
	{
		ClrWdt();

		if(!BT1)					//BOT�O 1 PRESSIONADO?
		{	
			FILTRO1--;				//DECREMENTA FILTRO1
			if(!FILTRO1)			//TERMINOU FILTRO1?
			{
				tempo_turbo--;		//DECREMETA TEMPO_TURBO
				if(!tempo_turbo)	//TERMINOU_TURBO?
				{
					valor_dados++;	//INCREMENTA VALOR_DADOS
					tempo_turbo = 30;//REINICIA TURBO_TECLA
					atualiza_linha_2(valor_dados);//ATUALIZA LINHA2
				}
			}		
			continue;
		}		

		if(!BT2)					//BOT�O 2 PRESSIONADO?
		{	
			FILTRO2--;				//DECREMENTA FILTRO2
			if(!FILTRO2)			//TERMINOU FILTRO2?
			{
				tempo_turbo--;		//DECREMETA TEMPO_TURBO
				if(!tempo_turbo)	//TERMINOU_TURBO?
				{
					valor_dados--;	//DECREMENTA VALOR_DADOS
					tempo_turbo = 30;//REINICIA TURBO_TECLA
					atualiza_linha_2(valor_dados);//ATUALIZA LINHA2
				}
			}		
			continue;
		}		
		
		if(!BT3)					//BOT�O 3 PRESSIONADO?
		{	
			FILTRO3--;				//DECREMENTA FILTRO3
			if(!FILTRO3)			//TERMINOU FILTRO3?
			{
				if (!ST_BT3)		//A��O DO BOT�O J� FOI EXECUTADA?
				{
					I2C_EEPROM_WRITE(0x10,valor_dados);//GRAVA DADO EM EEPROM
					atualiza_linha_2(valor_dados);	//ATUALIZA LINHA2
					ST_BT3 = 1;		//MARCA A��O DO BOT�O 3
				}
			}		
			continue;
		}		
		else ST_BT3 = 0; // BOT�O LIBERADO, LIMPA O FLAG
		
		if(!BT4)					//BOT�O 4 PRESSIONADO?
		{	
			FILTRO4--;				//DECREMENTA FILTRO4
			if(!FILTRO4)			//TERMINOU FILTRO4?
			{
				if (!ST_BT4)		//A��O DO BOT�O J� FOI EXECUTADA?
				{
					valor_dados = I2C_EEPROM_READ(0x10);// L� DADO EM EEPROM
					atualiza_linha_2(valor_dados);	//ATUALIZA LINHA2
					ST_BT4 = 1;		//MARCA A��O DO BOT�O 4
				}	
			}		
			continue;
		}		
		else ST_BT4 = 0; // BOT�O LIBERADO, LIMPA O FLAG

		FILTRO1 = 250;			//BT1
		FILTRO2 = 250;			//BT2
		FILTRO3 = 250;			//BT3
		FILTRO4 = 250;			//BT4
	}// FIM LA�O PRINCIPAL
}
