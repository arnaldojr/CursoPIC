#include<p18f452.h>
#include<delays.h>
#include<pwm.h>
#include<compare.h>
#include<timers.h>
#include<math.h>

#pragma config WDT = OFF, LVP = OFF, OSC = XT, PWRT = ON, BOR = ON, BORV = 42

//Protótipos de funções
void ISR_High_Priority(void);
void ISR_Low_Priority(void);
void refresh_lcd(int x);			
void inicia_lcd(void);			//Função de inicialização do LCD
void escreve_comando(char c);	//Envia comando para o LCD
void escreve_dado(char d);		//Escrita de uma dado no LCD

//Definição de variáveis globais
unsigned int dutyPWM = 0;
unsigned char W_temp, BSR_temp, STATUS_temp;
unsigned char flags = 0, ct = 0, div_freq_tmr0 = 20;
unsigned char decod[] = {'0','1','2','3','4','5','6','7','8','9'};
volatile unsigned int freq, medida = 0;

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
	INTCONbits.TMR0IF = 0;
	WriteTimer0(15536);

	if (!(div_freq_tmr0--))
	{
		div_freq_tmr0 = 20;
		freq = medida;			
		medida = 0;				//Zera var. medida para o próximo ciclo
		flags = (flags | 0b00000001);
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
	PIR1bits.CCP1IF = 0;	
	medida++;
	_asm
	MOVFF	W_temp, WREG
	MOVFF	STATUS_temp, STATUS
	MOVFF	BSR_temp, BSR
	RETFIE 0
	_endasm
}

//----------------------------------------------------------------
void main(void)
{
	TRISA = 0xFF; TRISB = 0b00001111;
	TRISC = 0b10111001; TRISD = 0x00; TRISE = 0x00;

	PORTB = 0x00; PORTD = 0x00;	

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
				T0_PS_1_1);					//Prescaler 1:1

	WriteTimer0(15536);		//15536 = 65536 - 50000 (50000 ciclos de p/ estouro)

	PIR1bits.CCP1IF = 0;
	INTCONbits.TMR0IF = 0;		//Inicializa flags de interrupções

	RCONbits.IPEN = 1;			//Habilita prioridade de interrupções
	INTCONbits.GIE = 1;			//Habilita interrupções de alta prioridade
	INTCONbits.PEIE = 1;		//Habilita interrupções de baixa prioridade

	OpenPWM2(24);	//Tpwm = 25us => maxPWM = 100
	SetDCPWM2(dutyPWM);
	DisablePullups();

	inicia_lcd();

	while(1)	//Loop principal
	{
		while((PORTBbits.RB2 == 1) && (PORTBbits.RB3 == 1))
		{
			if(flags & 0b00000001)
			{
				int freq_disp = freq;
				refresh_lcd(freq_disp);
				flags = flags & 0b11111110;
			}
		}
		Delay1KTCYx(20);	//Delay para debounce das teclas

		if((PORTBbits.RB2 == 0) && (PORTBbits.RB3 == 1))
		{	//RB2 pressionado
			if(dutyPWM < 100)
			{
				dutyPWM = dutyPWM + 10;
				escreve_comando(0x85 + dutyPWM/10);
				escreve_dado(0xFF);
			}
		}
		else if((PORTBbits.RB2 == 1) && (PORTBbits.RB3 == 0))
		{	//RB3 pressionado
			if(dutyPWM)
			{
				escreve_comando(0x85 + dutyPWM/10);
				escreve_dado(' ');
				dutyPWM = dutyPWM - 10;
			}
		}

		SetDCPWM2(dutyPWM);	//"Seta" novo duty-cycle			
		while(((PORTBbits.RB2 == 0) || (PORTBbits.RB3 == 0))){
		Delay1KTCYx(20);
		while(((PORTBbits.RB2 == 0) || (PORTBbits.RB3 == 0))){}
		}		
	}
}

//----------------------------------------------------------------
void refresh_lcd(int x)
{
	unsigned short centena, dezena, unidade;
	int aux;

	escreve_comando(0xC6);
	centena = ceil(x/100);
	escreve_dado(decod[centena]);
	aux = fmod(x,100);
	dezena = ceil(aux/10);
	escreve_dado(decod[dezena]);
	unidade = fmod(aux,10);
	escreve_dado(decod[unidade]);
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

	escreve_dado(' ');
	escreve_dado('P');
	escreve_dado('W');
	escreve_dado('M');
	escreve_dado(':');
	escreve_dado(' ');
	
	escreve_comando(0xC0);
	escreve_dado('R');
	escreve_dado('o');
	escreve_dado('t');
	escreve_dado('.');
	escreve_dado(':');
	escreve_dado(' ');
	escreve_dado(' ');
	escreve_dado(' ');
	escreve_dado(' ');
	escreve_dado('H');
	escreve_dado('z');	
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