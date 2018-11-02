#include<p18f452.h>
#include<delays.h>
#include<pwm.h>
#include<compare.h>
#include<timers.h>
#include<math.h>
#include<portb.h>
#include<usart.h>


#pragma config WDT = OFF, LVP = OFF, OSC = XT, PWRT = ON, BOR = ON, BORV = 42

//Protótipos de funções
void ISR_High_Priority(void);
void ISR_Low_Priority(void);			
void inicia_lcd(void);			//Função de inicialização do LCD
void escreve_comando(char c);	//Envia comando para o LCD
void escreve_dado(char d);		//Escrita de um dado no LCD

//Definição de variáveis globais
unsigned int dutyPWM = 0;
unsigned char W_temp, BSR_temp, STATUS_temp;
unsigned char decod[] = {'0','1','2','3','4','5','6','7','8','9'};
unsigned int voltas = 0;
long int velocidade_rps = 0;
unsigned char flag_atualiza_lcd = 0;
unsigned char centena;
unsigned char dezena;
unsigned char unidade;
unsigned char new_dt, rx_dt = 0;

//----------------------------------------------------------------
#pragma code vec_int_high_priority = 0x08
void vec_int_high_priority(void)
{ _asm GOTO ISR_High_Priority _endasm }

#pragma code vec_int_low_priority = 0x18
void vec_int_low_priority(void)
{ _asm GOTO ISR_Low_Priority _endasm }

#pragma code
void ISR_High_Priority(void)
{
	if(PIR1bits.RCIF) {
		rx_dt = ReadUSART();
		new_dt = 1;
		PIR1bits.RCIF = 0;
	}

	if(INTCONbits.TMR0IF) {
	WriteTimer0(65535-31250);   //Estoura a cada 1 segundo com PS 1:32
	
	velocidade_rps = voltas;
	voltas = 0;
	flag_atualiza_lcd = 1;
	
	INTCONbits.TMR0IF = 0;
	}

	_asm
	RETFIE 1
	_endasm
}

//----------------------------------------------------------------
void ISR_Low_Priority(void)
{
	_asm
	MOVFF	WREG,W_temp
	MOVFF	STATUS, STATUS_temp
	MOVFF	BSR, BSR_temp
	_endasm

	voltas++;
	
	PIR1bits.CCP1IF = 0;

	_asm
	MOVFF	W_temp, WREG
	MOVFF	STATUS_temp, STATUS
	MOVFF	BSR_temp, BSR
	RETFIE 0
	_endasm
}

//----------------------------------------------------------------
void Converte (long int valor) {
	centena = 0;
	dezena = 0;
	unidade = 0;
	while ((valor - 100) >= 0) {
		centena++;
		valor -= 100;
	}
	while ((valor - 10) >= 0) {
		dezena++;
		valor -= 10;
	}
	unidade = valor;
	centena = centena + 48;
	dezena = dezena + 48;
	unidade = unidade + 48;
}
//----------------------------------------------------------------
void main(void)
{
	ADCON1 = 0x07;		// PORTA como pinos digitais

	TRISA = 0xFF;
	TRISB = 0b00001100;
	TRISC = 0b11111001;
	TRISD = 0x00;
	TRISE = 0b00000000;

	PORTB = 0x00;
	PORTD = 0x00;	
	
	OpenTimer1(TIMER_INT_OFF & T1_16BIT_RW & T1_SOURCE_EXT
			& T1_PS_1_1 & T1_OSC1EN_OFF & T1_SOURCE_CCP);
	WriteTimer1(0);

	OpenCompare1(COM_INT_ON & COM_TRIG_SEVNT, 7);	//CCP1 como COMPARE e disparo de
													// TMR1 = CCP1H/L
													// e seta valor de comparação

	IPR1bits.CCP1IP = 0;		//COMPARE é de baixa prioridade
	IPR2 = 0;					//Int. de periféricos é de baixa prioridade

	INTCON2 = 0b00000100;		//Int. TMR0 é de alta prioridade
	OpenTimer0(TIMER_INT_ON &				//Liga interrupção
				T0_16BIT & T0_SOURCE_INT &	//Clock interno
				T0_PS_1_32);				//Prescaler 1:32

	WriteTimer0(65535-31250);		//Estoura a cada 1 segundo

	PIR1bits.CCP1IF = 0;
	INTCONbits.TMR0IF = 0;		//Inicializa flags de interrupções

	RCONbits.IPEN = 1;			//Habilita prioridade de interrupções
	IPR1bits.RCIP = 1;			//Int. USART (RX)  é de alta prioridade
	INTCONbits.GIE = 1;			//Habilita interrupções de alta prioridade
	INTCONbits.PEIE = 1;		//Habilita interrupções de baixa prioridade

	OpenPWM2(49);	//Tpwm = 50us => maxPWM = 200
	dutyPWM = 200;
	SetDCPWM2(dutyPWM);

	OpenUSART(	USART_TX_INT_OFF &		// Int. de transmissão habilitada
				USART_RX_INT_ON &		// Int. de recepção habilitada
				USART_ASYNCH_MODE &		// Comunicação assíncrona
				USART_EIGHT_BIT &		
				USART_CONT_RX &			// Recepção contínua
				USART_BRGH_HIGH, 25 );	// Alta velocidade 9600bps

	DisablePullups();
	inicia_lcd();

	while(1)	//Loop principal
	{
		while((PORTBbits.RB2 == 1) && (PORTBbits.RB3 == 1))
		{
			if (flag_atualiza_lcd) {
				flag_atualiza_lcd = 0;
				Converte(velocidade_rps);
				escreve_comando(0x80);
				escreve_dado(centena);
				escreve_dado(dezena);
				escreve_dado(unidade);
				WriteUSART(centena);
				Delay10KTCYx(1);
				WriteUSART(dezena);
				Delay10KTCYx(1);
				WriteUSART(unidade);
				Delay10KTCYx(1);
				WriteUSART(0x0D);
			}

			if(new_dt) {
				escreve_comando(0xC0);
				escreve_dado(rx_dt);
				new_dt = 0;
				if (rx_dt == 'u' && dutyPWM < 200) {
					dutyPWM += 10;
				}
				if (rx_dt == 'd' && dutyPWM > 0) {
					dutyPWM -= 10;
				}
				SetDCPWM2(dutyPWM);
			}
		}

				

		Delay1KTCYx(20);	//Delay para debounce das teclas

		if((PORTBbits.RB2 == 0) && (PORTBbits.RB3 == 1))
		{	//RB2 pressionado
			// Adicione aqui o que deve ser executado quando
			// as tecla RB2 for acionada.
		
		}
		else if((PORTBbits.RB2 == 1) && (PORTBbits.RB3 == 0))
		{	//RB3 pressionado
			// Adicione aqui o que deve ser executado quando
			// as tecla RB3 for acionada.
		}		

		while(((PORTBbits.RB2 == 0) || (PORTBbits.RB3 == 0))){}
		Delay1KTCYx(20);
		while(((PORTBbits.RB2 == 0) || (PORTBbits.RB3 == 0))){}		
	}
	
}

//----------------------------------------------------------------
//Função de inicialização do LCD
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

	escreve_dado('X');
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