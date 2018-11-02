// Firmware desenvolvido para o hardware da placa McLabIII - Implementação de comunicação rs485
// Este sofware implementa a rotina de leitura de um modulo escravo.
#include<p18f4550.h>
#include<delays.h>
#include<usart.h>

#pragma config PLLDIV = 1, WDT = OFF, LVP = OFF, FOSC = HSPLL_HS, PWRT = ON, BOR = ON		//configuração de fusivel para pic18f4550


//Definição de variáveis globais
unsigned char estado = 0;
unsigned char byte = 0;
unsigned char escravo = 1; 	//seleciona o kit escravo

#define	controle_leitura PORTEbits.RE2 = 0		
#define	controle_escrita PORTEbits.RE2 = 1
//----------------------------------------------------------------
void main(void)
{
	//----------------------------------------------------------------
	//Configurações das Portas
	TRISB=0X00;   			//LEDs
	TRISC=0b11111001;
	TRISD=0xF0;   					//Teclado Matricial
	TRISE=0X00;
	
	ADCON1=15; 						//15 é o codigo para configuração das portas analogivas (datacheet PIC18F45
	
	PORTE=0X00;
	PORTC=0X00;
	PORTB=0X00;
	PORTD=0b00000001; 				//Iniciando Portas

	OpenUSART(	USART_TX_INT_OFF &		// Int. de transmissão habilitada
				USART_RX_INT_OFF &		// Int. de recepção DESLIGADA
				USART_ASYNCH_MODE &		// Comunicação assíncrona
				USART_EIGHT_BIT &		
				USART_CONT_RX &			// Recepção contínua
				USART_BRGH_HIGH, 25 );	// Alta velocidade 9600bps

				
	controle_leitura;
	while(1)	//Loop principal
	{
		if(DataRdyUSART() == escravo)
		{
			if(RCSTAbits.RX9D == 1)		//se for dado
			{
				byte = ReadUSART();		//le o end. recebido
				if(byte=0b00000001)		//se for end. 01h
				{
				estado=1;
				}
			}
			else			//se for dado
			{
				byte = ReadUSART();
				if(estado=1)
				{
					PORTB=byte;
					estado=0;
				}
			}
		}	
	}
}
