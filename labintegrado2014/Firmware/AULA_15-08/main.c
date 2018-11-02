// Firmware desenvolvido para o hardware da placa McLabII - comunicação serial
#include<p18f452.h>
#include<delays.h>
#include<usart.h>

#pragma config WDT = OFF, LVP = OFF, OSC = XT, PWRT = ON


//----------------------------------------------------------------
void main(void)
{
	PORTB = 0x0f;
	PORTD = 0x00;		
	
	ADCON1 = 0x07;			// PORTA como pinos digitais

	TRISB = 0b00001111;
	TRISC = 0b10111001;		// RC7: Rx e RC6:Tx
	TRISD = 0x00;
	TRISE = 0b00000100;

	OpenUSART(	USART_TX_INT_OFF &		// Int. de transmissão habilitada
				USART_RX_INT_OFF &		// Int. de recepção DESLIGADA
				USART_ASYNCH_MODE &		// Comunicação assíncrona
				USART_EIGHT_BIT &		
				USART_CONT_RX &			// Recepção contínua
				USART_BRGH_HIGH, 25 );	// Alta velocidade 9600bps

	while(1)	//Loop principal
	{

		if(DataRdyUSART() == 1)
			PORTD=ReadUSART();
		else if(PORTBbits.RB0 == 0)
			WriteUSART(0);
		else if(PORTBbits.RB1 == 0)
			WriteUSART(1);
		else if(PORTBbits.RB2 == 0)
			WriteUSART(2);
		else if(PORTBbits.RB3 == 0)
			WriteUSART(3);

	}

}




