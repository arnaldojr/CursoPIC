#include<p18f452.h>
#include<timers.h>
#include<adc.h>
#include<delays.h>
#include<pwm.h>
#include "xlcd.h"



#pragma config WDT = OFF, LVP = OFF, OSC = XT, PWRT = ON, BOR = ON, BORV = 42

void ISR_High_Priority(void); //Prototipagem da função de tratamento
							  //de interrupções de alta prioridade
unsigned char valor = 0;

char l1[] = "V: ,   V";
char l2[] = " ";
unsigned int i = 0;
unsigned long int Result = 0;
unsigned long int Tensao = 0;
char centena = 0;
char dezena = 0;
char unidade = 0;
unsigned char Nivel_PWM2 = 140;
unsigned long int Nivel_PWM2_display = 0;

#define Liga_LCD		XLCDCommand(0b00001100);
#define Desliga_LCD		XLCDCommand(0b00001000);
#define Sinal			PORTAbits.RA1
#define Bot_inc			PORTBbits.RB1
#define Bot_dec			PORTBbits.RB2

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

#pragma code vec_int_high_priority = 0x08
void vec_int_high_priority(void)
{ _asm GOTO ISR_High_Priority _endasm }

#pragma code
#pragma interrupt ISR_High_Priority
void ISR_High_Priority(void)
{
	WriteTimer0(65536-15625);
	Converte(Tensao);
	//Desliga_LCD;
	XLCDCommand(0x82);
	XLCDPut(centena);
	XLCDCommand(0x84);
	XLCDPut(dezena);
	XLCDPut(unidade);
	//Nivel_PWM2_display = Nivel_PWM2;
	Converte(Nivel_PWM2_display);
	XLCDCommand(0xC0);
	XLCDPut(centena);
	XLCDPut(dezena);
	XLCDPut(unidade);	
	Liga_LCD;
	INTCONbits.TMR0IF = 0;
}


void main(void)
{
	
	TRISA = 0b11111111;
	PORTB = 0b10000010;
	TRISB = 0b00001111;
	TRISC = 0b11111101;
	PORTD = 0x00;
	TRISD = 0x00;
	ADCON0 = 0x07;
	XLCDInit();
	
	Desliga_LCD;
	XLCDL1home();
	XLCDPutRamString(l1);
	XLCDL2home();
	XLCDPutRamString(l2);
	OpenTimer0( TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_32 );	
	OpenTimer2( TIMER_INT_OFF & T0_PS_1_1 );	
	OpenADC( ADC_FOSC_32 & ADC_RIGHT_JUST & ADC_3ANA_0REF, ADC_CH1 & ADC_INT_OFF );
	Desliga_LCD;
	INTCONbits.GIE = 1;  
	OpenPWM2(49);
	SetDCPWM2(Nivel_PWM2);         // Como estamos usando 20kHZ, tem 200 níveis
	while(1) //Loop principal
	{
		ConvertADC();
		while (BusyADC()) {};
		Result = ReadADC();
		Tensao = Result*500/1023;
		Nivel_PWM2_display = Result*200/1023;
		SetDCPWM2(Nivel_PWM2_display);
		/*
		Nivel_PWM2_display = Nivel_PWM2*200/1023;
		if (Bot_inc == 0) {
			Delay10TCYx(10);
			if (Bot_inc == 0){
				if (Nivel_PWM2 != 200) {
					Nivel_PWM2 += 20;
					SetDCPWM2(Nivel_PWM2);
				}
				while (Bot_inc == 0) {}
			}
		}
		else if (Bot_dec == 0) {
			Delay10TCYx(10);
			if (Bot_dec == 0){
				if (Nivel_PWM2 != 0) {
					Nivel_PWM2 -= 20;
					SetDCPWM2(Nivel_PWM2);
				}
				while (Bot_dec == 0) {}
			}
		}
		*/	
	}

}