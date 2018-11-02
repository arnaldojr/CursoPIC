#include<p18f4550.h>
#include<delays.h>
#include<pwm.h>
#include<adc.h>
#include<timers.h>
void teclado (void);

void escreve_comando (unsigned char com);

void escreve_dado (unsigned char dado);

void inicia_lcd (void);

void le_ad (void);

void exibe_referencia (void);

void alta (void);

void alta_rot (void);

void exibe_ad (void);

void le_tabela(void);

void exibe_tabelalcd(void);

char decoder (char u);

char Bot1, Bot2, Bot3, Bot4, Bot;

unsigned char aux;

unsigned char dado;

unsigned char com;

unsigned char PORT_aux;

unsigned char Valor_Inicial_L;

unsigned char Valor_Inicial_H;

unsigned char EntraRef, ZeraRef, RecuaRef, AvancaRef;

unsigned char xtablow, xtabhi, low1, low2, hi1, hi2, u;

int x;

int y;

int i, posicaolcd, contador;

//15 é o codigo para configuração das portas analogicas (datacheet PIC18F4550)
  
 t=0.05;
 ki=1;
    kp=20;

 Bot=0;
 Bot1=0;
 Bot2=0;
 Bot3=0;
 Bot4=0;
 contador=0;

 OpenTimer0(TIMER_INT_ON &
    T0_16BIT &
    T0_SOURCE_INT &
    T0_PS_1_16);

 Valor_Inicial_H=0x6D;
 Valor_Inicial_L=0x84;

 while (1)   //LOOP Principal (While Infinito)
 {     //WHILE START
  le_ad();     
  le_tabela();
  exibe_referencia();
 }
}
#pragma code int_alta=0x008  //Endereço de interrupção de alta prioridade
 void alta(void)
 {
  _asm
  GOTO 0x1000
  _endasm
 }

#pragma code int_alta_rot=0x1000

 void alta_rot(void)
 
{
 TMR0L=Valor_Inicial_L;
 TMR0H=Valor_Inicial_H;

 //Implementando a equação de diferenças

  le_ad();

  temp=x;

  temp=5*temp/1023;

  ref = 1000*Bot1 + 100*Bot2 + 10*Bot3 + Bot4;
  ref = 0.001*ref;

void le_tabela(void)   //FUNÇÃO PARA LER TABELA


{
 TBLPTR = 0X2000+(2*x);  //PONTEIRO
 _asm
 TBLRD      //COMANDO PARA LEITURA
 _endasm
 xtablow=TABLAT;    //byte lido esta carregado em TABLAT
 TBLPTR++;
 _asm
 TBLRD      //COMANDO PARA LEITURA
 _endasm
 xtabhi=TABLAT;
 low1= xtablow & 0b00001111; //salva os valores de 0a9
 low1= decoder(low1);

void exibe_referencia(void)
{
 escreve_comando(0x80);


 escreve_dado('R');
 escreve_dado('e');
 escreve_dado('f');
 escreve_dado(':');
 escreve_dado(' ');
 teclado();
 
 escreve_dado(decoder(Bot1));
 escreve_dado(',');
 escreve_dado(decoder(Bot2));
 escreve_dado(decoder(Bot3));
 escreve_dado(decoder(Bot4));
}

void teclado(void)
{

 //Multipliexação do Teclado
 
 if (PORTDbits.RD0==1)  //Testa o Bit 2 da Porta D
 {
  PORTD=0b00000010; 
 }
 else if (PORTDbits.RD1==1) //Testa o Bit 3 da Porta D
 {
  PORTD=0b00000100; 
 }
 else if (PORTDbits.RD2==1) //Testa o Bit 4 da Porta D
 {
  PORTD=0b00001000; 
 }
 else if (PORTDbits.RD3==1) //Testa o Bit 1 da Porta D
 {
  PORTD=0b00000001; 
 }

 Delay10KTCYx(1);   //Tempo para o sinal de Multiplexação (10000 Cliclos de Maquina)

 //Teste do Teclado -------------------------------------------------------------------------

 //Primeira Linha
 if ((PORTDbits.RD0==1) &(PORTDbits.RD4==1))    //Tecla S1
 {
  if (contador >= 4) {contador = 0;}
  if(contador == 0) {Bot1 = 1;}
  if(contador == 1) {Bot2 = 1;}
  if(contador == 2) {Bot3 = 1;}
  if(contador == 3) {Bot4 = 1;}
  contador++;  

 }
 else if ((PORTDbits.RD1==1) & (PORTDbits.RD4==1))  //Tecla S2
 {
  if (contador >= 4) {contador = 0;}
  if(contador == 0) {Bot1 = 2;}
  if(contador == 1) {Bot2 = 2;}
  if(contador == 2) {Bot3 = 2;}
  if(contador == 3) {Bot4 = 2;}
  contador++; 

 }
 else if ((PORTDbits.RD2==1) & (PORTDbits.RD4==1))  //Tecla S3
 {
  if (contador >= 4) {contador = 0;}
  if(contador == 0) {Bot1 = 3;}
  if(contador == 1) {Bot2 = 3;}
  if(contador == 2) {Bot3 = 3;}
  if(contador == 3) {Bot4 = 3;}
  contador++; 

 }
 else if ((PORTDbits.RD3==1) & (PORTDbits.RD4==1))  //Tecla S4
 {
  //reservado para o cooler
 }
 else if ((PORTDbits.RD0==1) & (PORTDbits.RD5==1))  //Tecla S5
 {
  if (contador >= 4) {contador = 0;}
  if(contador == 0) {Bot1 = 4;}
  if(contador == 1) {Bot2 = 4;}
  if(contador == 2) {Bot3 = 4;}
  if(contador == 3) {Bot4 = 4;}
  contador++; 

 }
 else if ((PORTDbits.RD1==1) & (PORTDbits.RD5==1))  //Tecla S6
 {
  if (contador >= 4) {contador = 0;}
  if(contador == 0) {Bot1 = 5;}
  if(contador == 1) {Bot2 = 5;}
  if(contador == 2) {Bot3 = 5;}
  if(contador == 3) {Bot4 = 5;}
  contador++; 
  if (Bot1 == 5) {
   contador = 0;
   Bot2 = 0;
   Bot3 = 0;
   Bot4 = 0;}

 }
 else if ((PORTDbits.RD2==1) & (PORTDbits.RD5==1))  //Tecla S7
 {
  if (contador >= 4) {contador = 0;}
  if(contador == 0) {Bot1 = 0;}
  if(contador == 1) {Bot2 = 6;}
  if(contador == 2) {Bot3 = 6;}
  if(contador == 3) {Bot4 = 6;}
  contador++; 

 }
 else if ((PORTDbits.RD3==1) & (PORTDbits.RD5==1))  //Tecla S8
 {
  //reservado para o cooler 
 }
 else if ((PORTDbits.RD0==1) & (PORTDbits.RD6==1))  //Tecla S9
 {
  if (contador >= 4) {contador = 0;}
  if(contador == 0) {Bot1 = 0;}
  if(contador == 1) {Bot2 = 7;}
  if(contador == 2) {Bot3 = 7;}
  if(contador == 3) {Bot4 = 7;}
  contador++; 

 }
 else if ((PORTDbits.RD1==1) & (PORTDbits.RD6==1))  //Tecla S10
 {
  if (contador >= 4) {contador = 0;}
  if(contador == 0) {Bot1 = 0;}
  if(contador == 1) {Bot2 = 8;}
  if(contador == 2) {Bot3 = 8;}
  if(contador == 3) {Bot4 = 8;}
  contador++; 

 }
 else if ((PORTDbits.RD2==1) & (PORTDbits.RD6==1))  //Tecla S11
 {
  if (contador >= 4) {contador = 0;}
  if(contador == 0) {Bot1 = 0;}
  if(contador == 1) {Bot2 = 9;}
  if(contador == 2) {Bot3 = 9;}
  if(contador == 3) {Bot4 = 9;}
  contador++; 

 }
 else if ((PORTDbits.RD3==1) & (PORTDbits.RD6==1))  //Tecla S12
 {
  //reservado para o cooler
 }
 else if ((PORTDbits.RD0==1) & (PORTDbits.RD7==1))  //Tecla S13
 {
  //EntraRef=1;
  //dec

 }
 
 else if ((PORTDbits.RD1==1) & (PORTDbits.RD7==1))  //Tecla S14
 {
  if (contador >= 4) {contador = 0;}
  if(contador == 0) {Bot1 = 0;}
  if(contador == 1) {Bot2 = 0;}
  if(contador == 2) {Bot3 = 0;}
  if(contador == 3) {Bot4 = 0;}
  contador++; 

 }
 
 else if ((PORTDbits.RD2==1) & (PORTDbits.RD7==1))  //Tecla S15
 {
  //AvancaRef=1;
  if(posicaolcd==1){
   posicaolcd=2;
  }
  else if(posicaolcd==2){
    posicaolcd=3;
  }
  else if(posicaolcd==3){
    posicaolcd=1;
 }
 else if ((PORTDbits.RD3==1) & (PORTDbits.RD7==1))  //Tecla S16
 {
  //reservado para o cooler
 }
 }
}