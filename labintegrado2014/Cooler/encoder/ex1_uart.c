// Firmware desenvolvido para o hardware da placa McLabII - comunica��o serial
#include<p18f452.h>
#include<delays.h>
#include<usart.h>

#pragma config WDT = OFF, LVP = OFF, OSC = XT, PWRT = ON, BOR = ON, BORV = 42

//Prot�tipos de fun��es
void ISR_High_Priority(void);			
void inicia_lcd(void);			//Fun��o de inicializa��o do LCD
void escreve_comando(char c);	//Envia comando para o LCD
void escreve_dado(char d);		//Escrita de uma dado no LCD

//Defini��o de vari�veis globais
volatile unsigned char new_dt, rx_dt = 0;

//----------------------------------------------------------------
#pragma code vec_int_high_priority = 0x08
void vec_int_high_priority(void)
{ _asm GOTO ISR_High_Priority _endasm }

#pragma interrupt ISR_High_Priority
void ISR_High_Priority(void)
{
	if(PIR1bits.RCIF)
	{
		rx_dt = ReadUSART();
		new_dt = 1;
		PIR1bits.RCIF = 0;	
	}
}

//----------------------------------------------------------------
void main(void)
{
	PORTB = 0x00;
	PORTD = 0x00;		
	
	ADCON1 = 0x07;			// PORTA como pinos digitais

	TRISB = 0b00001111;
	TRISC = 0b10111001;		// RC7: Rx e RC6:Tx
	TRISD = 0x00;
	TRISE = 0b00000100;

	OpenUSART(	USART_TX_INT_OFF &		// Int. de transmiss�o habilitada
				USART_RX_INT_ON &		// Int. de recep��o habilitada
				USART_ASYNCH_MODE &		// Comunica��o ass�ncrona
				USART_EIGHT_BIT &		
				USART_CONT_RX &			// Recep��o cont�nua
				USART_BRGH_HIGH, 25 );	// Alta velocidade 9600bps

	RCONbits.IPEN = 1;			//Habilita prioridade de interrup��es
	IPR1bits.RCIP = 1;			//Int. USART (RX)  � de alta prioridade
	INTCONbits.GIE = 1;			//Habilita interrup��es
	INTCONbits.PEIE = 1;		//Habilita interrup��es de perif�ricos

	PIR1bits.RCIF = 0;			//Inicializa flag de interrup��es
	inicia_lcd();

	while(1)	//Loop principal
	{
		while((PORTB | 0xF0) == 0xFF)
		{
			if(new_dt)
			{
				new_dt = 0;
				if(rx_dt != '0')
					escreve_dado(rx_dt);
				else
				{
					escreve_comando(0xC3);
					//for...
					escreve_dado('X');
					escreve_dado('X');
					escreve_dado('X');
				}
			}
		}
		Delay1KTCYx(20);	//Delay para debounce das teclas


		while((PORTB | 0xF0) != 0xFF){}
		Delay1KTCYx(30);	//Delay para debounce das teclas
		while((PORTB | 0xF0) != 0xFF){}
	}

}

//----------------------------------------------------------------
//Envia comando para o LCD
void escreve_comando(char c)
{
	PORTEbits.RE0=0;
	PORTD=c;
	Delay10TCYx(1);
	PORTEbits.RE1=1;
	Delay1TCY();
	Delay1TCY();
	Delay1TCY();
	PORTEbits.RE1=0;
	Delay1KTCYx(1);
}

//----------------------------------------------------------------
//Escrita de uma dado no LCD
void escreve_dado(char d)
{
PORTEbits.RE0=1;
PORTD=d;
Delay10TCYx(1);
PORTEbits.RE1=1;
Delay1TCY();
Delay1TCY();
Delay1TCY();
PORTEbits.RE1=0;
Delay1KTCYx(1);
PORTEbits.RE0=1;
}

//----------------------------------------------------------------
//Fun��o de inicializa��o do LCD
void inicia_lcd(void)
{
	escreve_comando(0x38);
	Delay1KTCYx(3);
	escreve_comando(0x38);
	escreve_comando(0x06);
	escreve_comando(0x0C);
	escreve_comando(0x01);
	Delay1KTCYx(1);
	escreve_comando(0x80);	//Posiciona cursor

	escreve_dado('.');
	escreve_comando(0xC0);
	escreve_dado('R');
	escreve_dado('X');
	escreve_dado(':');
}