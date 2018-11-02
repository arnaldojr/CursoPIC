
// Firmware desenvolvido para o hardware da placa McLabIII
// CONTROLE DE TEMPERATURA + PWM + AD + INTERRUPÇÃO + SERIAL+ LCD

#include<p18f4520.h>
//#include<p18f4550.h>
#include<delays.h>
#include<pwm.h>
#include<adc.h>
#include<timers.h>
#include<usart.h>

//#pragma config PLLDIV = 1, WDT = OFF, LVP = OFF, FOSC = HSPLL_HS, PWRT = ON, BOR = ON		//configuração de fusivel para pic18f4550
#pragma config WDT = OFF, LVP = OFF, OSC = XT, PWRT = ON, BOREN = ON						//configuração de fusivel para pic18f452
//Protótipos de funções
void ISR_High_Priority(void);
void ISR_Low_Priority(void);
void inicia_lcd(void);						//Função de inicialização do LCD
void escreve_comando(unsigned char com);	//Envia comando para o LCD
void escreve_dado(unsigned char dado);		//Escrita de uma dado no LCD
void teclado (void);
void le_ad (void);
void exibe_ad (void);
void le_tabela(void);
void exibe_tabelalcd(void);

//Definição de variáveis globais
char decoder (char u);
unsigned char dado, com, PORT_aux;
unsigned char Valor_Inicial_L, Valor_Inicial_H;
unsigned char Bot1, Bot2, Bot3, Bot4, Bot5, Bot6, Bot7, Bot8, Bot9, Bot0, EntraRef, ZeraRef, RecuaRef, AvancaRef, posicaolcd;
unsigned char xtablow, xtabhi, low1, low2, hi1, hi2, u;
int x,y,i, PWM;
float ui0, ui, up, e, e0, up, temp, ref;
float	t=0.05;								//usado na equação de diferença
float	ki=0.1;								//constante do integrador
float	kp=1;								//constante do proporcional;
unsigned int dutyPWM = 0;					//DutyCyclo PWM

 //----------------------------------------------------------------
#pragma code vec_int_high_priority = 0x08	//Endereço de interrupção de alta prioridade
void vec_int_high_priority(void)
{ _asm GOTO ISR_High_Priority _endasm }

#pragma code 
void ISR_High_Priority(void)
{
	if(INTCONbits.TMR0IF)
	{
		TMR0L=Valor_Inicial_L;
		TMR0H=Valor_Inicial_H;
	
		//Implementando a equação de diferenças
		le_ad();
		temp=x;
	//	WriteUSART(1);				//TX
	//	putsUSART("temp");
		ref=512;
		e=ref-temp;
  		ui=((ki*e*t)*0.5)+((ki*e0*t)*0.5)+ui0;
		up=kp*e;
		e0=e;
		ui0=ui;
		dutyPWM=ui+up;
		SetDCPWM1(dutyPWM);

		INTCONbits.TMR0IF=0; 			//Limpando o flag de interrupção
	}
} 
  
void main(void)
{
	//----------------------------------------------------------------
	//Configurações das Portas
	TRISB=0X00;   			//LEDs
	TRISC=0b11111001;
	TRISD=0x00;   					//Teclado Matricial
	TRISE=0X00;
	
	ADCON1=15; 						//15 é o codigo para configuração das portas analogivas (datacheet PIC18F45
	
	PORTE=0X00;
	PORTC=0X00;
	PORTB=0X00;
	PORTD=0b00000001; 				//Iniciando Portas
	
	
	Delay10KTCYx(255); 				//Tempo para LCD
	inicia_lcd(); 
	
	OpenPWM1(0xFF);  				//Habilita o PWM2 com 10 bits
	SetDCPWM1(dutyPWM);				//SetDCPWM2 é uma função que admite como parametro
									//um numero de 0 (0% de PWM) ate 1023 (100% de PWM)
	
	
	OpenADC(ADC_FOSC_64 &   		//CLOCK do ADC
			ADC_RIGHT_JUST &  		//Justificação a direita
			ADC_2_TAD,    			//Tempo de aquisição
			ADC_CH0 &    			//Selecionando o Canal 0
			ADC_INT_OFF &   		//Interrupção desligada
			ADC_VREFPLUS_VDD &  	//Referencia maxima
			ADC_VREFMINUS_VSS, 15); //referencia minima //15 é o codigo para configuração das portas analogivas (datacheet PIC18F4550)
			INTCON2bits.TMR0IP=1;
			RCONbits.IPEN=1;
			INTCONbits.GIE=1;
									
	OpenUSART(	USART_TX_INT_OFF &		// Int. de transmissão habilitada
				USART_RX_INT_OFF &		// Int. de recepção habilitada
				USART_ASYNCH_MODE &		// Comunicação assíncrona
				USART_EIGHT_BIT &		
				USART_CONT_RX &			// Recepção contínua
				USART_BRGH_HIGH, 25 );	// Alta velocidade 9600bps
	
	
	
	OpenTimer0(TIMER_INT_ON &		//habilita interrupção
				T0_16BIT &			//Modo 16bits
				T0_SOURCE_INT &		//Fonte de clock interno Tosc
				T0_PS_1_16);		//Prescale 1:16
				
				
	Valor_Inicial_H=0x6D;
	Valor_Inicial_L=0x84;
	

	while (1)   //LOOP Principal (While Infinito)
	{     //WHILE START
	putsUSART('oi');
	//le_ad();     
	//teclado();  //Rotina de teste do Teclado
	//le_tabela();
	}
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



void le_ad (void) //Rotina que le o AD
{
 ConvertADC();   //Inicia a conversão AD
 while(BusyADC()==1)  //Espera o fim da Conversão
 {}
 x=ReadADC();   //variavel "x" deve ser INT ou Unsigned INT
}
//void exibe_ad (void)  //rotina que exibe no lcd o valor lido no AD
//{
// le_ad();
// y=0x0001;
// //escreve_comando(0x01); //Limpa LCD 
// 
// escreve_comando(0x80);
// escreve_dado('T');
// escreve_dado('e');
// escreve_dado('m');
// escreve_dado('.');
// for (i=0;i<=9;i++)
// {
//  if ((x&y)!=0)
//  {
//   escreve_dado('1');
//  }
//  else
//  {
//   escreve_dado('0');
//  }
//  y=y<<1;
// }
//
//}

void le_tabela(void)   				//FUNÇÃO PARA LER TABELA
{
	TBLPTR = 0X2000+(2*x);  		//PONTEIRO
	_asm	TBLRD	_endasm			//COMANDO PARA LEITURA
 	xtablow=TABLAT;    				//byte lido esta carregado em TABLAT
	TBLPTR++;
	_asm	TBLRD	_endasm 		//COMANDO PARA LEITURA
	xtabhi=TABLAT;
	low1= xtablow& 0b00001111; //salva os valores de 0a9
	low1= decoder(low1);
	xtablow=xtablow>>4;
	low2= xtablow;
	low2= decoder(low2);
	hi1= xtabhi& 0b00001111;
	hi1= decoder(hi1);
	xtabhi=xtabhi>>4;
	hi2= xtabhi;
	hi2= decoder(hi2);
	exibe_tabelalcd();
}



void  exibe_tabelalcd(void)
{
	escreve_comando(0xc0);
 	escreve_dado('T');
	escreve_dado('e');
	escreve_dado('n');
	escreve_dado('s');
	escreve_dado('a');
	escreve_dado('o');
	escreve_dado(':');
	escreve_dado(' ');
	escreve_dado(hi2);
	escreve_dado(',');
	escreve_dado(hi1);
	escreve_dado(low2);
	escreve_dado(low1);
}
void exibe_referencia(void)
{
  
 escreve_comando(0xc0);
 escreve_dado('R');
 escreve_dado('e');
 escreve_dado('f');
 escreve_dado(':');
 escreve_dado(' ');
 escreve_dado(hi2);
 escreve_dado(',');
 escreve_dado(hi1);
 escreve_dado(low2);
 escreve_dado(low1);
}

void teclado(void)
{
 //Multipliexação do Teclado
 
 if (PORTDbits.RD0==1)  //Testa o Bit 0 da Porta D
 {
  PORTD=0b00000010; 
 }
 else if (PORTDbits.RD1==1) //Testa o Bit 1 da Porta D
 {
  PORTD=0b00000100; 
 }
 else if (PORTDbits.RD2==1) //Testa o Bit 2 da Porta D
 {
  PORTD=0b00001000; 
 }
 else if (PORTDbits.RD3==1) //Testa o Bit 3 da Porta D
 {
  PORTD=0b00000001; 
 }
 Delay10KTCYx(1);    //Tempo para o sinal de Multiplexação (10000 Cliclos de Maquina)
 //Teste do Teclado -------------------------------------------------------------------------
 //Primeira Linha
 if ((PORTDbits.RD0==1) &(PORTDbits.RD4==1))    //Tecla S1
 {
  Bot1=1;
  
 }
 else if ((PORTDbits.RD1==1) & (PORTDbits.RD4==1))  //Tecla S2
 {
  Bot2=2;
 }
 else if ((PORTDbits.RD2==1) & (PORTDbits.RD4==1))  //Tecla S3
 {
  Bot3=3;
 }
 else if ((PORTDbits.RD3==1) & (PORTDbits.RD4==1))  //Tecla S4
 {
  //reservado para o cooler
 }
 else if ((PORTDbits.RD0==1) & (PORTDbits.RD5==1))  //Tecla S5
 {
  Bot4=4;
 }
 else if ((PORTDbits.RD1==1) & (PORTDbits.RD5==1))  //Tecla S6
 {
  Bot5=5;
 }
 else if ((PORTDbits.RD2==1) & (PORTDbits.RD5==1))  //Tecla S7
 {
  Bot6=6;
 }
 else if ((PORTDbits.RD3==1) & (PORTDbits.RD5==1))  //Tecla S8
 {
  //reservado para o cooler
 
 }
 else if ((PORTDbits.RD0==1) & (PORTDbits.RD6==1))  //Tecla S9
 {
  Bot7=7;
 }
 else if ((PORTDbits.RD1==1) & (PORTDbits.RD6==1))  //Tecla S10
 {
  Bot8=8;
 }
 else if ((PORTDbits.RD2==1) & (PORTDbits.RD6==1))  //Tecla S11
 {
  
  Bot9=9;
 }
 else if ((PORTDbits.RD3==1) & (PORTDbits.RD6==1))  //Tecla S12
 {
  //reservado para o cooler
 }
 else if ((PORTDbits.RD0==1) & (PORTDbits.RD7==1))  //Tecla S13
 {
  //EntraRef=1;
 }
 
 else if ((PORTDbits.RD1==1) & (PORTDbits.RD7==1))  //Tecla S14
 {
  Bot0=0;
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
 
 
 
 
 
//*************************  Tabela CAD  **************************************
 

#pragma romdata tabela=0x2000

unsigned int rom as0=0x00;
unsigned int rom a1=0x05;
unsigned int rom a2=0x10;
unsigned int rom a3=0x15;
unsigned int rom a4=0x20;
unsigned int rom a5=0x24;
unsigned int rom a6=0x29;
unsigned int rom a7=0x34;
unsigned int rom a8=0x39;
unsigned int rom a9=0x44;
unsigned int rom a10=0x49;
unsigned int rom a11=0x54;
unsigned int rom a12=0x59;
unsigned int rom a13=0x64;
unsigned int rom a14=0x68;
unsigned int rom a15=0x73;
unsigned int rom a16=0x78;
unsigned int rom a17=0x83;
unsigned int rom a18=0x88;
unsigned int rom a19=0x93;
unsigned int rom a20=0x98;
unsigned int rom a21=0x103;
unsigned int rom a22=0x108;
unsigned int rom a23=0x112;
unsigned int rom a24=0x117;
unsigned int rom a25=0x122;
unsigned int rom a26=0x127;
unsigned int rom a27=0x132;
unsigned int rom a28=0x137;
unsigned int rom a29=0x142;
unsigned int rom a30=0x147;
unsigned int rom a31=0x152;
unsigned int rom a32=0x156;
unsigned int rom a33=0x161;
unsigned int rom a34=0x166;
unsigned int rom a35=0x171;
unsigned int rom a36=0x176;
unsigned int rom a37=0x181;
unsigned int rom a38=0x186;
unsigned int rom a39=0x191;
unsigned int rom a40=0x196;
unsigned int rom a41=0x200;
unsigned int rom a42=0x205;
unsigned int rom a43=0x210;
unsigned int rom a44=0x215;
unsigned int rom a45=0x220;
unsigned int rom a46=0x225;
unsigned int rom a47=0x230;
unsigned int rom a48=0x235;
unsigned int rom a49=0x239;
unsigned int rom a50=0x244;
unsigned int rom a51=0x249;
unsigned int rom a52=0x254;
unsigned int rom a53=0x259;
unsigned int rom a54=0x264;
unsigned int rom a55=0x269;
unsigned int rom a56=0x274;
unsigned int rom a57=0x279;
unsigned int rom a58=0x283;
unsigned int rom a59=0x288;
unsigned int rom a60=0x293;
unsigned int rom a61=0x298;
unsigned int rom a62=0x303;
unsigned int rom a63=0x308;
unsigned int rom a64=0x313;
unsigned int rom a65=0x318;
unsigned int rom a66=0x323;
unsigned int rom a67=0x327;
unsigned int rom a68=0x332;
unsigned int rom a69=0x337;
unsigned int rom a70=0x342;
unsigned int rom a71=0x347;
unsigned int rom a72=0x352;
unsigned int rom a73=0x357;
unsigned int rom a74=0x362;
unsigned int rom a75=0x367;
unsigned int rom a76=0x371;
unsigned int rom a77=0x376;
unsigned int rom a78=0x381;
unsigned int rom a79=0x386;
unsigned int rom a80=0x391;
unsigned int rom a81=0x396;
unsigned int rom a82=0x401;
unsigned int rom a83=0x406;
unsigned int rom a84=0x411;
unsigned int rom a85=0x415;
unsigned int rom a86=0x420;
unsigned int rom a87=0x425;
unsigned int rom a88=0x430;
unsigned int rom a89=0x435;
unsigned int rom a90=0x440;
unsigned int rom a91=0x445;
unsigned int rom a92=0x450;
unsigned int rom a93=0x455;
unsigned int rom a94=0x459;
unsigned int rom a95=0x464;
unsigned int rom a96=0x469;
unsigned int rom a97=0x474;
unsigned int rom a98=0x479;
unsigned int rom a99=0x484;
unsigned int rom a100=0x489;
unsigned int rom a101=0x494;
unsigned int rom a102=0x499;
unsigned int rom a103=0x503;
unsigned int rom a104=0x508;
unsigned int rom a105=0x513;
unsigned int rom a106=0x518;
unsigned int rom a107=0x523;
unsigned int rom a108=0x528;
unsigned int rom a109=0x533;
unsigned int rom a110=0x538;
unsigned int rom a111=0x543;
unsigned int rom a112=0x547;
unsigned int rom a113=0x552;
unsigned int rom a114=0x557;
unsigned int rom a115=0x562;
unsigned int rom a116=0x567;
unsigned int rom a117=0x572;
unsigned int rom a118=0x577;
unsigned int rom a119=0x582;
unsigned int rom a120=0x587;
unsigned int rom a121=0x591;
unsigned int rom a122=0x596;
unsigned int rom a123=0x601;
unsigned int rom a124=0x606;
unsigned int rom a125=0x611;
unsigned int rom a126=0x616;
unsigned int rom a127=0x621;
unsigned int rom a128=0x626;
unsigned int rom a129=0x630;
unsigned int rom a130=0x635;
unsigned int rom a131=0x640;
unsigned int rom a132=0x645;
unsigned int rom a133=0x650;
unsigned int rom a134=0x655;
unsigned int rom a135=0x660;
unsigned int rom a136=0x665;
unsigned int rom a137=0x670;
unsigned int rom a138=0x674;
unsigned int rom a139=0x679;
unsigned int rom a140=0x684;
unsigned int rom a141=0x689;
unsigned int rom a142=0x694;
unsigned int rom a143=0x699;
unsigned int rom a144=0x704;
unsigned int rom a145=0x709;
unsigned int rom a146=0x714;
unsigned int rom a147=0x718;
unsigned int rom a148=0x723;
unsigned int rom a149=0x728;
unsigned int rom a150=0x733;
unsigned int rom a151=0x738;
unsigned int rom a152=0x743;
unsigned int rom a153=0x748;
unsigned int rom a154=0x753;
unsigned int rom a155=0x758;
unsigned int rom a156=0x762;
unsigned int rom a157=0x767;
unsigned int rom a158=0x772;
unsigned int rom a159=0x777;
unsigned int rom a160=0x782;
unsigned int rom a161=0x787;
unsigned int rom a162=0x792;
unsigned int rom a163=0x797;
unsigned int rom a164=0x802;
unsigned int rom a165=0x806;
unsigned int rom a166=0x811;
unsigned int rom a167=0x816;
unsigned int rom a168=0x821;
unsigned int rom a169=0x826;
unsigned int rom a170=0x831;
unsigned int rom a171=0x836;
unsigned int rom a172=0x841;
unsigned int rom a173=0x846;
unsigned int rom a174=0x850;
unsigned int rom a175=0x855;
unsigned int rom a176=0x860;
unsigned int rom a177=0x865;
unsigned int rom a178=0x870;
unsigned int rom a179=0x875;
unsigned int rom a180=0x880;
unsigned int rom a181=0x885;
unsigned int rom a182=0x890;
unsigned int rom a183=0x894;
unsigned int rom a184=0x899;
unsigned int rom a185=0x904;
unsigned int rom a186=0x909;
unsigned int rom a187=0x914;
unsigned int rom a188=0x919;
unsigned int rom a189=0x924;
unsigned int rom a190=0x929;
unsigned int rom a191=0x934;
unsigned int rom a192=0x938;
unsigned int rom a193=0x943;
unsigned int rom a194=0x948;
unsigned int rom a195=0x953;
unsigned int rom a196=0x958;
unsigned int rom a197=0x963;
unsigned int rom a198=0x968;
unsigned int rom a199=0x973;
unsigned int rom a200=0x978;
unsigned int rom a201=0x982;
unsigned int rom a202=0x987;
unsigned int rom a203=0x992;
unsigned int rom a204=0x997;
unsigned int rom a205=0x1002;
unsigned int rom a206=0x1007;
unsigned int rom a207=0x1012;
unsigned int rom a208=0x1017;
unsigned int rom a209=0x1022;
unsigned int rom a210=0x1026;
unsigned int rom a211=0x1031;
unsigned int rom a212=0x1036;
unsigned int rom a213=0x1041;
unsigned int rom a214=0x1046;
unsigned int rom a215=0x1051;
unsigned int rom a216=0x1056;
unsigned int rom a217=0x1061;
unsigned int rom a218=0x1065;
unsigned int rom a219=0x1070;
unsigned int rom a220=0x1075;
unsigned int rom a221=0x1080;
unsigned int rom a222=0x1085;
unsigned int rom a223=0x1090;
unsigned int rom a224=0x1095;
unsigned int rom a225=0x1100;
unsigned int rom a226=0x1105;
unsigned int rom a227=0x1109;
unsigned int rom a228=0x1114;
unsigned int rom a229=0x1119;
unsigned int rom a230=0x1124;
unsigned int rom a231=0x1129;
unsigned int rom a232=0x1134;
unsigned int rom a233=0x1139;
unsigned int rom a234=0x1144;
unsigned int rom a235=0x1149;
unsigned int rom a236=0x1153;
unsigned int rom a237=0x1158;
unsigned int rom a238=0x1163;
unsigned int rom a239=0x1168;
unsigned int rom a240=0x1173;
unsigned int rom a241=0x1178;
unsigned int rom a242=0x1183;
unsigned int rom a243=0x1188;
unsigned int rom a244=0x1193;
unsigned int rom a245=0x1197;
unsigned int rom a246=0x1202;
unsigned int rom a247=0x1207;
unsigned int rom a248=0x1212;
unsigned int rom a249=0x1217;
unsigned int rom a250=0x1222;
unsigned int rom a251=0x1227;
unsigned int rom a252=0x1232;
unsigned int rom a253=0x1237;
unsigned int rom a254=0x1241;
unsigned int rom a255=0x1246;
unsigned int rom a256=0x1251;
unsigned int rom a257=0x1256;
unsigned int rom a258=0x1261;
unsigned int rom a259=0x1266;
unsigned int rom a260=0x1271;
unsigned int rom a261=0x1276;
unsigned int rom a262=0x1281;
unsigned int rom a263=0x1285;
unsigned int rom a264=0x1290;
unsigned int rom a265=0x1295;
unsigned int rom a266=0x1300;
unsigned int rom a267=0x1305;
unsigned int rom a268=0x1310;
unsigned int rom a269=0x1315;
unsigned int rom a270=0x1320;
unsigned int rom a271=0x1325;
unsigned int rom a272=0x1329;
unsigned int rom a273=0x1334;
unsigned int rom a274=0x1339;
unsigned int rom a275=0x1344;
unsigned int rom a276=0x1349;
unsigned int rom a277=0x1354;
unsigned int rom a278=0x1359;
unsigned int rom a279=0x1364;
unsigned int rom a280=0x1369;
unsigned int rom a281=0x1373;
unsigned int rom a282=0x1378;
unsigned int rom a283=0x1383;
unsigned int rom a284=0x1388;
unsigned int rom a285=0x1393;
unsigned int rom a286=0x1398;
unsigned int rom a287=0x1403;
unsigned int rom a288=0x1408;
unsigned int rom a289=0x1413;
unsigned int rom a290=0x1417;
unsigned int rom a291=0x1422;
unsigned int rom a292=0x1427;
unsigned int rom a293=0x1432;
unsigned int rom a294=0x1437;
unsigned int rom a295=0x1442;
unsigned int rom a296=0x1447;
unsigned int rom a297=0x1452;
unsigned int rom a298=0x1457;
unsigned int rom a299=0x1461;
unsigned int rom a300=0x1466;
unsigned int rom a301=0x1471;
unsigned int rom a302=0x1476;
unsigned int rom a303=0x1481;
unsigned int rom a304=0x1486;
unsigned int rom a305=0x1491;
unsigned int rom a306=0x1496;
unsigned int rom a307=0x1500;
unsigned int rom a308=0x1505;
unsigned int rom a309=0x1510;
unsigned int rom a310=0x1515;
unsigned int rom a311=0x1520;
unsigned int rom a312=0x1525;
unsigned int rom a313=0x1530;
unsigned int rom a314=0x1535;
unsigned int rom a315=0x1540;
unsigned int rom a316=0x1544;
unsigned int rom a317=0x1549;
unsigned int rom a318=0x1554;
unsigned int rom a319=0x1559;
unsigned int rom a320=0x1564;
unsigned int rom a321=0x1569;
unsigned int rom a322=0x1574;
unsigned int rom a323=0x1579;
unsigned int rom a324=0x1584;
unsigned int rom a325=0x1588;
unsigned int rom a326=0x1593;
unsigned int rom a327=0x1598;
unsigned int rom a328=0x1603;
unsigned int rom a329=0x1608;
unsigned int rom a330=0x1613;
unsigned int rom a331=0x1618;
unsigned int rom a332=0x1623;
unsigned int rom a333=0x1628;
unsigned int rom a334=0x1632;
unsigned int rom a335=0x1637;
unsigned int rom a336=0x1642;
unsigned int rom a337=0x1647;
unsigned int rom a338=0x1652;
unsigned int rom a339=0x1657;
unsigned int rom a340=0x1662;
unsigned int rom a341=0x1667;
unsigned int rom a342=0x1672;
unsigned int rom a343=0x1676;
unsigned int rom a344=0x1681;
unsigned int rom a345=0x1686;
unsigned int rom a346=0x1691;
unsigned int rom a347=0x1696;
unsigned int rom a348=0x1701;
unsigned int rom a349=0x1706;
unsigned int rom a350=0x1711;
unsigned int rom a351=0x1716;
unsigned int rom a352=0x1720;
unsigned int rom a353=0x1725;
unsigned int rom a354=0x1730;
unsigned int rom a355=0x1735;
unsigned int rom a356=0x1740;
unsigned int rom a357=0x1745;
unsigned int rom a358=0x1750;
unsigned int rom a359=0x1755;
unsigned int rom a360=0x1760;
unsigned int rom a361=0x1764;
unsigned int rom a362=0x1769;
unsigned int rom a363=0x1774;
unsigned int rom a364=0x1779;
unsigned int rom a365=0x1784;
unsigned int rom a366=0x1789;
unsigned int rom a367=0x1794;
unsigned int rom a368=0x1799;
unsigned int rom a369=0x1804;
unsigned int rom a370=0x1808;
unsigned int rom a371=0x1813;
unsigned int rom a372=0x1818;
unsigned int rom a373=0x1823;
unsigned int rom a374=0x1828;
unsigned int rom a375=0x1833;
unsigned int rom a376=0x1838;
unsigned int rom a377=0x1843;
unsigned int rom a378=0x1848;
unsigned int rom a379=0x1852;
unsigned int rom a380=0x1857;
unsigned int rom a381=0x1862;
unsigned int rom a382=0x1867;
unsigned int rom a383=0x1872;
unsigned int rom a384=0x1877;
unsigned int rom a385=0x1882;
unsigned int rom a386=0x1887;
unsigned int rom a387=0x1891;
unsigned int rom a388=0x1896;
unsigned int rom a389=0x1901;
unsigned int rom a390=0x1906;
unsigned int rom a391=0x1911;
unsigned int rom a392=0x1916;
unsigned int rom a393=0x1921;
unsigned int rom a394=0x1926;
unsigned int rom a395=0x1931;
unsigned int rom a396=0x1935;
unsigned int rom a397=0x1940;
unsigned int rom a398=0x1945;
unsigned int rom a399=0x1950;
unsigned int rom a400=0x1955;
unsigned int rom a401=0x1960;
unsigned int rom a402=0x1965;
unsigned int rom a403=0x1970;
unsigned int rom a404=0x1975;
unsigned int rom a405=0x1979;
unsigned int rom a406=0x1984;
unsigned int rom a407=0x1989;
unsigned int rom a408=0x1994;
unsigned int rom a409=0x1999;
unsigned int rom a410=0x2004;
unsigned int rom a411=0x2009;
unsigned int rom a412=0x2014;
unsigned int rom a413=0x2019;
unsigned int rom a414=0x2023;
unsigned int rom a415=0x2028;
unsigned int rom a416=0x2033;
unsigned int rom a417=0x2038;
unsigned int rom a418=0x2043;
unsigned int rom a419=0x2048;
unsigned int rom a420=0x2053;
unsigned int rom a421=0x2058;
unsigned int rom a422=0x2063;
unsigned int rom a423=0x2067;
unsigned int rom a424=0x2072;
unsigned int rom a425=0x2077;
unsigned int rom a426=0x2082;
unsigned int rom a427=0x2087;
unsigned int rom a428=0x2092;
unsigned int rom a429=0x2097;
unsigned int rom a430=0x2102;
unsigned int rom a431=0x2107;
unsigned int rom a432=0x2111;
unsigned int rom a433=0x2116;
unsigned int rom a434=0x2121;
unsigned int rom a435=0x2126;
unsigned int rom a436=0x2131;
unsigned int rom a437=0x2136;
unsigned int rom a438=0x2141;
unsigned int rom a439=0x2146;
unsigned int rom a440=0x2151;
unsigned int rom a441=0x2155;
unsigned int rom a442=0x2160;
unsigned int rom a443=0x2165;
unsigned int rom a444=0x2170;
unsigned int rom a445=0x2175;
unsigned int rom a446=0x2180;
unsigned int rom a447=0x2185;
unsigned int rom a448=0x2190;
unsigned int rom a449=0x2195;
unsigned int rom a450=0x2199;
unsigned int rom a451=0x2204;
unsigned int rom a452=0x2209;
unsigned int rom a453=0x2214;
unsigned int rom a454=0x2219;
unsigned int rom a455=0x2224;
unsigned int rom a456=0x2229;
unsigned int rom a457=0x2234;
unsigned int rom a458=0x2239;
unsigned int rom a459=0x2243;
unsigned int rom a460=0x2248;
unsigned int rom a461=0x2253;
unsigned int rom a462=0x2258;
unsigned int rom a463=0x2263;
unsigned int rom a464=0x2268;
unsigned int rom a465=0x2273;
unsigned int rom a466=0x2278;
unsigned int rom a467=0x2283;
unsigned int rom a468=0x2287;
unsigned int rom a469=0x2292;
unsigned int rom a470=0x2297;
unsigned int rom a471=0x2302;
unsigned int rom a472=0x2307;
unsigned int rom a473=0x2312;
unsigned int rom a474=0x2317;
unsigned int rom a475=0x2322;
unsigned int rom a476=0x2326;
unsigned int rom a477=0x2331;
unsigned int rom a478=0x2336;
unsigned int rom a479=0x2341;
unsigned int rom a480=0x2346;
unsigned int rom a481=0x2351;
unsigned int rom a482=0x2356;
unsigned int rom a483=0x2361;
unsigned int rom a484=0x2366;
unsigned int rom a485=0x2370;
unsigned int rom a486=0x2375;
unsigned int rom a487=0x2380;
unsigned int rom a488=0x2385;
unsigned int rom a489=0x2390;
unsigned int rom a490=0x2395;
unsigned int rom a491=0x2400;
unsigned int rom a492=0x2405;
unsigned int rom a493=0x2410;
unsigned int rom a494=0x2414;
unsigned int rom a495=0x2419;
unsigned int rom a496=0x2424;
unsigned int rom a497=0x2429;
unsigned int rom a498=0x2434;
unsigned int rom a499=0x2439;
unsigned int rom a500=0x2444;
unsigned int rom a501=0x2449;
unsigned int rom a502=0x2454;
unsigned int rom a503=0x2458;
unsigned int rom a504=0x2463;
unsigned int rom a505=0x2468;
unsigned int rom a506=0x2473;
unsigned int rom a507=0x2478;
unsigned int rom a508=0x2483;
unsigned int rom a509=0x2488;
unsigned int rom a510=0x2493;
unsigned int rom a511=0x2498;
unsigned int rom a512=0x2502;
unsigned int rom a513=0x2507;
unsigned int rom a514=0x2512;
unsigned int rom a515=0x2517;
unsigned int rom a516=0x2522;
unsigned int rom a517=0x2527;
unsigned int rom a518=0x2532;
unsigned int rom a519=0x2537;
unsigned int rom a520=0x2542;
unsigned int rom a521=0x2546;
unsigned int rom a522=0x2551;
unsigned int rom a523=0x2556;
unsigned int rom a524=0x2561;
unsigned int rom a525=0x2566;
unsigned int rom a526=0x2571;
unsigned int rom a527=0x2576;
unsigned int rom a528=0x2581;
unsigned int rom a529=0x2586;
unsigned int rom a530=0x2590;
unsigned int rom a531=0x2595;
unsigned int rom a532=0x2600;
unsigned int rom a533=0x2605;
unsigned int rom a534=0x2610;
unsigned int rom a535=0x2615;
unsigned int rom a536=0x2620;
unsigned int rom a537=0x2625;
unsigned int rom a538=0x2630;
unsigned int rom a539=0x2634;
unsigned int rom a540=0x2639;
unsigned int rom a541=0x2644;
unsigned int rom a542=0x2649;
unsigned int rom a543=0x2654;
unsigned int rom a544=0x2659;
unsigned int rom a545=0x2664;
unsigned int rom a546=0x2669;
unsigned int rom a547=0x2674;
unsigned int rom a548=0x2678;
unsigned int rom a549=0x2683;
unsigned int rom a550=0x2688;
unsigned int rom a551=0x2693;
unsigned int rom a552=0x2698;
unsigned int rom a553=0x2703;
unsigned int rom a554=0x2708;
unsigned int rom a555=0x2713;
unsigned int rom a556=0x2717;
unsigned int rom a557=0x2722;
unsigned int rom a558=0x2727;
unsigned int rom a559=0x2732;
unsigned int rom a560=0x2737;
unsigned int rom a561=0x2742;
unsigned int rom a562=0x2747;
unsigned int rom a563=0x2752;
unsigned int rom a564=0x2757;
unsigned int rom a565=0x2761;
unsigned int rom a566=0x2766;
unsigned int rom a567=0x2771;
unsigned int rom a568=0x2776;
unsigned int rom a569=0x2781;
unsigned int rom a570=0x2786;
unsigned int rom a571=0x2791;
unsigned int rom a572=0x2796;
unsigned int rom a573=0x2801;
unsigned int rom a574=0x2805;
unsigned int rom a575=0x2810;
unsigned int rom a576=0x2815;
unsigned int rom a577=0x2820;
unsigned int rom a578=0x2825;
unsigned int rom a579=0x2830;
unsigned int rom a580=0x2835;
unsigned int rom a581=0x2840;
unsigned int rom a582=0x2845;
unsigned int rom a583=0x2849;
unsigned int rom a584=0x2854;
unsigned int rom a585=0x2859;
unsigned int rom a586=0x2864;
unsigned int rom a587=0x2869;
unsigned int rom a588=0x2874;
unsigned int rom a589=0x2879;
unsigned int rom a590=0x2884;
unsigned int rom a591=0x2889;
unsigned int rom a592=0x2893;
unsigned int rom a593=0x2898;
unsigned int rom a594=0x2903;
unsigned int rom a595=0x2908;
unsigned int rom a596=0x2913;
unsigned int rom a597=0x2918;
unsigned int rom a598=0x2923;
unsigned int rom a599=0x2928;
unsigned int rom a600=0x2933;
unsigned int rom a601=0x2937;
unsigned int rom a602=0x2942;
unsigned int rom a603=0x2947;
unsigned int rom a604=0x2952;
unsigned int rom a605=0x2957;
unsigned int rom a606=0x2962;
unsigned int rom a607=0x2967;
unsigned int rom a608=0x2972;
unsigned int rom a609=0x2977;
unsigned int rom a610=0x2981;
unsigned int rom a611=0x2986;
unsigned int rom a612=0x2991;
unsigned int rom a613=0x2996;
unsigned int rom a614=0x3001;
unsigned int rom a615=0x3006;
unsigned int rom a616=0x3011;
unsigned int rom a617=0x3016;
unsigned int rom a618=0x3021;
unsigned int rom a619=0x3025;
unsigned int rom a620=0x3030;
unsigned int rom a621=0x3035;
unsigned int rom a622=0x3040;
unsigned int rom a623=0x3045;
unsigned int rom a624=0x3050;
unsigned int rom a625=0x3055;
unsigned int rom a626=0x3060;
unsigned int rom a627=0x3065;
unsigned int rom a628=0x3069;
unsigned int rom a629=0x3074;
unsigned int rom a630=0x3079;
unsigned int rom a631=0x3084;
unsigned int rom a632=0x3089;
unsigned int rom a633=0x3094;
unsigned int rom a634=0x3099;
unsigned int rom a635=0x3104;
unsigned int rom a636=0x3109;
unsigned int rom a637=0x3113;
unsigned int rom a638=0x3118;
unsigned int rom a639=0x3123;
unsigned int rom a640=0x3128;
unsigned int rom a641=0x3133;
unsigned int rom a642=0x3138;
unsigned int rom a643=0x3143;
unsigned int rom a644=0x3148;
unsigned int rom a645=0x3152;
unsigned int rom a646=0x3157;
unsigned int rom a647=0x3162;
unsigned int rom a648=0x3167;
unsigned int rom a649=0x3172;
unsigned int rom a650=0x3177;
unsigned int rom a651=0x3182;
unsigned int rom a652=0x3187;
unsigned int rom a653=0x3192;
unsigned int rom a654=0x3196;
unsigned int rom a655=0x3201;
unsigned int rom a656=0x3206;
unsigned int rom a657=0x3211;
unsigned int rom a658=0x3216;
unsigned int rom a659=0x3221;
unsigned int rom a660=0x3226;
unsigned int rom a661=0x3231;
unsigned int rom a662=0x3236;
unsigned int rom a663=0x3240;
unsigned int rom a664=0x3245;
unsigned int rom a665=0x3250;
unsigned int rom a666=0x3255;
unsigned int rom a667=0x3260;
unsigned int rom a668=0x3265;
unsigned int rom a669=0x3270;
unsigned int rom a670=0x3275;
unsigned int rom a671=0x3280;
unsigned int rom a672=0x3284;
unsigned int rom a673=0x3289;
unsigned int rom a674=0x3294;
unsigned int rom a675=0x3299;
unsigned int rom a676=0x3304;
unsigned int rom a677=0x3309;
unsigned int rom a678=0x3314;
unsigned int rom a679=0x3319;
unsigned int rom a680=0x3324;
unsigned int rom a681=0x3328;
unsigned int rom a682=0x3333;
unsigned int rom a683=0x3338;
unsigned int rom a684=0x3343;
unsigned int rom a685=0x3348;
unsigned int rom a686=0x3353;
unsigned int rom a687=0x3358;
unsigned int rom a688=0x3363;
unsigned int rom a689=0x3368;
unsigned int rom a690=0x3372;
unsigned int rom a691=0x3377;
unsigned int rom a692=0x3382;
unsigned int rom a693=0x3387;
unsigned int rom a694=0x3392;
unsigned int rom a695=0x3397;
unsigned int rom a696=0x3402;
unsigned int rom a697=0x3407;
unsigned int rom a698=0x3412;
unsigned int rom a699=0x3416;
unsigned int rom a700=0x3421;
unsigned int rom a701=0x3426;
unsigned int rom a702=0x3431;
unsigned int rom a703=0x3436;
unsigned int rom a704=0x3441;
unsigned int rom a705=0x3446;
unsigned int rom a706=0x3451;
unsigned int rom a707=0x3456;
unsigned int rom a708=0x3460;
unsigned int rom a709=0x3465;
unsigned int rom a710=0x3470;
unsigned int rom a711=0x3475;
unsigned int rom a712=0x3480;
unsigned int rom a713=0x3485;
unsigned int rom a714=0x3490;
unsigned int rom a715=0x3495;
unsigned int rom a716=0x3500;
unsigned int rom a717=0x3504;
unsigned int rom a718=0x3509;
unsigned int rom a719=0x3514;
unsigned int rom a720=0x3519;
unsigned int rom a721=0x3524;
unsigned int rom a722=0x3529;
unsigned int rom a723=0x3534;
unsigned int rom a724=0x3539;
unsigned int rom a725=0x3543;
unsigned int rom a726=0x3548;
unsigned int rom a727=0x3553;
unsigned int rom a728=0x3558;
unsigned int rom a729=0x3563;
unsigned int rom a730=0x3568;
unsigned int rom a731=0x3573;
unsigned int rom a732=0x3578;
unsigned int rom a733=0x3583;
unsigned int rom a734=0x3587;
unsigned int rom a735=0x3592;
unsigned int rom a736=0x3597;
unsigned int rom a737=0x3602;
unsigned int rom a738=0x3607;
unsigned int rom a739=0x3612;
unsigned int rom a740=0x3617;
unsigned int rom a741=0x3622;
unsigned int rom a742=0x3627;
unsigned int rom a743=0x3631;
unsigned int rom a744=0x3636;
unsigned int rom a745=0x3641;
unsigned int rom a746=0x3646;
unsigned int rom a747=0x3651;
unsigned int rom a748=0x3656;
unsigned int rom a749=0x3661;
unsigned int rom a750=0x3666;
unsigned int rom a751=0x3671;
unsigned int rom a752=0x3675;
unsigned int rom a753=0x3680;
unsigned int rom a754=0x3685;
unsigned int rom a755=0x3690;
unsigned int rom a756=0x3695;
unsigned int rom a757=0x3700;
unsigned int rom a758=0x3705;
unsigned int rom a759=0x3710;
unsigned int rom a760=0x3715;
unsigned int rom a761=0x3719;
unsigned int rom a762=0x3724;
unsigned int rom a763=0x3729;
unsigned int rom a764=0x3734;
unsigned int rom a765=0x3739;
unsigned int rom a766=0x3744;
unsigned int rom a767=0x3749;
unsigned int rom a768=0x3754;
unsigned int rom a769=0x3759;
unsigned int rom a770=0x3763;
unsigned int rom a771=0x3768;
unsigned int rom a772=0x3773;
unsigned int rom a773=0x3778;
unsigned int rom a774=0x3783;
unsigned int rom a775=0x3788;
unsigned int rom a776=0x3793;
unsigned int rom a777=0x3798;
unsigned int rom a778=0x3803;
unsigned int rom a779=0x3807;
unsigned int rom a780=0x3812;
unsigned int rom a781=0x3817;
unsigned int rom a782=0x3822;
unsigned int rom a783=0x3827;
unsigned int rom a784=0x3832;
unsigned int rom a785=0x3837;
unsigned int rom a786=0x3842;
unsigned int rom a787=0x3847;
unsigned int rom a788=0x3851;
unsigned int rom a789=0x3856;
unsigned int rom a790=0x3861;
unsigned int rom a791=0x3866;
unsigned int rom a792=0x3871;
unsigned int rom a793=0x3876;
unsigned int rom a794=0x3881;
unsigned int rom a795=0x3886;
unsigned int rom a796=0x3891;
unsigned int rom a797=0x3895;
unsigned int rom a798=0x3900;
unsigned int rom a799=0x3905;
unsigned int rom a800=0x3910;
unsigned int rom a801=0x3915;
unsigned int rom a802=0x3920;
unsigned int rom a803=0x3925;
unsigned int rom a804=0x3930;
unsigned int rom a805=0x3935;
unsigned int rom a806=0x3939;
unsigned int rom a807=0x3944;
unsigned int rom a808=0x3949;
unsigned int rom a809=0x3954;
unsigned int rom a810=0x3959;
unsigned int rom a811=0x3964;
unsigned int rom a812=0x3969;
unsigned int rom a813=0x3974;
unsigned int rom a814=0x3978;
unsigned int rom a815=0x3983;
unsigned int rom a816=0x3988;
unsigned int rom a817=0x3993;
unsigned int rom a818=0x3998;
unsigned int rom a819=0x4003;
unsigned int rom a820=0x4008;
unsigned int rom a821=0x4013;
unsigned int rom a822=0x4018;
unsigned int rom a823=0x4022;
unsigned int rom a824=0x4027;
unsigned int rom a825=0x4032;
unsigned int rom a826=0x4037;
unsigned int rom a827=0x4042;
unsigned int rom a828=0x4047;
unsigned int rom a829=0x4052;
unsigned int rom a830=0x4057;
unsigned int rom a831=0x4062;
unsigned int rom a832=0x4066;
unsigned int rom a833=0x4071;
unsigned int rom a834=0x4076;
unsigned int rom a835=0x4081;
unsigned int rom a836=0x4086;
unsigned int rom a837=0x4091;
unsigned int rom a838=0x4096;
unsigned int rom a839=0x4101;
unsigned int rom a840=0x4106;
unsigned int rom a841=0x4110;
unsigned int rom a842=0x4115;
unsigned int rom a843=0x4120;
unsigned int rom a844=0x4125;
unsigned int rom a845=0x4130;
unsigned int rom a846=0x4135;
unsigned int rom a847=0x4140;
unsigned int rom a848=0x4145;
unsigned int rom a849=0x4150;
unsigned int rom a850=0x4154;
unsigned int rom a851=0x4159;
unsigned int rom a852=0x4164;
unsigned int rom a853=0x4169;
unsigned int rom a854=0x4174;
unsigned int rom a855=0x4179;
unsigned int rom a856=0x4184;
unsigned int rom a857=0x4189;
unsigned int rom a858=0x4194;
unsigned int rom a859=0x4198;
unsigned int rom a860=0x4203;
unsigned int rom a861=0x4208;
unsigned int rom a862=0x4213;
unsigned int rom a863=0x4218;
unsigned int rom a864=0x4223;
unsigned int rom a865=0x4228;
unsigned int rom a866=0x4233;
unsigned int rom a867=0x4238;
unsigned int rom a868=0x4242;
unsigned int rom a869=0x4247;
unsigned int rom a870=0x4252;
unsigned int rom a871=0x4257;
unsigned int rom a872=0x4262;
unsigned int rom a873=0x4267;
unsigned int rom a874=0x4272;
unsigned int rom a875=0x4277;
unsigned int rom a876=0x4282;
unsigned int rom a877=0x4286;
unsigned int rom a878=0x4291;
unsigned int rom a879=0x4296;
unsigned int rom a880=0x4301;
unsigned int rom a881=0x4306;
unsigned int rom a882=0x4311;
unsigned int rom a883=0x4316;
unsigned int rom a884=0x4321;
unsigned int rom a885=0x4326;
unsigned int rom a886=0x4330;
unsigned int rom a887=0x4335;
unsigned int rom a888=0x4340;
unsigned int rom a889=0x4345;
unsigned int rom a890=0x4350;
unsigned int rom a891=0x4355;
unsigned int rom a892=0x4360;
unsigned int rom a893=0x4365;
unsigned int rom a894=0x4370;
unsigned int rom a895=0x4374;
unsigned int rom a896=0x4379;
unsigned int rom a897=0x4384;
unsigned int rom a898=0x4389;
unsigned int rom a899=0x4394;
unsigned int rom a900=0x4399;
unsigned int rom a901=0x4404;
unsigned int rom a902=0x4409;
unsigned int rom a903=0x4413;
unsigned int rom a904=0x4418;
unsigned int rom a905=0x4423;
unsigned int rom a906=0x4428;
unsigned int rom a907=0x4433;
unsigned int rom a908=0x4438;
unsigned int rom a909=0x4443;
unsigned int rom a910=0x4448;
unsigned int rom a911=0x4453;
unsigned int rom a912=0x4457;
unsigned int rom a913=0x4462;
unsigned int rom a914=0x4467;
unsigned int rom a915=0x4472;
unsigned int rom a916=0x4477;
unsigned int rom a917=0x4482;
unsigned int rom a918=0x4487;
unsigned int rom a919=0x4492;
unsigned int rom a920=0x4497;
unsigned int rom a921=0x4501;
unsigned int rom a922=0x4506;
unsigned int rom a923=0x4511;
unsigned int rom a924=0x4516;
unsigned int rom a925=0x4521;
unsigned int rom a926=0x4526;
unsigned int rom a927=0x4531;
unsigned int rom a928=0x4536;
unsigned int rom a929=0x4541;
unsigned int rom a930=0x4545;
unsigned int rom a931=0x4550;
unsigned int rom a932=0x4555;
unsigned int rom a933=0x4560;
unsigned int rom a934=0x4565;
unsigned int rom a935=0x4570;
unsigned int rom a936=0x4575;
unsigned int rom a937=0x4580;
unsigned int rom a938=0x4585;
unsigned int rom a939=0x4589;
unsigned int rom a940=0x4594;
unsigned int rom a941=0x4599;
unsigned int rom a942=0x4604;
unsigned int rom a943=0x4609;
unsigned int rom a944=0x4614;
unsigned int rom a945=0x4619;
unsigned int rom a946=0x4624;
unsigned int rom a947=0x4629;
unsigned int rom a948=0x4633;
unsigned int rom a949=0x4638;
unsigned int rom a950=0x4643;
unsigned int rom a951=0x4648;
unsigned int rom a952=0x4653;
unsigned int rom a953=0x4658;
unsigned int rom a954=0x4663;
unsigned int rom a955=0x4668;
unsigned int rom a956=0x4673;
unsigned int rom a957=0x4677;
unsigned int rom a958=0x4682;
unsigned int rom a959=0x4687;
unsigned int rom a960=0x4692;
unsigned int rom a961=0x4697;
unsigned int rom a962=0x4702;
unsigned int rom a963=0x4707;
unsigned int rom a964=0x4712;
unsigned int rom a965=0x4717;
unsigned int rom a966=0x4721;
unsigned int rom a967=0x4726;
unsigned int rom a968=0x4731;
unsigned int rom a969=0x4736;
unsigned int rom a970=0x4741;
unsigned int rom a971=0x4746;
unsigned int rom a972=0x4751;
unsigned int rom a973=0x4756;
unsigned int rom a974=0x4761;
unsigned int rom a975=0x4765;
unsigned int rom a976=0x4770;
unsigned int rom a977=0x4775;
unsigned int rom a978=0x4780;
unsigned int rom a979=0x4785;
unsigned int rom a980=0x4790;
unsigned int rom a981=0x4795;
unsigned int rom a982=0x4800;
unsigned int rom a983=0x4804;
unsigned int rom a984=0x4809;
unsigned int rom a985=0x4814;
unsigned int rom a986=0x4819;
unsigned int rom a987=0x4824;
unsigned int rom a988=0x4829;
unsigned int rom a989=0x4834;
unsigned int rom a990=0x4839;
unsigned int rom a991=0x4844;
unsigned int rom a992=0x4848;
unsigned int rom a993=0x4853;
unsigned int rom a994=0x4858;
unsigned int rom a995=0x4863;
unsigned int rom a996=0x4868;
unsigned int rom a997=0x4873;
unsigned int rom a998=0x4878;
unsigned int rom a999=0x4883;
unsigned int rom a1000=0x4888;
unsigned int rom a1001=0x4892;
unsigned int rom a1002=0x4897;
unsigned int rom a1003=0x4902;
unsigned int rom a1004=0x4907;
unsigned int rom a1005=0x4912;
unsigned int rom a1006=0x4917;
unsigned int rom a1007=0x4922;
unsigned int rom a1008=0x4927;
unsigned int rom a1009=0x4932;
unsigned int rom a1010=0x4936;
unsigned int rom a1011=0x4941;
unsigned int rom a1012=0x4946;
unsigned int rom a1013=0x4951;
unsigned int rom a1014=0x4956;
unsigned int rom a1015=0x4961;
unsigned int rom a1016=0x4966;
unsigned int rom a1017=0x4971;
unsigned int rom a1018=0x4976;
unsigned int rom a1019=0x4980;
unsigned int rom a1020=0x4985;
unsigned int rom a1021=0x4990;
unsigned int rom a1022=0x4995;
unsigned int rom a1023=0x5000;