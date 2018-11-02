#include <p18f4550.h>
#include <spi.h>
#include <delays.h>
#include <ADXL345.h> // Made by me
#pragma config WDT = OFF
#pragma config FOSC = XT_XT // 4 Mhz XT

#define SPI_CS PORTEbits.RE2 // pin#10 as Chip Select

unsigned char address = 0, data = 0;
unsigned char var, x_low;

void byte_write (unsigned char address, unsigned char data);
unsigned char byte_read (unsigned char address);
void initialize (void);

void main (void){
	initialize ();
	SPI_CS = 1; // Transferred disabled
	OpenSPI(SPI_FOSC_4, MODE_11, SMPMID); // SCL = 1 MHz, MID sampling 
	
	byte_write (DATA_FORMAT, 0b00001101); // SELF_TEST = OFF, SPI = 4 wire, INTERRUPTS active high, RESOLUTION 16 bit, left-justified, 4g (01)
	byte_write (BW_RATE, 0b00001000); // Set sensor refreshing 400 Hz, LOW_POWER = OFF
	byte_write (FIFO_CTL, 0b00011111); // Bypass mode (00), TRIG linked to INT1, all zeros would be a reset value

	byte_write (POWER_CTL, 0b00001000); // Normal operation, starting measuring
	
	while (1)
	{
		x_low = byte_read (DATAX0); // x_low is the low part of the X axis register

		if (x_low != 0){
			PORTD = x_low;
			Delay1KTCYx(300); // LED blinking could be too fast so I have to put a delay
		}	
	}
}

void initialize (void){
	TRISE = 0; // For Chip Select (output)
	TRISD = 0; // Output
}

void byte_write (unsigned char address, unsigned char data){
	SPI_CS = 0;
	var = putcSPI(SPI_WRITE);
	var = putcSPI(address);
	var = putcSPI(data);
	SPI_CS = 1;
}

unsigned char byte_read (unsigned char address){
	SPI_CS = 0;
	var = putcSPI(SPI_READ);
	var = putcSPI(address);
	var = getcSPI();
	SPI_CS = 1;
	return (var);
}

