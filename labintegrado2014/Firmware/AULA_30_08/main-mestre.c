/*
ESTE PROGRAMA IMPLEMENTA NO KIT MCLAB3 COMUNICAÇAO SERIAL,
INTRERRUPÇAO DA USART COMO ALTA PRIORIDADE PARA RECEBER OS BYTES.
INTERRUPÇAO DO CONVERSOR AD COMO BAIXA PRIORIDADDE
MODIFICAR O PROTOCOLO DE COMUNICAÇAO.

MESTRE CONTROLA O PWM DO ESCRAVO 8BITS

1oBYTE		2oBYTE		3oBYTE		4oBYTE
END. ESCRA.	COMANDO		PWM(HIGH)	PWM(LOW)

MESTRE LE A ENTRADA ANALOGICA DO ESCRAVO.

1oBYTE		2oBYTE		3oBYTE		4oBYTE
END. ESCRA.	COMANDO		AD(HIGH)	AD(LOW)



COMANDO
0Ah -->CONTROLA PWM
05h -->LE AD
		
*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              DEFINIÇAO PIC                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>           //Register definitions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇOES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  
#include <timers.h>             //Timer library functions
#include <delays.h>             //Delay library functions
#include <usart.h>              //USART library functions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Configuraçoes para gravaçao                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma config PLLDIV = 1, WDT = OFF, LVP = OFF, FOSC = HSPLL_HS, PWRT = ON, BOR = ON		//configuraçao de fusivel para pic18f4550

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Definiçao e inicializaçao das variáveis globais          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco estao definidas as variáveis globais do programa.

unsigned char contador_varredura;           // Variável mutiplicadora do tempo
                                            // de varredura

unsigned char ad_high;                     // Variável para armazenamento
                                            // dos valor do ad
											
unsigned char rx_dt, new_dt	;						// Variável para armazenamento
                                            // dos valor da USART					

unsigned char hexa_high, hexa_low, PORT_aux, dado, com;

unsigned char W_temp, BSR_temp, STATUS_temp;
 
 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUNÇOES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void comando_lcd(unsigned char caracter);
void escreve_lcd(unsigned char caracter);
void limpa_lcd(void);
void inicializa_lcd(void);
void escreve_frase(const rom char *frase);
void tela_principal(void);

//Rotina de interrupção	
void ISR_High_Priority(void);					//Prototipagem da função de tratamento de interrupções de alta prioridade
void ISR_Low_Priority(void);					//Prototipagem da função de tratamento de interrupções de baixa prioridade

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       VETOR DE INTERRUPÇÃO   	                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 #pragma code vec_int_high_priority = 0x08
 void vec_int_high_priority(void)
 { _asm GOTO ISR_High_Priority _endasm }

 #pragma code vec_int_low_priority = 0x18
 void vec_int_low_priority(void)
 { _asm GOTO ISR_Low_Priority _endasm }

 #pragma code
 void ISR_High_Priority(void)
 {
	 if(PIR1bits.RCIF)
	{
		rx_dt = ReadUSART();
		new_dt = 1;
 		PIR1bits.RCIF = 0;
	}
 _asm
 RETFIE 1
 _endasm
 }

 //----------------------------------------------------------------
 void ISR_Low_Priority(void)
 {
 _asm
 MOVFF WREG,W_temp
 MOVFF STATUS, STATUS_temp
 MOVFF BSR, BSR_temp
 _endasm

 //;

 _asm
 MOVFF W_temp, WREG
 MOVFF STATUS_temp, STATUS
 MOVFF BSR_temp, BSR
 RETFIE 0
 _endasm
}
	
 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SAÍDAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//As saídas devem ser associadas a nomes para facilitar a programaçao e
//futuras alteraçoes do hardware.

#define     COLUNA_1    PORTDbits.RD0   //PINO PARA ATIVAR COLUNA 1 DO TECLADO
                                        //MATRICIAL
                                        //1 -> COLUNA ATIVADA
                                        //0 -> COLUNA DESATIVADA

#define     rs          PORTEbits.RE0   // via do lcd que sinaliza recepçao de dados ou comando
#define     rw          PORTBbits.RB7   // via do lcd que configura leitura ou escrita no barramento
#define     enable      PORTEbits.RE1   // enable do lcd


//----------------------------------------------------------------
//Envia comando para o LCD
	void comando_lcd(unsigned char com) //Envia comando ao LCD
	{
		PORTEbits.RE0=0;	  	//Sinal de comando
		PORT_aux=PORTD; 	  	//Salvando o valor da Porta D
		TRISD=0X00;    			//Mudando a Porta D para saida
		Delay10TCYx(1); 	 	//Tempo
		PORTD=com;    			//Comando para Porta D
		PORTEbits.RE1=1;	  	//Pulso enable
		Delay10TCYx(3); 	 	//Tempo
		PORTEbits.RE1=0;	  	//Desabilita pulso de enable
		TRISD=0xF0;    			//Muda a porta para o teclado
		Delay10TCYx(1); 	  	//Tempo
		PORTD=PORT_aux; 	  	//Restaura o valor da Porta D
		Delay10KTCYx(2);	  	//Tempo
	}
//----------------------------------------------------------------
//Escrita de uma dado no LCD
	void escreve_lcd(unsigned char dado) //Envia dado ao LCD
	{
		PORTEbits.RE0=1;  		//Sinal
		PORT_aux=PORTD;   		//Salvando o valor da Porta D
		TRISD=0X00;    			//Mudando a Porta D para saida
		Delay10TCYx(1);  		//Tempo
		PORTD=dado;    			//Comando para Porta D
		PORTEbits.RE1=1;  		//Pulso enable
		Delay10TCYx(3);  		//Tempo
		PORTEbits.RE1=0;  
		TRISD=0xF0;  		  	//Muda a porta para o teclado
		Delay10TCYx(1);		  	//Tempo
		PORTD=PORT_aux;		  	//Restaura o valor da Porta D
		Delay10KTCYx(2);	  	//Tempo
	}
//----------------------------------------------------------------
//Função de inicialização do LCD
	void inicializa_lcd (void)
	{
		comando_lcd(0x38);
		Delay10KTCYx(6);
		comando_lcd(0x38);
		comando_lcd(0x0C);
		comando_lcd(0x06);
		comando_lcd(0x01);
		Delay10KTCYx(2);
		comando_lcd(0x80);	//Posiciona cursor
	}	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              Funçao para limpar o LCD                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void limpa_lcd(void)
{
    comando_lcd(0x01);  // limpa lcd
    Delay1KTCYx(2);
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *             Funçao para escrever uma frase no LCD                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void escreve_frase(const rom char *frase)
{
    do
    {
        escreve_lcd(*frase);
    }while(*++frase);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Tela Principal                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tela_principal(void)
{
    comando_lcd(0x80);            // posiciona o cursor na linha 0, coluna 0
    escreve_frase("   MESTRE   ");
    comando_lcd(0xC0);            // posiciona o cursor na linha 1, coluna 0
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                            Funçao Principal                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void main(void)
{
	TRISB=0X00;   			//LEDs
	TRISC=0b11111001;
	TRISD=0xF0;   					//Teclado Matricial
	TRISE=0X00;
	
	ADCON1=15; 						//15 é o codigo para configuração das portas analogivas (datacheet PIC18F45
	
	PORTE=0X00;
	PORTC=0X00;
	PORTB=0X00;
	PORTD=0b00000001; 				//Iniciando Portas	   

   
    //CONFIGURAÇAO DA USART
    OpenUSART(USART_TX_INT_OFF &
			USART_RX_INT_OFF & 
			USART_ASYNCH_MODE &
			USART_EIGHT_BIT  & 
			USART_CONT_RX    & 
			USART_BRGH_HIGH,25);
	IPR1bits.RCIP = 1; 					//Int. USART (RX) é de alta prioridade
	PIR1bits.RCIF = 0; 					//Inicializa flag de interrupções
    INTCONbits.GIE = 1; 				//Habilita interrupções
	INTCONbits.PEIE = 1;
	
	inicializa_lcd();
	tela_principal();
	
	
	ad_high=0;
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              Rotina Principal                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    while(1)
	{
		if(new_dt)
		{
			new_dt = 0;
			if(ad_high != 0 )
				{
					comando_lcd(0xc0);
					escreve_lcd(rx_dt);
					ad_high = 0;
					comando_lcd(0x80);            // posiciona o cursor na linha 0, coluna 0
					escreve_frase("alta");
					
				}
				else 
				{	
					comando_lcd(0xc0);				
					escreve_lcd(rx_dt);
					ad_high = 1;
					comando_lcd(0x85);            // posiciona o cursor na linha 0, coluna 0
					escreve_frase("baixa");
				}
		}
	}
}   