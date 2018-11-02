
// ADXL345 Accelerometer and 18F4550 in Hi-Tech C
// Using the hardware MSSP I2C port on RB0 & RB1 and LCD on port D

/*   Using HI-TECH C for PIC18 MCU Family.

  LCD on D  ICSP is on Port B6 and B7.
  The wiring for the LCD using Hitachi HD44780 is at
  http://www.winpicprog.co.uk/pic_tutorial_lcd_board.htm
*/
//#include <pic18f4550.h>
#include <pic18.h>
#include <htc.h>
#include <stdlib.h>
#include <stdio.h>

/* Configuration words for a PIC18F4450
* with a 20 MHz crystal oscillator
*/
__CONFIG(1,USBPLL&PLLDIV5&CPUDIV4&HSPLL&FCMEN&IESOEN);
__CONFIG(2,VREGDIS&PWRTEN&BORDIS&BORV42&WDTDIS&WDTPS32K);
__CONFIG(3,PBDIGITAL&LPT1DIS&MCLREN);
__CONFIG(4,XINSTDIS&STVRDIS&LVPDIS&ICPORTDIS&DEBUGDIS);
__CONFIG(5,UNPROTECT);
__CONFIG(6,UNPROTECT);
__CONFIG(7,UNPROTECT);


char  IDDV =0;   
#define  BW_RATE            44    //0x2C    
#define  POWER_CTL          45    //0x2D    
#define  DATA_FORMAT        49    //0x31    
#define  DATAX0             50    //0x32
#define  DATAX1             51    //0x33
#define  DATAY0             52    //0x34
#define  DATAY1             53    //0x35
#define  DATAZ0             54    //0x36
#define  DATAZ1             55    //0x37
#define  FIFO_CTL           56    //0x38			ADXL1345	1010011   0x53   Device
#define CHIP_Write	0xA6		// adxl345 address for writing	10100110  0xA6   Write
#define CHIP_Read	0xA7		// and reading					10100111  0xA7   Read


#define _XTAL_FREQ  20000000		// Xtal speed

#define LCD_PORT PORTD			// constants
#define LCD_TRIS TRISD			//
#define LCD_RS 	LATD4		// It didn't like RD4?
#define LCD_RW	LATD6
#define LCD_E	LATD7


		// Required prototypes.. each function needs to be declared
		// if called BEFORE definition.

void LCD_init(void), LCD_cmd(unsigned char ch);
void LCD_goto(char line, char column), LCD_clr(void);
void LCD_cur(unsigned char ch), pulse_E(void);
void LCD_char(unsigned char ch), LCD_charD(unsigned char ch);
void LCD_printC(const char *str), LCD_printR(char *str);
void delayMs(int x);

// Hardware I2C functions
void I2CInit(void);
void I2CStart();
void I2CStop();
void I2CRestart();
void I2CAck();
void I2CNak();
void I2CWait();
void I2CSend(unsigned char dat);
unsigned char I2CRead(void);
char E_Write(int addr, unsigned char ch);
unsigned char E_Read(int addr);



 void ini_adxl345()
{
 E_Write(FIFO_CTL,0x9f); 
__delay_ms(10);        
 E_Write(DATA_FORMAT,0x09);  
__delay_ms(10);      
 E_Write(BW_RATE,0x0d);
__delay_ms(10);            
 E_Write(POWER_CTL,0x08);  // activate      
}

unsigned char LCDbuffer[17];
char ErrFlags;			// Read error. (I don't use it, but you might!)
		


/* our main program loop */
void main(void)
{
	CMCON = 0x07;						// Comparitors off

	int index = 0;						// text variable
	unsigned char ch;
	LCD_TRIS = 0b00000000;			// LCD port as outputs
	LCD_PORT = 0b00000000;			// Clear LCD port
	delayMs(150);					// let LCD stabilise 
	LCD_init();						// Initalise screen

//	LCD_goto(1,0);	// line 1.
//	LCD_printC("Hi World");

	// Initialize I2C Port 
	I2CInit();
	// Initialize ADXL345 Accelerometer
	ini_adxl345();

/*	LCD_goto(1,0);
      int id, pow, bw_rate;
      id= E_Read(IDDV);  // should be 229 for ADXL1345
 	__delay_ms(10);
      pow=E_Read(0x2D);  //should be 8 to activate
	 __delay_ms(10);
      bw_rate =E_Read(0x2C);  //BW_RATE 0x2C is 13 
		sprintf(LCDbuffer,"id=%3d pow=%3d BW_RATE=%3d",id,pow,bw_rate);
		LCD_printR(LCDbuffer);		// use sprintf to format
*/

    signed long x,y,z;
	signed long xhi,xlo,yhi,ylo,zhi,zlo;
	signed long Xaccumulate, Yaccumulate, Zaccumulate;
	signed long Xaverage, Yaverage, Zaverage;
	int i;


  while(1) {

	Xaccumulate = Yaccumulate = Zaccumulate = 0;

	for (i=0; i<16; i++) {  // Read sequentially 16 times then get an average.


	ErrFlags = 0;									// Clear error
	I2CStart();
	I2CSend(CHIP_Write);
	I2CSend(DATAX0);	// DATAX0 is the first of 6 bytes 
	I2CRestart();
	I2CSend(CHIP_Read);							//
	xlo = I2CRead();								// read character
	I2CAck();
	xhi = I2CRead();								// read character
	I2CAck();
	ylo = I2CRead();								// read character
	I2CAck();
	yhi = I2CRead();								// read character
	I2CAck();
	zlo = I2CRead();								// read character
	I2CAck();
	zhi = I2CRead();								// read character
	I2CNak();
	I2CStop();


	Xaccumulate += ((xhi<<8) | xlo); //Xaccumulate = Xaccumulate + (xhi*256 + xlo)
	Yaccumulate += ((yhi<<8) | ylo);
	Zaccumulate += ((zhi<<8) | zlo);

  }  // for loop

//	x= (xhi<<8) | xlo;
//	y= (yhi<<8) | ylo;
//	z= (zhi<<8) | zlo;

// Calculate average accelerometer readings over last 16 samples

/* *** Other methods of dividing gave me results in the +/- thousands ***
DIV   From Hi-Tech C manual
Synopsis
#include <stdlib.h>
div_t div (int numer, int demon)
Description
The div() function computes the quotient and remainder of the numerator divided by the denominator.
Example
#include <stdlib.h>
#include <stdio.h>
void
main (void)
{
div_t x;
x = div(12345, 66);
printf("quotient = %d, remainder = %d\n", x.quot, x.rem);
}
See Also
udiv(), ldiv(), uldiv()
Return Value
Returns the quotient and remainder into the div_t structure.
*/

div_t xavg ; // div_t is a structure explained above
div_t yavg ;
div_t zavg ;

xavg = div(Xaccumulate, 16);
yavg = div(Yaccumulate, 16);
zavg = div(Zaccumulate, 16);

LCD_goto(1,0);
		sprintf(LCDbuffer,"X=%5d",xavg.quot);
		LCD_printR(LCDbuffer);		// use sprintf to format

LCD_goto(1,9);
		sprintf(LCDbuffer,"Y=%5d",yavg.quot);
		LCD_printR(LCDbuffer);		// use sprintf to format

LCD_goto(2,4);		
sprintf(LCDbuffer,"Z=%5d",zavg.quot);
		LCD_printR(LCDbuffer);		// use sprintf to format
__delay_ms(25);



/*
// This reads byte registers one at a time.
   x=(E_Read(DATAX1)*256) | (E_Read(DATAX0)); 
   y=(E_Read(DATAY1)*256) | (E_Read(DATAY0));
   z=(E_Read(DATAZ1)*256) | (E_Read(DATAZ0));
 

LCD_goto(1,0);
		sprintf(LCDbuffer,"x=%5d",x);
		LCD_printR(LCDbuffer);		// use sprintf to format

LCD_goto(1,12);
		sprintf(LCDbuffer,"y=%5d",y);
		LCD_printR(LCDbuffer);		// use sprintf to format

LCD_goto(2,0);		
sprintf(LCDbuffer,"z=%5d",z);
		LCD_printR(LCDbuffer);		// use sprintf to format
*/

 
   			}   // while loop
}  // Main





/*
Function: I2CInit
Return:
Arguments:
Description: Initialize I2C in master mode, Sets the required baudrate
*/
void I2CInit(void)
{
	TRISBbits.TRISB0 = 1; /* SDA and SCL as input pin */
 	TRISBbits.TRISB1 = 1; /* these pins can be configured either i/p or o/p */
	SSPSTAT |= 0x80; /* Slew rate disabled */
	SSPCON1 = 0x28;//Enable SDA and SCL, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
				/* SSPEN = 1, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1)) */
    SSPCON2 = 0x00;      // Reset MSSP Control Register
	SSPADD = 49;      //20000000 / 4= 5000000, 5000000/ 100000= 50 50-1=49
	PIR1bits.SSPIF=0;    // Clear MSSP Interrupt Flag
    PIR2bits.BCLIF=0;
/* *** For different Frequencies ***
FOSC     FCY  SSPADD Value  FCLOCK
40 MHz 10 MHz 	63h 		100 kHz
32 MHz 	8 MHz	4Fh 		100 kHz  
20 MHz 	5 MHz	31h, 49d 	100 kHz
16 MHz 	4 MHz 	27h 		100 kHz
 8 MHz 	2 MHz 	13h, 19d	100 kHz
 4 MHz 	1 MHz 	09h 		100 kHz
*/
}


/*
Function: I2CStart
Return:
Arguments:
Description: Send a start condition on I2C Bus
*/
void I2CStart()
{
	SEN = 1;         /* Start condition enabled */
	while(SEN);      /* automatically cleared by hardware */
                     /* wait for start condition to finish */
}

/*
Function: I2CStop
Return:
Arguments:
Description: Send a stop condition on I2C Bus
*/
void I2CStop()
{
	PEN = 1;         /* Stop condition enabled */
	while(PEN);      /* Wait for stop condition to finish */
                     /* PEN automatically cleared by hardware */
}

/*
Function: I2CRestart
Return:
Arguments:
Description: Sends a repeated start condition on I2C Bus
*/
void I2CRestart()
{
	RSEN = 1;        /* Repeated start enabled */
	while(RSEN);     /* wait for condition to finish */
}

/*
Function: I2CAck
Return:
Arguments:
Description: Generates acknowledge for a transfer
*/
void I2CAck()
{
	ACKDT = 0;       /* Acknowledge data bit, 0 = ACK */
	ACKEN = 1;       /* Ack data enabled */
	while(ACKEN);    /* wait for ack data to send on bus */
}

/*
Function: I2CNck
Return:
Arguments:
Description: Generates Not-acknowledge for a transfer
*/
void I2CNak()
{
	ACKDT = 1;       /* Acknowledge data bit, 1 = NAK */
	ACKEN = 1;       /* Ack data enabled */
	while(ACKEN);    /* wait for ack data to send on bus */
}

/*
Function: I2CWait
Return:
Arguments:
Description: wait for transfer to finish
*/
void I2CWait()
{
	while ((SSPCON2 & 0x1F ) || ( SSPSTAT & 0x04 ) );
    /* wait for any pending transfer */
}

/*
Function: I2CSend
Return:
Arguments: dat - 8-bit data to be sent on bus
           data can be either address/data byte
Description: Send 8-bit data on I2C bus
*/
void I2CSend(unsigned char dat)
{
	SSPBUF = dat;    /* Move data to SSPBUF */
	while(BF);       /* wait till complete data is sent from buffer */
	I2CWait();       /* wait for any pending transfer */
}

/*
Function: I2CRead
Return:    8-bit data read from I2C bus
Arguments:
Description: read 8-bit data from I2C bus
*/
unsigned char I2CRead(void)
{
	unsigned char temp;
/* Reception works if transfer is initiated in read mode */
	RCEN = 1;        /* Enable data reception */
	while(!BF);      /* wait for buffer full */
	temp = SSPBUF;   /* Read serial buffer and store in temp register */
	I2CWait();       /* wait to check any pending transfer */
	return temp;     /* Return the read data from bus */
}


// This is random writing. Write to a specified address
char E_Write(int addr, unsigned char ch)
	{
	/* Send Start condition */
	I2CStart();
	/* Send ADXL1345 slave address with write operation */
	I2CSend(CHIP_Write);
	/* Send subaddress, we are writing to this location */
	I2CSend(addr);
	/* send I2C data one by one */
	I2CSend(ch);
	/* Send a stop condition - as transfer finishes */
	I2CStop();
	return 1;										// All went well
	}

// The function takes an address and Returns a character
// This is random reading. Read from a specified addess
unsigned char E_Read(int addr)
	{
	unsigned char byte;
	unsigned char ch;
	ErrFlags = 0;									// Clear error
	/* Send Start condition */
	I2CStart();
	/* Send ADXL1345 slave address with write operation */
	I2CSend(CHIP_Write);
	/* this address is actually where we are going to read from */
	I2CSend(addr);
	/* Send a repeated start, after a dummy write to start reading */
	I2CRestart();
	/* send slave address with read bit set */
	I2CSend(CHIP_Read);

	ch = I2CRead();								// read character
	I2CNak();
	I2CStop();
	return ch;
	}




// This may take calculated delay times. Don't use for LCD
void delayMs(int x)
	{
	while(x--)
		__delay_ms(1);
	}

void LCD_printC(const char * str)	// This passes the start a ROM character array
	{								// by default the pointer points to data section
	while(*str != 0)				// while the character pointed to isn't 0
		LCD_char(*str++);			// print out the character, then increment
	}								// the pointer down the array

void LCD_printR(char * str)	 		// This passes the start of a RAM character array
	{								// by default the pointer points to data section
	while(*str != 0)				// while the character pointed to isn't 0
		LCD_char(*str++);			// print out the character, then increment
	}

void LCD_init()
	{
	LCD_cmd(0x20);					// 4 bit
	LCD_cmd(0x28);					// display shift
	LCD_cmd(0x6);					// character mode
	LCD_cmd(0xc);					// display on / off and cursor
	LCD_clr();						// clear display
	}

void LCD_goto(char line, char column)		// combines line and lineW
	{
	unsigned char data = 0x80;				// default to 1
	if(line == 2)data = 0xc0;				// change to 2
	data += column;							// add in  column
	LCD_cmd(data);
	}

void LCD_clr()
	{
	LCD_cmd(1);								// Clr screen
	}

void LCD_cur(char on)
	{
	unsigned char cur = 0xc;				// cursor off
	if(on) cur = 0xd;						// cursor on
	LCD_cmd(cur);
	}

void LCD_cmd(unsigned char ch)
	{
	LCD_PORT = ch >> 4 & 0xf;			// write high nibble
	LCD_RS = 0;
	pulse_E();
	LCD_PORT = ch & 0xf;				// write low nibble
	LCD_RS = 0;
	pulse_E();
	__delay_ms(5);
	}

void LCD_charD(unsigned char ch)
	{
	ch+=0x30;
	LCD_char(ch);						// convert to ascii
	}

void LCD_char(unsigned char ch)
	{
	LCD_PORT = ch >> 4 & 0xf;			// High nibble
	LCD_RS = 1;
	pulse_E();
	LCD_PORT = ch & 0xf;				// low nibble
	LCD_RS = 1;
	pulse_E();
	__delay_ms(5);
	}

void pulse_E()
	{
	LCD_E = 1;
	__delay_us(1);
	LCD_E = 0;
	}