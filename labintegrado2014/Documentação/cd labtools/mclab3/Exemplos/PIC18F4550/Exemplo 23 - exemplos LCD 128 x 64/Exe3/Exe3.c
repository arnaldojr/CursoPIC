/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                          Exemplo 23 (Exe 3)                             *
 *                                                                         *
 *                               MOSAICO                                   *
 *                                                                         *
 *  TEL: (0XX11) 4992-8775               SITE: www.mosaico.com.br          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   VERSÃO : 1.0                                                          *
 *   DATA : 10/05/2011                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             Descrição geral                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
	Este software demonstra como realizar a comunicação entre o controlador 
	touch-screen e o microcontrolador. A cada toque do display a informação 
	correspondente as posições X e Y são exibidas na tela.
*/

// Observação: ligar a chave SCK(RB1), SDI(RC4), SCK(RC7) e CS-TOUCH

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                DEFINIÇÃO DAS VARIÁVEIS INTERNAS DO PIC                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <p18F4550.h>        	 // Register definitions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <delays.h>        		 //Delay library functions

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                         Configurações para gravação                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma config FOSC 	= HS
#pragma config CPUDIV 	= OSC4_PLL6
#pragma config WDT 		= OFF
#pragma config WDTPS 	= 128
#pragma config LVP 		= OFF
#pragma config PWRT	    = ON
#pragma config BOR 		= ON
#pragma config BORV 	= 2
#pragma config PBADEN 	= OFF
#pragma config DEBUG 	= OFF
#pragma config PLLDIV 	= 1
#pragma config USBDIV 	= 1
#pragma config FCMEN 	= OFF
#pragma config IESO 	= OFF
#pragma config VREGEN 	= OFF
#pragma config MCLRE 	= ON
#pragma config LPT1OSC 	= OFF
#pragma config CCP2MX 	= OFF
#pragma config STVREN 	= OFF
#pragma config ICPRT 	= OFF
#pragma config XINST 	= OFF
#pragma config CP0 		= OFF
#pragma config CP1 		= OFF
#pragma config CP2 		= OFF
#pragma config CP3 		= OFF
#pragma config CPB 		= OFF
#pragma config CPD 		= OFF
#pragma config WRT0 	= OFF
#pragma config WRT1 	= OFF
#pragma config WRT2 	= OFF
#pragma config WRT3 	= OFF
#pragma config WRTB 	= OFF
#pragma config WRTC 	= OFF
#pragma config WRTD 	= OFF
#pragma config EBTR0 	= OFF
#pragma config EBTR1 	= OFF
#pragma config EBTR2 	= OFF
#pragma config EBTR3 	= OFF
#pragma config EBTRB 	= OFF

//
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                INCLUDES DAS FUNÇÕES DE PERIFÉRICOS DO PIC             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                       PROTOTIPAGEM DE FUNÇÕES                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void glcd_init(unsigned char mode);
void glcd_pixel(unsigned char x, unsigned char y, unsigned char color);
void glcd_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color);
void glcd_rect(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char fill, unsigned char color);
void glcd_bar(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char width, unsigned char color);
void glcd_circle(unsigned char x, unsigned char y, unsigned char radius, unsigned char fill, unsigned char color);
void glcd_text57(unsigned char x, unsigned char y, unsigned char *textptr, unsigned char size, unsigned char color);
void glcd_fillScreen(unsigned char color);
void glcd_writeByte(unsigned char chip, unsigned char data);
unsigned char glcd_readByte(unsigned char chip);

unsigned char leitura_touch(unsigned char eixo);
void filtro_touch(unsigned char canal);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.

#define GLCD_WIDTH 128    // Used for text wrapping by glcd_text57 function

#define X  1
#define Y  0

#define ON  1
#define OFF 0

#define YES 1
#define NO  0

#define	TRUE   1
#define FALSE  0

#define SEL_GLCD_CS1 0
#define SEL_GLCD_CS2 1

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                 Definição e inicialização das variáveis                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//Neste bloco estão definidas as variáveis globais do programa.

unsigned char buffer_latch_controle,aux = 0;
unsigned int  temp_coluna_touch,temp_linha_touch,aux1;
unsigned char texto[4];

unsigned char texto1[] = "TOQUE NA TELA";
unsigned char texto2[] = "X: ";
unsigned char texto3[] = "Y: ";

unsigned char filtro_linha_touch_press;
unsigned char filtro_coluna_touch_press;
unsigned char filtro_linha_touch_lib;
unsigned char filtro_coluna_touch_lib;
unsigned char linha_touch_ok;
unsigned char coluna_touch_ok;
unsigned char pos_touch_ok;

unsigned char linha_touch_anterior;
unsigned char coluna_touch_anterior;
unsigned char linha_touch;
unsigned char coluna_touch;

unsigned char t_f_touch_press = 10;			// FILTRO DE PRESS. DE DEBOUNCE DO TOUCH (MULTIPLOS DE 1ms)
unsigned char t_f_touch_lib   =  2;			// FILTRO DE LIBER. DE DEBOUNCE DO TOUCH (MULTIPLOS DE 1ms)

#pragma romdata tabela = 0x100
const rom unsigned char TEXT[96][5] = {		
									 0x00, 0x00, 0x00, 0x00, 0x00, // SPACE    32
					                 0x00, 0x00, 0x5F, 0x00, 0x00, // !	33
					                 0x00, 0x03, 0x00, 0x03, 0x00, // "	34
					                 0x14, 0x3E, 0x14, 0x3E, 0x14, // #	35
					                 0x24, 0x2A, 0x7F, 0x2A, 0x12, // $	36
					                 0x43, 0x33, 0x08, 0x66, 0x61, // %	37
					                 0x36, 0x49, 0x55, 0x22, 0x50, // &	38
					                 0x00, 0x05, 0x03, 0x00, 0x00, // '	39	
					                 0x00, 0x1C, 0x22, 0x41, 0x00, // (	40
					                 0x00, 0x41, 0x22, 0x1C, 0x00, // )	41
					                 0x14, 0x08, 0x3E, 0x08, 0x14, // *	42
					                 0x08, 0x08, 0x3E, 0x08, 0x08, // +	43
					                 0x00, 0x50, 0x30, 0x00, 0x00, // ,	44
					                 0x08, 0x08, 0x08, 0x08, 0x08, // -	45
					                 0x00, 0x60, 0x60, 0x00, 0x00, // .	46
					                 0x20, 0x10, 0x08, 0x04, 0x02, // /	47
									 0x3E, 0x51, 0x49, 0x45, 0x3E, // 0	48
			                         0x04, 0x02, 0x7F, 0x00, 0x00, // 1	49
			                         0x42, 0x61, 0x51, 0x49, 0x46, // 2	50
			                         0x22, 0x41, 0x49, 0x49, 0x36, // 3	51
			                         0x18, 0x14, 0x12, 0x7F, 0x10, // 4	521
			                         0x27, 0x45, 0x45, 0x45, 0x39, // 5	53
			                         0x3E, 0x49, 0x49, 0x49, 0x32, // 6	54
			                         0x01, 0x01, 0x71, 0x09, 0x07, // 7	55
			                         0x36, 0x49, 0x49, 0x49, 0x36, // 8	56
			                         0x26, 0x49, 0x49, 0x49, 0x3E, // 9	57
			                         0x00, 0x36, 0x36, 0x00, 0x00, // :	58
			                         0x00, 0x56, 0x36, 0x00, 0x00, // ;	59
			                         0x08, 0x14, 0x22, 0x41, 0x00, // <	60
			                         0x14, 0x14, 0x14, 0x14, 0x14, // =	61
			                         0x00, 0x41, 0x22, 0x14, 0x08, // >	62
			                         0x02, 0x01, 0x51, 0x09, 0x06, // ?	63
			                         0x3E, 0x41, 0x59, 0x55, 0x5E, // @	64
			                         0x7E, 0x09, 0x09, 0x09, 0x7E, // A	65	
			                         0x7F, 0x49, 0x49, 0x49, 0x36, // B	66
			                         0x3E, 0x41, 0x41, 0x41, 0x22, // C	67
			                         0x7F, 0x41, 0x41, 0x41, 0x3E, // D	68
			                         0x7F, 0x49, 0x49, 0x49, 0x41, // E	69
			                         0x7F, 0x09, 0x09, 0x09, 0x01, // F	70
			                         0x3E, 0x41, 0x41, 0x49, 0x3A, // G	71
			                         0x7F, 0x08, 0x08, 0x08, 0x7F, // H	72
			                         0x00, 0x41, 0x7F, 0x41, 0x00, // I	73
			                         0x30, 0x40, 0x40, 0x40, 0x3F, // J	74
			                         0x7F, 0x08, 0x14, 0x22, 0x41, // K	75
			                         0x7F, 0x40, 0x40, 0x40, 0x40, // L	76
			                         0x7F, 0x02, 0x0C, 0x02, 0x7F, // M	77
			                         0x7F, 0x02, 0x04, 0x08, 0x7F, // N	78
			                         0x3E, 0x41, 0x41, 0x41, 0x3E, // O	79
			                         0x7F, 0x09, 0x09, 0x09, 0x06, // P	80
			                         0x1E, 0x21, 0x21, 0x21, 0x5E, // Q	81
			                         0x7F, 0x09, 0x09, 0x09, 0x76, // R	82
									 0x26, 0x49, 0x49, 0x49, 0x32, // S	83
			                         0x01, 0x01, 0x7F, 0x01, 0x01, // T	84
			                         0x3F, 0x40, 0x40, 0x40, 0x3F, // U	85
			                         0x1F, 0x20, 0x40, 0x20, 0x1F, // V	86
			                         0x7F, 0x20, 0x10, 0x20, 0x7F, // W	87
			                         0x41, 0x22, 0x1C, 0x22, 0x41, // X	88
			                         0x07, 0x08, 0x70, 0x08, 0x07, // Y	89
			                         0x61, 0x51, 0x49, 0x45, 0x43, // Z	90
			                         0x00, 0x7F, 0x41, 0x00, 0x00, // [	91
			                         0x02, 0x04, 0x08, 0x10, 0x20, // \	92
			                         0x00, 0x00, 0x41, 0x7F, 0x00, // ]	93
			                         0x04, 0x02, 0x01, 0x02, 0x04, // ^	94
			                         0x40, 0x40, 0x40, 0x40, 0x40, // _	95
			                         0x00, 0x01, 0x02, 0x04, 0x00, // `	96
			                	     0x20, 0x54, 0x54, 0x54, 0x78, // a	98
			                         0x7F, 0x44, 0x44, 0x44, 0x38, // b	99
			                         0x38, 0x44, 0x44, 0x44, 0x44, // c	100
			                         0x38, 0x44, 0x44, 0x44, 0x7F, // d	101
			                         0x38, 0x54, 0x54, 0x54, 0x18, // e	102
			                         0x04, 0x04, 0x7E, 0x05, 0x05, // f	103
			                         0x08, 0x54, 0x54, 0x54, 0x3C, // g	104
			                         0x7F, 0x08, 0x04, 0x04, 0x78, // h	105
			                         0x00, 0x44, 0x7D, 0x40, 0x00, // i	106
			                         0x20, 0x40, 0x44, 0x3D, 0x00, // j	107
			                         0x7F, 0x10, 0x28, 0x44, 0x00, // k	108
			                         0x00, 0x41, 0x7F, 0x40, 0x00, // l	109
			                         0x7C, 0x04, 0x78, 0x04, 0x78, // m	110
			                         0x7C, 0x08, 0x04, 0x04, 0x78, // n	111
			                         0x38, 0x44, 0x44, 0x44, 0x38, // o	112
			                         0x7C, 0x14, 0x14, 0x14, 0x08, // p	113
			                         0x08, 0x14, 0x14, 0x14, 0x7C, // q	114
			                         0x00, 0x7C, 0x08, 0x04, 0x04, // r	115
			                         0x48, 0x54, 0x54, 0x54, 0x20, // s	116
			                         0x04, 0x04, 0x3F, 0x44, 0x44, // t	117
			                         0x3C, 0x40, 0x40, 0x20, 0x7C, // u	118
			                         0x1C, 0x20, 0x40, 0x20, 0x1C, // v	119
			                         0x3C, 0x40, 0x30, 0x40, 0x3C, // w	120
			                         0x44, 0x28, 0x10, 0x28, 0x44, // x	121
			                         0x0C, 0x50, 0x50, 0x50, 0x3C, // y	122
			                         0x44, 0x64, 0x54, 0x4C, 0x44, // z	123
			                         0x00, 0x08, 0x36, 0x41, 0x41, // {	124
			                         0x00, 0x00, 0x7F, 0x00, 0x00, // |	125
			                         0x41, 0x41, 0x36, 0x08, 0x00, // }	126
			                         0x02, 0x01, 0x02, 0x04, 0x02  // ~ 127
									};
#pragma romdata

//	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    Declaração dos flags de software                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de flags ajuda na programação e economiza memória RAM.

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               ENTRADAS                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// As entradas devem ser associadas a nomes para facilitar a programação e
//futuras alterações do hardware.

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                 SAÍDAS                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// AS SAÍDAS DEVEM SER ASSOCIADAS A NOMES PARA FACILITAR A PROGRAMAÇÃO E
//FUTURAS ALTERAÇÕES DO HARDWARE.

#define CS1	 		LATBbits.LATB0		// CS1 DO LCD
#define CS2	 		LATBbits.LATB1		// CS2 DO LCD
#define RST	 		LATBbits.LATB6		// RESET DO LCD
#define RW	 		LATBbits.LATB7		// READ/WRITE DO LCD
#define RS	 		LATEbits.LATE0		// RS DO LCD
#define E	 		LATEbits.LATE1		// ENABLE DO LCD
#define BARRAMENTO	PORTD				// BARRAMENTO DO LCD
#define BARRAMENTO_TRIS	TRISD			// BARRAMENTO DO LCD

#define SDI	 		PORTBbits.RB0		// PINO SDI DO CONTROLADOR TOUCH
#define SDI_TRIS    TRISBbits.TRISB0	// PINO SDI DO CONTROLADOR TOUCH

#define SDO	 		PORTCbits.RC7		// PINO SDO DO CONTROLADOR TOUCH
#define SCK	 		PORTBbits.RB1		// PINO SCK DO CONTROLADOR TOUCH
#define CS_T 		PORTAbits.RA2		// PINO CHIP SELECT DO CONTROLADOR TOUCH

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                          Configurações do Pic                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void main()
{
	RCON = 0x7F;					// Reset control
									// IPEN=0 modo compativel com
    								// 16F, toda int. no end. 0x0008

	PORTA = 0x00;                       //Limpa PORTA
	PORTB = 0x00;                       //Limpa PORTB
	PORTC = 0x00;		         	    //Limpa PORTC
	PORTD = 0x00;                       //Limpa PORTD
	PORTE = 0x00;                       //Limpa PORTE

	LATA = 0x00;                      	//Limpa PORTA
	LATB = 0x00;                        //Limpa PORTB
	LATC = 0x00;		     	        //Limpa PORTC
	LATD = 0x00;                        //Limpa PORTD
	LATE = 0x00;                        //Limpa PORTE

			//76543210
	TRISA = 0b11111011;                 //CONFIG DIREÇÃO DOS PINOS PORTA
	TRISB = 0b00111100;                 //CONFIG DIREÇÃO DOS PINOS PORTB
	TRISC = 0b01111111;		            //CONFIG DIREÇÃO DOS PINOS PORTC
	TRISD = 0b00000000;                 //CONFIG DIREÇÃO DOS PINOS PORTD
	TRISE = 0b00001100;                 //CONFIG DIREÇÃO DOS PINOS PORTE

	ADCON1 = 0b00001111;                //DESLIGA CONVERSORES A/D
	CMCON  = 0x07;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                        Inicialização do Sistema                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	Delay1KTCYx(200);		// Tempo de acomodação do LCD

	glcd_init(ON);			// Inicializa LCD

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              Rotina principal                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	glcd_text57(20,10,texto1,1,1);		// Escreve tela com texto e nome
	glcd_text57(0,40,texto2,1,1);		// das posições
	glcd_text57(85,40,texto3,1,1);	

	pos_touch_ok=0;
	
	while(1)
	{
				coluna_touch = leitura_touch(X);	// Obtem posição X
				filtro_touch(X);					// Filtragem da amostra

				linha_touch = leitura_touch(Y);		// Obtem posição Y
				filtro_touch(Y);					// Filtragem da amostra
		
				if( pos_touch_ok )		// É uma posição válida (validada pelo filtro) ?
				{
					
					if ( linha_touch != temp_linha_touch ) 
					{
						itoa((unsigned char)temp_linha_touch,(char*)texto);	// Converte número em string
						glcd_text57(100,40,texto,1,0);					// Apaga posição anterior
	
						temp_linha_touch  = linha_touch;				// Atualiza buffer
	
						itoa((unsigned char)linha_touch,(char*)texto);			// Converte número em string
						glcd_text57(100,40,texto,1,1);					// Plota nova posição
					}

					if ( coluna_touch != temp_coluna_touch )
					{
						itoa((unsigned char)temp_coluna_touch,(char*)texto);	// Converte número em string
						glcd_text57(15,40,texto,1,0);					// Apaga posição anterior
		
						temp_coluna_touch = coluna_touch;				// Atualiza buffer
	
						itoa((unsigned char)coluna_touch,(char*)texto);		// Converte número em string
						glcd_text57(15,40,texto,1,1);					// Plota nova posição
					}
				
					pos_touch_ok = 0;				// Reinicia flag posição válida
				}
				
    }

}

/*
**===========================================================================
**===========================================================================
**
**          FUNÇÕES DE CONTROLE DO LCD 128 X 64
**
**===========================================================================
**===========================================================================
*/

// Purpose:       Initialize a graphic LCD. This must be called before any
//                other glcd function is used.
// Inputs:        The initialization mode
//                OFF - Turns the LCD off
//                ON  - Turns the LCD on
// Date:          5/28/2003
void glcd_init(unsigned char mode)
{
    SDI_TRIS = 0;   //configura o pino como entrada
    Nop();
    Nop();
    
	RST = 1;
	E = 0;
	CS1 = 0;
	CS2 = 0;
	RS = 0;              // Set for instruction

	glcd_writeByte(SEL_GLCD_CS1, 0xC0);     // Specify first RAM line at the top
   	glcd_writeByte(SEL_GLCD_CS2, 0xC0);     // of the screen
   	glcd_writeByte(SEL_GLCD_CS1, 0x40);     // Set the column address to 0
   	glcd_writeByte(SEL_GLCD_CS2, 0x40);
   	glcd_writeByte(SEL_GLCD_CS1, 0xB8);     // Set the page address to 0
   	glcd_writeByte(SEL_GLCD_CS2, 0xB8);

	if(mode == ON)
   	{
    	glcd_writeByte(SEL_GLCD_CS1, 0x3F);	// Turn the display on
      	glcd_writeByte(SEL_GLCD_CS2, 0x3F);
   	}
   	else
   	{
    	glcd_writeByte(SEL_GLCD_CS1, 0x3E);	// Turn the display off
      	glcd_writeByte(SEL_GLCD_CS2, 0x3E);
   	}

	glcd_fillScreen(OFF);               	// Clear the display

    SDI_TRIS = 1;   //configura o pino como entrada
}
// Purpose:       Turn a pixel on a graphic LCD on or off
// Inputs:        x - the x coordinate of the pixel
//                y - the y coordinate of the pixel
//                color - ON or OFF
// Output:        1 if coordinate out of range, 0 if in range
void glcd_pixel(unsigned char x, unsigned char y, unsigned char color)
{
	unsigned char data;
	unsigned char tmp;
   	unsigned char chip = SEL_GLCD_CS1;	// Stores which chip to use on the LCD
   	
   	if(x > 63)  						// Check for first or second display area
   	{
		x -= 64;
      	chip = SEL_GLCD_CS2;
   	}

	// Set for instruction
	RS = 0;

	x = x & 0b01111111;										// Clear the MSB. Part of an instruction code
	x = x | 0b01000000;										// Set bit 6. Also part of an instruction code
   	glcd_writeByte(chip, x);								// Set the horizontal address
   	glcd_writeByte(chip, (y/8 & 0b10111111) | 0b10111000);	// Set the vertical page address

	// Set for data
	RS = 1;

	data = glcd_readByte(chip);

	if(color == ON)
	{
		tmp = 0b00000001;									// Turn the pixel on
		tmp = tmp << (y%8);
		data = data | tmp;
	}
	else
	{														// or
		tmp = 0b00000001;									// turn the pixel off
		tmp = ~(tmp << (y%8));
		data = data & tmp;
    }

	// Set for instruction
	RS = 0;
   	glcd_writeByte(chip, x);								// Set the horizontal address

	// Set for data
	RS = 1;
   	glcd_writeByte(chip, data);								// Write the pixel data
}


// Purpose:       Draw a line on a graphic LCD using Bresenham's
//                line drawing algorithm
// Inputs:        (x1, y1) - the start coordinate
//                (x2, y2) - the end coordinate
//                color - LCD_ON or LCD_OFF
// Dependencies:  glcd_pixel()
void glcd_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color)
{
	signed char  x, y, addx, addy, dx, dy;
   	signed long P;
   	unsigned char i;

   	if( x2 > x1 )
   		dx = x2 - x1;
   	else
   		dx = x1 - x2;

   	if( y2 > y1 )
   		dy = y2 - y1;
   	else
   		dy = y1 - y2;

   	x = x1;
   	y = y1;

	if(x1 > x2)
		addx = -1;
   	else
   		addx = 1;

   	if(y1 > y2)
   		addy = -1;
   	else
   		addy = 1;

   	if(dx >= dy)
   	{
		P = 2*dy - dx;

      	for(i=0; i<=dx; ++i)
      	{
			glcd_pixel(x, y, color);

			if(P < 0)
         	{
            	P += 2*dy;
            	x += addx;
			}
         	else
			{
            	P += 2*dy - 2*dx;
            	x += addx;
            	y += addy;
         	}
		}
	}
   	else
   	{
		P = 2*dx - dy;

      	for(i=0; i<=dy; ++i)
      	{
			glcd_pixel(x, y, color);

			if(P < 0)
			{
            	P += 2*dx;
            	y += addy;
			}
         	else
         	{
	            P += 2*dx - 2*dy;
	            x += addx;
	            y += addy;
         	}
      	}
   	}
}
// Purpose:       Draw a rectangle on a graphic LCD
// Inputs:        (x1, y1) - the start coordinate
//                (x2, y2) - the end coordinate
//                fill  - YES or NO
//                color - ON or OFF
// Dependencies:  glcd_pixel(), glcd_line()
void glcd_rect(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char fill, unsigned char color)
{
	if(fill)
   	{
		unsigned char y, ymax;								// Find the y min and max
      	if(y1 < y2)
      	{
			y = y1;
         	ymax = y2;
		}
      	else
      	{
			y = y2;
         	ymax = y1;
		}

		for(; y<=ymax; ++y) glcd_line(x1, y, x2, y, color);	// Draw lines to fill the rectangle

	}
   	else
   	{
		glcd_line(x1, y1, x2, y1, color);      				// Draw the 4 sides
		glcd_line(x1, y2, x2, y2, color);
		glcd_line(x1, y1, x1, y2, color);
		glcd_line(x2, y1, x2, y2, color);
   	}
}


// Purpose:       Draw a bar (wide line) on a graphic LCD
// Inputs:        (x1, y1) - the start coordinate
//                (x2, y2) - the end coordinate
//                width  - The number of pixels wide
//                color - ON or OFF
void glcd_bar(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char width, unsigned char color)
{
	signed char  x, y, addx, addy, j;
   	signed long P, dx, dy, c1, c2;
   	unsigned char i;

   	dx = fabs((signed char)(x2 - x1));
   	dy = fabs((signed char)(y2 - y1));
   	x = x1;
   	y = y1;

   	c1 = -dx*x1 - dy*y1;
   	c2 = -dx*x2 - dy*y2;

   	if(x1 > x2)
   	{
		addx = -1;
      	c1 = -dx*x2 - dy*y2;
      	c2 = -dx*x1 - dy*y1;
   	}
   	else addx = 1;
	if(y1 > y2)
   	{
		addy = -1;
      	c1 = -dx*x2 - dy*y2;
      	c2 = -dx*x1 - dy*y1;
   	}
	else addy = 1;

	if(dx >= dy)
   	{
		P = 2*dy - dx;

      	for(i=0; i<=dx; ++i)
      	{
			for(j=-(width/2); j<width/2+width%2; ++j)
         	{
            	if(dx*x+dy*(y+j)+c1 >= 0 && dx*x+dy*(y+j)+c2 <=0)
				glcd_pixel(x, y+j, color);
         	}
         	if(P < 0)
         	{
            	P += 2*dy;
            	x += addx;
         	}
         	else
         	{
            	P += 2*dy - 2*dx;
            	x += addx;
            	y += addy;
         	}
		}
   	}
   	else
   	{
		P = 2*dx - dy;

      	for(i=0; i<=dy; ++i)
      	{
			if(P < 0)
         	{
            	P += 2*dx;
            	y += addy;
         	}
         	else
         	{
            	P += 2*dx - 2*dy;
            	x += addx;
            	y += addy;
         	}
         	for(j=-(width/2); j<width/2+width%2; ++j)
         	{
            	if(dx*x+dy*(y+j)+c1 >= 0 && dx*x+dy*(y+j)+c2 <=0)
               	glcd_pixel(x+j, y, color);
         	}
      	}
   	}
}

// Purpose:       Draw a circle on a graphic LCD
// Inputs:        (x,y) - the center of the circle
//                radius - the radius of the circle
//                fill - YES or NO
//                color - ON or OFF
void glcd_circle(unsigned char x, unsigned char y, unsigned char radius, unsigned char fill, unsigned char color)
{
	signed char a, b, P;
   	
   	a = 0;
   	b = radius;
   	P = 1 - radius;

   	do
   	{
		if(fill)
      	{
			glcd_line(x-a, y+b, x+a, y+b, color);
         	glcd_line(x-a, y-b, x+a, y-b, color);
         	glcd_line(x-b, y+a, x+b, y+a, color);
         	glcd_line(x-b, y-a, x+b, y-a, color);
      	}
      	else
      	{
			glcd_pixel(a+x, b+y, color);
         	glcd_pixel(b+x, a+y, color);
         	glcd_pixel(x-a, b+y, color);
         	glcd_pixel(x-b, a+y, color);
         	glcd_pixel(b+x, y-a, color);
         	glcd_pixel(a+x, y-b, color);
         	glcd_pixel(x-a, y-b, color);
         	glcd_pixel(x-b, y-a, color);
      	}

      	if(P < 0) P+= 3 + 2*a++;
		else P+= 5 + 2*(a++ - b--);
		
	}while(a <= b);
}



// Purpose:       Write text on a graphic LCD
// Inputs:        (x,y) - The upper left coordinate of the first letter
//                textptr - A pointer to an array of text to display
//                size - The size of the text: 1 = 5x7, 2 = 10x14, ...
//                color - ON or OFF
void glcd_text57(unsigned char x, unsigned char y, unsigned char *textptr, unsigned char size, unsigned char color)
{
	unsigned char i, j, k, l, m;					// Loop counters
   	unsigned char pixelData[5];                     // Stores character data
   	unsigned char tmp;

   	for(i=0; textptr[i] != '\0'; i++, x++)			// Loop through the passed string
   	{
	   	
		if(textptr[i] <= '~')						// Checks if the letter is in the text array
		{
			memcpypgm2ram((char*)pixelData, TEXT[textptr[i]-' '], 5);
		}
      	else										// Default to space
      	{
			memcpypgm2ram((char*)pixelData, TEXT[0], 5);
		}
			
      	if(x+5*size >= GLCD_WIDTH)					// Performs character wrapping
      	{
			x = 0;									// Set x at far left position
			y += 7*size + 1;						// Set y at next position down
		}
		
      	for(j=0; j<5; j++, x+=size)					// Loop through character byte data
		{
			for(k=0; k<7*size; k++)					// Loop through the vertical pixels
         	{
				if((pixelData[j] >> k) & 0b00000001)// Check if the pixel should be set				
            	{
               		for(l=0; l<size; l++)      		// The next two loops change the
               		{                          		// character's size
                  		for(m=0; m<size; m++)
                  		{
                     		glcd_pixel(x+m, y+k*size+l, color);// Draws the pixel
                  		}
	           		}
            	}
			}
      	}
   	}
}


// Purpose:       Fill the LCD screen with the passed in color.
//                Works much faster than drawing a rectangle to fill the screen.
// Inputs:        ON - turn all the pixels on
//                OFF - turn all the pixels off
// Dependencies:  glcd_writeByte()
void glcd_fillScreen(unsigned char color)
{
	unsigned char i, j;

   	// Loop through the vertical pages
   	for(i = 0; i<8; i++)
   	{

		// Set for instruction
		RS = 0;

      	glcd_writeByte(SEL_GLCD_CS2, 0b01000000);		// Set horizontal address to 0
		glcd_writeByte(SEL_GLCD_CS1, 0b01000000);
		glcd_writeByte(SEL_GLCD_CS2, i | 0b10111000);	// Set page address
		glcd_writeByte(SEL_GLCD_CS1, i | 0b10111000);

		// Set for data
		RS = 1;

		// Loop through the horizontal sections
      	for(j = 0; j < 64; j++)
      	{
			glcd_writeByte(SEL_GLCD_CS1, 0xFF*color);	// Turn pixels on or off
			glcd_writeByte(SEL_GLCD_CS2, 0xFF*color);	// Turn pixels on or off
      	}

   	}
}

// Purpose:       Write a byte of data to the specified chip
// Inputs:        chipSelect - which chip to write the data to
//                data - the byte of data to write

void glcd_writeByte(unsigned char chip, unsigned char data)
{
    SDI_TRIS = 0;   //configura o pino como entrada
    Nop();
    Nop();
    
	// Choose which chip to write to
	if(chip == SEL_GLCD_CS1) CS1 = 1;
   	else CS2 = 1;

	// Set for writing
	RW = 0;
	BARRAMENTO = data;
	Nop();
	Nop();
	E = 1;
	Nop();
	Nop();
	E = 0;
	Nop();
	Nop();

	// Reset the chip select lines
	CS1 = 0;
	CS2 = 0;
	
    SDI_TRIS = 1;   //configura o pino como entrada
}

// Purpose:       Reads a byte of data from the specified chip
// Ouputs:        A byte of data read from the chip
unsigned char glcd_readByte(unsigned char chip)
{
	unsigned char data=0;						// Stores the data read from the LCD

    SDI_TRIS = 0;   //configura o pino como entrada
    Nop();
    Nop();

	// Choose which chip to write to
	if(chip == SEL_GLCD_CS1) CS1 = 1;
   	else CS2 = 1;

	// Set port d to input
	BARRAMENTO_TRIS = 0xFF;

	// Set for reading
	RW = 1;
	Nop();
	Nop();
	E = 1;
	Nop();
	Nop();
	E = 0;
	Nop();
	Nop();
	E = 1;
	Nop();
	Nop();
	data = BARRAMENTO;
	Nop();
	Nop();
	E = 0;

	// Reset the chip select lines
	CS1 = 0;
	CS2 = 0;

	// Set port d to output
	BARRAMENTO_TRIS = 0x00;
    
    SDI_TRIS = 1;   //configura o pino como entrada
}
unsigned char leitura_touch(unsigned char eixo)
{
    unsigned char valor,i;
    
    CS_T = 1;       //seleciona o controlador touch
//    SDI_TRIS = 1;   //configura o pino como entrada

	Nop();
	Nop();
	
    CS_T = 0;       //seleciona o controlador touch

/*************************************************
*		  Palavra de controle ADS7843  			 *
**************************************************
* S - A2 - A1 - A0 - MODE - SFR/DFR - PD1 - PD0	 *
**************************************************
*  S           -> Start bit 					 *
*                  (nível lógico 1)              *
*                                                *
*  A2  A1  A0  -> Seleção do canal da conversão  *
*   1   0   1   =  canal X                       *
*   0   0   1   =  canal Y                       *
*                                                *
*  MODE        -> Resolução de conversão         *
*   1           =  8 bits                        *
*   0           =  16 bits                       *
*                                                *
*  SFR/DFR     -> Modo da converssão             *
*   0           =  Single ended                  *
*   1           =  Differential                  *
*                                                *
*  PD1 PD0     -> Modos de operação              *
*   0   0       =  Power down on  +  PENIRQ on   * 
*   0   1       =  Power down on  +  PENIRQ off  * 
*   1   1		=  Power down off +  PENIRQ off  *
*                                                *
*************************************************/

	if (eixo)
	  	valor = 0b11010001;	  	
	else
		valor = 0b10010001;		

	for (i=0;i<8;i++) 
	{
		if(valor&0x80)
			SDO = 1;
		else
			SDO = 0;		
	
		Nop();
		Nop();
		SCK = 1;	   	
		Nop();
		Nop();
	    SCK = 0;	   	
		Nop();
		Nop();
		valor = valor<<1;
	}
   
    SDO = 0;	   	

	Delay10TCYx(2);

	valor = 0;
	for (i=0;i<8;i++) 
	{
    	valor = valor<<1;
    	Nop();
    	Nop();
    	SCK = 1;	   	
    	Nop();
    	Nop();
        SCK = 0;	   	
    	Nop();
    	Nop();
		if(SDI == 1)
			valor = valor | 0x01;
	}

    CS_T = 1;
//    SDI_TRIS = 0;   //configura o pino como saída
    
    return valor;
}


void filtro_touch(unsigned char canal)
{
    if (canal == 0)
    {
        if (linha_touch == linha_touch_anterior)
        {
	        filtro_linha_touch_lib = t_f_touch_lib;           // reinicia filtro de debounce
	        
	        filtro_linha_touch_press--;
	        if (!filtro_linha_touch_press)
	        {
		        linha_touch_ok=1;                       // marca que linha está ok.
		    }
        }
        else
        {
	        filtro_linha_touch_press = t_f_touch_press;           // reinicia filtro de debounce

            filtro_linha_touch_lib--;
            if (!filtro_linha_touch_lib)
            {
                linha_touch_ok = 0;                       // marca que linha está ok.
				linha_touch_anterior = 0;
                pos_touch_ok = 0;
				t_f_touch_press = 10;
				t_f_touch_lib = 2;
            }
        }
        linha_touch_anterior = linha_touch;               // realiza o atraso
    }
    else
    {
        if (coluna_touch == coluna_touch_anterior)
        {
	        filtro_coluna_touch_lib = t_f_touch_lib;           // reinicia filtro de debounce

            filtro_coluna_touch_press--;
            if (!filtro_coluna_touch_press)
            {
	            coluna_touch_ok = 1;                       // marca que linha está ok.
            }
        }
        else
        {
            filtro_coluna_touch_press = t_f_touch_press;           // reinicia filtro de debounce

            filtro_coluna_touch_lib--;
             if (!filtro_coluna_touch_lib)
             {
 		         coluna_touch_ok = 0;                       // marca que linha está ok.
				 coluna_touch_anterior = 0;
                 pos_touch_ok = 0;
				 t_f_touch_press = 10;
				 t_f_touch_lib = 2;
             }
        }
        coluna_touch_anterior = coluna_touch;               // realiza o atraso
    }
    if ( linha_touch_ok && coluna_touch_ok )
	{
		pos_touch_ok = 1;
		linha_touch_ok = 0;
		coluna_touch_ok = 0;
		t_f_touch_press = 1;
		t_f_touch_lib = 2;
    }
}
