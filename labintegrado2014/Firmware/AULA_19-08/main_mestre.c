// Firmware desenvolvido para o hardware da placa McLabIII - Implementação de comunicação rs485
// Este sofware implementa a rotina de leitura de um modulo escravo.
#include<p18f4550.h>
#include<delays.h>
#include<usart.h>
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Configurações para gravação                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma config PLLDIV = 1, WDT = OFF, LVP = OFF, FOSC = HSPLL_HS, PWRT = ON, BOR = ON		//configuração de fusivel para pic18f4550


//Protótipos de funções
void envia_rs485(void);
void teclado(void);
void escreve_comando (unsigned char com);
void escreve_dado (unsigned char dado);
void inicia_lcd (void);

//Definição de variáveis globais
char decoder (char u);
unsigned char estado = 0;
unsigned char byte = 0;
unsigned char dado_escravo, PORT_aux, aux_decoder;
unsigned char numero_escravo = 1; 	//seleciona o kit escravo
unsigned int outval;
	
#define	controle_leitura	PORTEbits.RE2 = 0		
#define	controle_escrita	PORTEbits.RE2 = 1
#define	select_escravo_ON	TXSTAbits.TX9D = 1
#define	select_escravo_OFF	TXSTAbits.TX9D = 0



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
				USART_NINE_BIT &		// Comunicação de 9bits
				USART_CONT_RX &			// Recepção contínua
				USART_BRGH_HIGH, 25 	// Alta velocidade 9600bps
			);							//fim da configuração da USART
			
	byte = 0 ;	
	controle_escrita;
	
	inicia_lcd();
	escreve_comando(0x80);
	escreve_dado('e');
	escreve_dado('s');
	escreve_dado('c');	
	escreve_comando(0xC0);
	escreve_dado('b');
	escreve_dado('y');
	escreve_dado('t');
	escreve_dado('e');	
	while(1)	//Loop principal
	{
	teclado();
	
	}
}

//----------------------------------------------------------------
//Função para enviar dados para o escravo
	void envia_rs485()
	{

USART_Status.TX_NINE = (numero_escravo & 0x0100)>> 8;

WriteUSART( numero_escravo );

		//select_escravo_ON;
	
		while(BusyUSART());
		
		select_escravo_OFF;
		dado_escravo = byte;
		WriteUSART(dado_escravo);
		while(BusyUSART());
		dado_escravo = 0;
	}
		
//----------------------------------------------------------------
//Envia comando para o LCD
	void escreve_comando(unsigned char com) //Envia comando ao LCD
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
	void escreve_dado(unsigned char dado) //Envia dado ao LCD
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
	void inicia_lcd (void)
	{
		escreve_comando(0x38);
		Delay10KTCYx(6);
		escreve_comando(0x38);
		escreve_comando(0x0C);
		escreve_comando(0x06);
		escreve_comando(0x01);
		Delay10KTCYx(2);
		escreve_comando(0x80);	//Posiciona cursor
	}	
//----------------------------------------------------------------
//Função para decodificar valor para o LCD
char decoder (char u)
{
	switch(u)
 	{
	 	case 0: return '0'; 
	  	case 1: return '1'; 
	  	case 2: return '2'; 
	  	case 3: return '3'; 
	  	case 4: return '4'; 
	  	case 5: return '5'; 
	  	case 6: return '6'; 
	  	case 7: return '7'; 
	  	case 8: return '8'; 
	  	case 9: return '9'; 
	  	default: return 'E'; 
	 }
}
	
//----------------------------------------------------------------
//Função para ler teclado  *********precisa trabalhar nela ainda****	
	void teclado(void)
	{
	//Multipliexação do Teclado
	
		if (PORTDbits.RD0==1)			PORTD=0b00000010; //Testa o Bit 0 da Porta D
		
		else if (PORTDbits.RD1==1)		PORTD=0b00000100;  //Testa o Bit 1 da Porta D
		
		else if (PORTDbits.RD2==1)		PORTD=0b00001000; //Testa o Bit 2 da Porta D
		
		else if (PORTDbits.RD3==1)		PORTD=0b00000001;//Testa o Bit 3 da Porta D
		
		Delay10KTCYx(1);    //Tempo para o sinal de Multiplexação (10000 Cliclos de Maquina)
	
	//Teste do Teclado -------------------------------------------------------------------------
	//Primeira Linha
		if ((PORTDbits.RD0==1) & (PORTDbits.RD4==1))    	//Tecla S1
		{
			Delay10KTCYx(10);
			numero_escravo = 1;
			byte++;
			PORTB=byte;
			aux_decoder = decoder(numero_escravo);
			escreve_comando(0x83);
			escreve_dado(aux_decoder);
			aux_decoder = decoder(byte);
			escreve_comando(0xC5);
			escreve_dado(aux_decoder);
			
			envia_rs485();
		}
		else if ((PORTDbits.RD1==1) & (PORTDbits.RD4==1))  //Tecla S2
		{
			Delay10KTCYx(10);
			numero_escravo = 1;
			byte--;
			PORTB=byte;
			aux_decoder = decoder(numero_escravo);
			escreve_comando(0x83);
			escreve_dado(aux_decoder);
			aux_decoder = decoder(byte);
			escreve_comando(0xC5);
			escreve_dado(aux_decoder);
			envia_rs485();
		}
		else if ((PORTDbits.RD2==1) & (PORTDbits.RD4==1))  //Tecla S3
		{
			Delay10KTCYx(10);
			numero_escravo = 2;
			byte++;
			PORTB=byte;
			aux_decoder = decoder(numero_escravo);
			escreve_comando(0x83);
			escreve_dado(aux_decoder);
			aux_decoder = decoder(byte);
			escreve_comando(0xC5);
			escreve_dado(aux_decoder);
			envia_rs485();
		}
		else if ((PORTDbits.RD3==1) & (PORTDbits.RD4==1))  //Tecla S4
		{
			Delay10KTCYx(10);	
			numero_escravo = 2;
			byte --;
			PORTB=byte;
			aux_decoder = decoder(numero_escravo);
			escreve_comando(0x83);
			escreve_dado(aux_decoder);
			aux_decoder = decoder(byte);
			escreve_comando(0xC5);
			escreve_dado(aux_decoder);
			envia_rs485();
		}
	
	}