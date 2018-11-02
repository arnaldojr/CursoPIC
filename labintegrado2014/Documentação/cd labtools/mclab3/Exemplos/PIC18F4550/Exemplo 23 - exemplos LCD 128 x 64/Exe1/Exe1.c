/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *              Exemplo de Código para utilização com a McLab3             *
 *                          Exemplo 23 (Exe 1)                             *
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
	Este software está preparado para no LCD Gráfico. O software realiza
    uma sequência de instruções gráficas que ensinam como utilizar cada
    função específica da biblioteca.
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
void glcd_fillScreen(unsigned char color);
void glcd_writeByte(unsigned char chip, unsigned char data);
unsigned char glcd_readByte(unsigned char chip);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           Constantes internas                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//A definição de constantes facilita a programação e a manutenção.

#define GLCD_WIDTH 128    // Used for text wrapping by glcd_text57 function

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
unsigned char texto[4];

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

#define SDI	 		PORTCbits.RC4		// PINO SDI DO CONTROLADOR TOUCH
#define SDO	 		LATCbits.LATC7		// PINO SDO DO CONTROLADOR TOUCH
#define SCK	 		LATBbits.LATB1		// PINO SCK DO CONTROLADOR TOUCH
#define CS_T 		LATAbits.LATA2		// PINO CHIP SELECT DO CONTROLADOR TOUCH

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

	while(1)
	{

	for (aux=0;aux<64;aux = aux + 7)		//gera tabuleiro
		glcd_line(0,aux,127,aux,ON);

	for (aux=0;aux<128;aux = aux + 7)
		glcd_line(aux,0,aux,63,ON);

	Delay1KTCYx(3000);

	glcd_fillScreen(OFF);

	for (aux=0;aux<64;aux = aux + 7)		// efeito com linha
		glcd_line(0+aux,0+aux,127-aux,63-aux,ON);

	Delay1KTCYx(3000);

	glcd_fillScreen(OFF);

	for (aux=0;aux<128;aux = aux + 15)		// sequência de barras
	 	glcd_rect(0+aux,0,7+aux,63,1,1);

	Delay1KTCYx(3000);

	glcd_fillScreen(OFF);

	glcd_circle(10,10,10,1,ON);				// animação com círculos
	glcd_circle(117,53,10,1,ON);
	glcd_circle(10,53,10,1,ON);
	glcd_circle(117,10,10,1,ON);
	glcd_circle(63,31,20,0,ON);

	Delay1KTCYx(3000);

	glcd_fillScreen(OFF);

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
}

// Purpose:       Reads a byte of data from the specified chip
// Ouputs:        A byte of data read from the chip
unsigned char glcd_readByte(unsigned char chip)
{
	unsigned char data=0;						// Stores the data read from the LCD

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

  	return data;               				// Return the read data
}