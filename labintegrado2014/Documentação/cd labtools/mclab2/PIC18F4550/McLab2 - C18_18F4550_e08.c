/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *        Programação em C18 - Exemplos usando o PIC18F4550 e McLab2       *
 *                               Exemplo 8                                 *
 *                                                                         *
 *                   LABTOOLS - MOSAICO DIDACTIC DIVISION                  *
 *                                                                         *
 *  TEL: (0XX11) 4992-8775               SITE: www.labtools.com.br         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   VERSÃO : 1.0                                                          *
 *   DATA : 11/03/2009 				                                  	   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             Descrição geral                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
	ESTE EXEMPLO FOI ELABORADO PARA EXPLICAR O FUNCIONAMENTO DA LEITURA/ESCRITA
	NA MEMÓRIA E2PROM SERIAL EXTERNA, UTILIZANDO O I2C POR SOFTWARE.
	
	OBS.: USAR A PLACA ADAPTADORA USB COM A MEMÓRIA EXTERNA
*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                				DEFINIÇÃO PIC			       			 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>       // Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <pwm.h>			//PWM library functions
#include <adc.h>           	//ADC library functions
#include <timers.h>        	//Timer library functions
#include <delays.h>        	//Delay library functions
//#include <i2c.h>           	//I2C library functios (software)
#include <stdlib.h>        	//Library functions
#include <usart.h>         	//USART library functions

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
 *              Definição e inicialização das variáveis Globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco estão definidas as variáveis globais do programa.
//Este programa não utiliza nenhuma variável de usuário

unsigned char	FILTRO1;		//FILTRO BOTÃO 1
unsigned char	FILTRO2;		//FILTRO BOTÃO 2
unsigned char	FILTRO3;		//FILTRO BOTÃO 3
unsigned char	FILTRO4;		//FILTRO BOTÃO 4

unsigned char	valor_dados = 0;
unsigned char 	tempo_turbo = 30;
char var;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declaração dos flags de software                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de flags ajuda na programação e economiza memória RAM.
//Este programa não utiliza nenhum flag de usuário

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

#define	ST_BT3		FLAGSbits.BIT0		//STATUS DO BOTÃO 3
#define	ST_BT4		FLAGSbits.BIT1		//STATUS DO BOTÃO 4

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
void I2C_EEPROM_WRITE(unsigned char endereco, unsigned char dado);
unsigned char I2C_EEPROM_READ(unsigned char endereco);
void atualiza_linha_1(void);
void atualiza_linha_2(unsigned char aux);

//------------------------------------------------------------------------
//                 Funções de I2C por software (PROTÓTIPO)
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
// As entradas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

#define BT1 PORTBbits.RB0		//BOTÃO 1
								//0 -> PRESSIONADO
								//1 -> LIBERADO

#define BT2 PORTBbits.RB1		//BOTÃO 2
								//0 -> PRESSIONADO
								//1 -> LIBERADO

#define BT3 PORTBbits.RB2		//BOTÃO 3
								//0 -> PRESSIONADO
								//1 -> LIBERADO

#define BT4 PORTBbits.RB3		//BOTÃO 4
								//0 -> PRESSIONADO
								//1 -> LIBERADO

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SAÍDAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SAÍDAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMAÇÃO E
//FUTURAS ALTERAÇÕES DO HARDWARE.

#define rs		PORTEbits.RE0	// via do lcd que sinaliza recepção de dados ou comando
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
	Delay10TCYx(4);				// espera mínimo 40 microsegundos
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
	Delay10TCYx(4);			// espera mínimo 40 microsegundos
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *						        Função para limpar o LCD		           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void limpa_lcd(void)
{
	comando_lcd(0x01);			// limpa lcd	
	Delay1KTCYx(2);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *		     			Inicialização do Display de LCD			           *
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
 *		        Função para escrever uma frase no LCD					   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_frase(const rom char *frase)
{
	do
	{
		escreve_lcd(*frase);
	}while(*++frase);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *         		  FUNÇÃO PARA ATUALIZAR A LINHA 1 DO LCD		           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void atualiza_linha_1(void)
{
	comando_lcd(0x80);	// posiciona o cursor na linha 0, coluna 0
	escreve_frase("MASTER I2C  (SW)");	//IMPRIME MENSAGEM	
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *         		  FUNÇÃO PARA ATUALIZAR A LINHA 2 DO LCD		           *
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
//                      Funções de I2C por software
//------------------------------------------------------------------------

/*************************************************************************\
 * T_mbit							 	                                 *
 * Rotina que gera o tempo de meio Tbit do barramento I2C                *
 * Parâmetros: void							 	                         *
 * Retorno:    void							 	                         *
\*************************************************************************/

void T_mbit(void)
{
	Nop();
}	

/*************************************************************************\
 * set_SDA							 	                                 *
 * Rotina que seta via de dados do barramento I2C - mantém pino em alta  *
 * impedância e quem garante nível é o pull-up                           *
 * Parâmetros: void							 	                         *
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
 * saída e realiza bit clear                                             *
 * Parâmetros: void							 	                         *
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
 * Rotina que seta clock do barramento I2C - mantém pino em alta impedân-*
 * cia e quem garante nível é o pull-up                                  *
 * Parâmetros: void							 	                         *
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
 * Rotina que zera clock do barramento I2C - configura pino como saída e *
 * realiza bit clear                                                     *
 * Parâmetros: void							 	                         *
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
 * Parâmetros: void							 	                         *
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
 * Parâmetros: void							                             *
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
 * Parâmetros: void							                             *
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
 * Parâmetros: void							                             *
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
 * Rotina para ler o ACK do periférico no barramento I2C                 *	
 * Parâmetros: void							                             *
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
 * Parâmetros: dado a ser escrito					 	                 *
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
 * Parâmetros: void							                             *
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
 *                Função para escrita na EEPROM externa I2C                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void I2C_EEPROM_WRITE(unsigned char endereco, unsigned char dado)
{
	StartI2C();				// INICIA COMUNICAÇÃO
	WriteI2C(0b10100000);	// controle de escrita
	IdleI2C();				// espera fim do evento I2C
	WriteI2C(endereco);		// ENVIO DO ENDEREÇO
	IdleI2C();				// espera fim do evento I2C
	WriteI2C(dado);			// controle de leitura
	IdleI2C();				// espera fim do evento I2C	
	StopI2C();				// fim de comunicação
	Delay100TCYx(100);		// AGUARDA 10ms
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                Função para leitura na EEPROM externa I2C                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

unsigned char I2C_EEPROM_READ(unsigned char endereco)
{
	unsigned char dado;
	StartI2C();				// INICIA A COMUNICAÇÃO
	WriteI2C(0b10100000);	// controle de escrita
	IdleI2C();				// espera fim do evento I2C
	WriteI2C(endereco);		// ENVIO DO ENDEREÇO
	IdleI2C();				// espera fim do evento I2C
	StartI2C();				// ENVIO DO RESTART

	WriteI2C(0b10100001);	// controle de leitura
	IdleI2C();				// espera fim do evento I2C
	dado = ReadI2C();
	NackI2C();
	StopI2C();				// fim de comunicação
	return(dado);//RETORNA DADO
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

	TRISA = 0b11111111;                 //CONFIG DIREÇÃO DOS PINOS PORTA
	TRISB = 0b00001111;                 //CONFIG DIREÇÃO DOS PINOS PORTB
	TRISC = 0b11111111;		            //CONFIG DIREÇÃO DOS PINOS PORTC
	TRISD = 0b00000000;                 //CONFIG DIREÇÃO DOS PINOS PORTD
	TRISE = 0b00000100;                 //CONFIG DIREÇÃO DOS PINOS PORTE		

	ADCON1 = 0b00001111;                //DESLIGA CONVERSORES A/D

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	while(RCONbits.NOT_TO);		// aguarda estouro do WDT
	inicializa_lcd();			// configura o lcd

	ST_BT3 = 0;
	ST_BT4 = 0;

	atualiza_linha_1();			// ATUALIZA LINHA 1 DO LCD

	valor_dados = I2C_EEPROM_READ(0x10);//LÊ O ENDEREÇO 0x10 DA MEMÓRIA
	atualiza_linha_2(valor_dados);		//ATUALIZA LINHA 2 DO LCD
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Rotina Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	while(1)
	{
		ClrWdt();

		if(!BT1)					//BOTÃO 1 PRESSIONADO?
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

		if(!BT2)					//BOTÃO 2 PRESSIONADO?
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
		
		if(!BT3)					//BOTÃO 3 PRESSIONADO?
		{	
			FILTRO3--;				//DECREMENTA FILTRO3
			if(!FILTRO3)			//TERMINOU FILTRO3?
			{
				if (!ST_BT3)		//AÇÃO DO BOTÃO JÁ FOI EXECUTADA?
				{
					I2C_EEPROM_WRITE(0x10,valor_dados);//GRAVA DADO EM EEPROM
					atualiza_linha_2(valor_dados);	//ATUALIZA LINHA2
					ST_BT3 = 1;		//MARCA AÇÃO DO BOTÃO 3
				}
			}		
			continue;
		}		
		else ST_BT3 = 0; // BOTÃO LIBERADO, LIMPA O FLAG
		
		if(!BT4)					//BOTÃO 4 PRESSIONADO?
		{	
			FILTRO4--;				//DECREMENTA FILTRO4
			if(!FILTRO4)			//TERMINOU FILTRO4?
			{
				if (!ST_BT4)		//AÇÃO DO BOTÃO JÁ FOI EXECUTADA?
				{
					valor_dados = I2C_EEPROM_READ(0x10);// LÊ DADO EM EEPROM
					atualiza_linha_2(valor_dados);	//ATUALIZA LINHA2
					ST_BT4 = 1;		//MARCA AÇÃO DO BOTÃO 4
				}	
			}		
			continue;
		}		
		else ST_BT4 = 0; // BOTÃO LIBERADO, LIMPA O FLAG

		FILTRO1 = 250;			//BT1
		FILTRO2 = 250;			//BT2
		FILTRO3 = 250;			//BT3
		FILTRO4 = 250;			//BT4
	}// FIM LAÇO PRINCIPAL
}
