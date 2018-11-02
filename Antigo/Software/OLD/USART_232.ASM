; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                                                                         *
; *                               EXEMPLO 0                                 *
; *                           ENGENHARIA MAUÁ 		                        *
; *            				  4ºANO ELETRÔNICA       				        *
; *                                                                         *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *     DATA : 05/09/2013                                                   *
; *  	VERSÃO : 0.51                                                       *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *                             DESCRIÇÃO GERAL                             *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;
;  COMUNICAÇÃO USART 232
;  TRANSMIÇÃO DE DADOS PELA SERIAL.
;
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     ARQUIVOS DE DEFINIÇÕES                      *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#INCLUDE <P16F877A.INC>	; MICROCONTROLADOR UTILIZADO

	__CONFIG _BOREN_ON & _CP_OFF & _PWRTE_OFF & _WDT_OFF & _LVP_OFF & _HS_OSC
	

TXREG_T EQU 0x20
CNT0 EQU 0x21
CNT1 EQU 0x22

        ORG 0x00
INIT:
        ;---INIT PORTS
        ;CLEAR THE PORTS
        CLRF PORTB
        CLRF PORTC
		CLRF PORTD
        
        BSF STATUS, RP0 ;SWITCH TO BANK 01
        
        CLRF TRISC ;SET ALL PINS OF PORTC AS OUTPUT
        MOVLW B'00000001' ;SET PB4 AS INPUT AND THE REST PORTB AS OUTPUT
        MOVWF TRISD
        
        MOVLW B'00000111' ;DISABLE COMPARATORS MODULES
        MOVWF CMCON

        BCF OPTION_REG, 7 ;ENABLE PULL-UP RESISTORS OF PORTB


        ;---CONFIGURE SPBRG FOR DESIRED BAUD RATE
        MOVLW D'64' ;WE WILL USE 19200bps 
        MOVWF SPBRG ;BAUD AT 20MHZ

        
        ;---CONFIGURE TXSTA
        MOVLW B'00100100' ;CONFIGURE TXSTA AS :
        MOVWF TXSTA ;
        ;8 BIT TRANSMISSION - 6.BIT
        ;TRANSMIT ENABLED - 5.BIT
        ;ASYNCHRONOUS MODE - 4.BIT
        ;ENABLE HIGH SPEED BAUD RATE - 2.BIT

        BCF STATUS, RP0 ;SWITCH TO BANK 00
        
        MOVLW B'10000000' ;ENABLE SERIAL PORT
        MOVWF RCSTA ;RECEIVE STATUS REG
        
        MOVLW 0x20
        MOVWF TXREG_T ;TO TXREG

MAIN:
       BTFSC PORTD, 0 ;CHECK IF THE BUTTON IS PRESSED (BECOMES LOW FROM HIGH, WE USE PULL-UP RESITORS NOT PULL-DOWN)
       CALL	MENSAGEM1
		GOTO MAIN ;IF NOT CONTINUE TO LOOP
        
        ;ELSE TRANSMIT A BYTE
        INCF TXREG_T
        MOVF TXREG_T, W
        XORLW 0x7F
        BTFSS STATUS, Z
        GOTO NEXT_0
        MOVLW 0x21 ;'!'
        MOVWF TXREG_T ;TO TXREG
NEXT_0:        
        MOVF TXREG_T, W
        MOVWF TXREG ;TO TXREG
        
TRANSWT:
        BSF STATUS, RP0
WTHERE:
        BTFSS TXSTA, TRMT
    GOTO WTHERE

    BCF STATUS, RP0
        
        CALL DELAY
                        
        GOTO MAIN
MENSAGEM1
	    MOVLW 0x21 ;'!'
        MOVWF TXREG_T ;TO TXREG
		GOTO	TRANSWT
		RETURN

DELAY:
        MOVLW 0xF0
        MOVWF CNT1
D1:
        MOVLW 0xFA
        MOVWF CNT0
D0:
        DECFSZ CNT0
        GOTO D0
        
        DECFSZ CNT1
        GOTO D1

        RETURN

        END





;;Tutorial 11
;;Reading an analogue input, and displaying it on the LCD and RS232
;
;
;;	LIST	p=16F876, W=2, X=ON, R=DEC	;tell assembler what chip we are using
;;	include "P16F876.inc"			;include the defaults for the chip
;;	ERRORLEVEL	0,	-302		;suppress bank selection messages
;;	__CONFIG    0x393A			;sets the configuration settings (oscillator type etc.)
;
;
;		cblock	0x20			;start of general purpose registers
;			count
;			count1	
;			counta	
;			countb	
;			LoX		
;			Bit_Cntr	
;			Timer_H		
;			Flags		
;			Flags2		
;			tmp1		
;			tmp2		
;			tmp3			
;			NumL	
;			NumH	
;			TenK		
;			Thou	
;			Hund	
;			Tens			
;			Ones		
;			templcd	
;			templcd2	
;			Point			;position of decimal point
;		endc
;
;LCD_PORT	Equ	PORTB
;LCD_TRIS	Equ	TRISB
;LCD_RS		Equ	0x04			;LCD handshake lines
;LCD_RW		Equ	0x06
;LCD_E		Equ	0x07
;
;LEADZ         	Equ   	0x00   			;set to display leading zeros
;LCD		Equ	0x01			;set to print to LCD
;RS232		Equ	0x02			;set to print to RS232
;
;;start of program
;
;		ORG	0x0000
;		NOP
;		BCF	PCLATH,	3
;		BCF	PCLATH,	4
;		GOTO	Initialise
;
;		ORG	0x0004
;		RETURN
;
;		ORG	0x0005
;
;
;Initialise	clrf	count
;		clrf	PORTA
;		clrf	PORTB
;		clrf	PORTC
;		clrf	Flags
;		BANKSEL ADCON1			;disable A2D
;    		movlw   0x06
;    		movwf   ADCON1
;    		BANKSEL PORTA	
;						;variables for decimal numbers display
;		bsf	Flags, LEADZ		;show leading zeros
;		bsf	Flags, LCD		;set to print to LCD
;		bsf	Flags, RS232		;and also to RS232
;		movlw	0x02			;set decimal point position to two
;		movwf	Point
;
;
;
;SetPorts	bsf 	STATUS,		RP0	;select bank 1
;		movlw	0x00			;make all LCD pins outputs
;		movwf	LCD_TRIS
;		bcf 	STATUS,		RP0	;select bank 0
;		call	SER_INIT		;initialise serial port
;		call	LCD_Init		;setup LCD module
;		call	LCD_CurOff		;turn cursor off
;		call	Init_ADC		;initialise analogue input
;
;Main
;		bcf	Flags,	RS232		;turn off RS232
;		call	LCD_Line1		;set to first line
;		call	String1			;display title string
;		bsf	Flags,	RS232		;turn RS232 back on
;
;		call	LCD_Line2
;		call	Read_ADC		;read analogue input
;		call	LCD_Decimal		;and display (in decimal)
;
;		bcf	Flags,	RS232		;turn off RS232
;		movlw	'V'
;		call	LCD_Char		;display a 'V' on LCD only
;
;		bsf	Flags,	RS232		;turn RS232 back on
;		bcf	Flags,	LCD		;and turn off LCD
;		movlw	0x0A
;		call	LCD_Char		;display a LF
;		movlw	0x0D
;		call	LCD_Char		;display a CR
;		bsf	Flags,	LCD		;turn LCD back on
;
;		call	Delay100		;delay to give 10 readings per second
;
;		goto	Main			;loop for ever
;
;Init_ADC
;; Set ADCON0
;    		movlw   b'10000001'
;    		movwf   ADCON0
;; Set ADCON1
;    		BANKSEL ADCON1
;    		movlw   b'10000101'
;    		movwf   ADCON1
;    		BANKSEL ADCON0
;		return
;
;Read_ADC
;    		bsf	ADCON0, GO_DONE		;initiate conversion
;    		btfsc   ADCON0, GO_DONE
;    		goto    $-1			;wait for ADC to finish
;
;    		movf    ADRESH,W
;    		andlw   0x03
;    		movwf   NumH
;    		BANKSEL ADRESL
;    		movf    ADRESL,W
;    		BANKSEL	ADRESH
;			movwf	NumL			;return result in NumL and NumH
;		return
;
;
;;TABLES
;HEX_Table  	addwf   PCL       , f
;            	retlw   0x30
;            	retlw   0x31
;            	retlw   0x32
;            	retlw   0x33
;            	retlw   0x34
;            	retlw   0x35
;            	retlw   0x36
;            	retlw   0x37
;            	retlw   0x38
;            	retlw   0x39
;            	retlw   0x41
;            	retlw   0x42
;            	retlw   0x43
;            	retlw   0x44
;            	retlw   0x45
;            	retlw   0x46
;
;Xtext		addwf	PCL, f
;		retlw	'T'
;		retlw	'u'
;		retlw	't'
;		retlw	'o'
;		retlw	'r'
;		retlw	'i'
;		re	tlw	'a'
;		retlw	'l'
;		retlw	' '
;		retlw	'1'
;		retlw	'1'
;		retlw	'.'
;		retlw	'4'
;		retlw	0x00
;
;;end of tables
;
;String1		clrf	count			;set counter register to zero
;Mess1		movf	count, w		;put counter value in W
;		call	Xtext			;get a character from the text table
;		xorlw	0x00			;is it a zero?
;		btfsc	STATUS, Z
;		retlw	0x00			;return when finished
;		call	LCD_Char
;		incf	count, f
;		goto	Mess1
;
;;USART Serial routines
;
;SER_INIT
;           	BSF     STATUS, RP0           ;select bank 1
;     		MOVLW   d'129'                ;9600 baud @ 20 Mhz Fosc +0.16 err
;     		MOVWF   SPBRG
;     		MOVLW   b'00100100'           ;brgh = 1
;     		MOVWF   TXSTA                 ;enable Async Transmission, set brgh
;            BCF     STATUS, RP0           ;select bank 0
;     		MOVLW   b'10010000'
;     		MOVWF   RCSTA                 ;enable Async Reception
;            RETURN
;
;XMIT_RS232  btfss   PIR1,	TXIF 	      ;xmit buffer empty?
;     		GOTO	XMIT_RS232            ;no, wait
;     		MOVWF   TXREG		      ;now send
;            RETURN
;
;Rcv_RS232   	BTFSS   PIR1,	RCIF 	      ; check for received data
;     		GOTO    Rcv_RS232
;                MOVF    RCREG,	W
;                RETURN
;
;;End of serial routines
;
;;LCD routines
;
;;Initialise LCD
;LCD_Init	call 	LCD_Busy		;wait for LCD to settle
;
;		movlw	0x20			;Set 4 bit mode
;		call	LCD_Cmd
;
;		movlw	0x28			;Set display shift
;		call	LCD_Cmd
;
;		movlw	0x06			;Set display character mode
;		call	LCD_Cmd
;
;		movlw	0x0c			;Set display on/off and cursor command
;		call	LCD_Cmd			;Set cursor off
;
;		call	LCD_Clr			;clear display
;
;		retlw	0x00
;
;; command set routine
;LCD_Cmd		movwf	templcd
;		swapf	templcd,	w	;send upper nibble
;		andlw	0x0f			;clear upper 4 bits of W
;		movwf	LCD_PORT
;		bcf	LCD_PORT, LCD_RS	;RS line to 1
;		call	Pulse_e			;Pulse the E line high
;
;		movf	templcd,	w	;send lower nibble
;		andlw	0x0f			;clear upper 4 bits of W
;		movwf	LCD_PORT
;		bcf	LCD_PORT, LCD_RS	;RS line to 1
;		call	Pulse_e			;Pulse the E line high
;		call 	LCD_Busy
;		retlw	0x00
;
;LCD_CharD	addlw	0x30			;add 0x30 to convert to ASCII
;LCD_Char	btfsc	Flags, RS232
;		call	XMIT_RS232		;do RS232 if flag set
;		btfss	Flags, LCD
;		return				;return if LCD not set
;		movwf	templcd
;		swapf	templcd,	w	;send upper nibble
;		andlw	0x0f			;clear upper 4 bits of W
;		movwf	LCD_PORT
;		bsf	LCD_PORT, LCD_RS	;RS line to 1
;		call	Pulse_e			;Pulse the E line high
;
;		movf	templcd,	w	;send lower nibble
;		andlw	0x0f			;clear upper 4 bits of W
;		movwf	LCD_PORT
;		bsf	LCD_PORT, LCD_RS	;RS line to 1
;		call	Pulse_e			;Pulse the E line high
;		call 	LCD_Busy
;		retlw	0x00
;
;LCD_Line1	movlw	0x80			;move to 1st row, first column
;		call	LCD_Cmd
;		retlw	0x00
;
;LCD_Line2	movlw	0xc0			;move to 2nd row, first column
;		call	LCD_Cmd
;		retlw	0x00
;
;LCD_Line1W	addlw	0x80			;move to 1st row, column W
;		call	LCD_Cmd
;		retlw	0x00
;
;LCD_Line2W	addlw	0xc0			;move to 2nd row, column W
;		call	LCD_Cmd
;		retlw	0x00
;
;LCD_CurOn	movlw	0x0d			;Set display on/off and cursor command
;		call	LCD_Cmd
;		retlw	0x00
;
;LCD_CurOff	movlw	0x0c			;Set display on/off and cursor command
;		call	LCD_Cmd
;		retlw	0x00
;
;LCD_Clr		movlw	0x01			;Clear display
;		call	LCD_Cmd
;		retlw	0x00
;
;LCD_HEX		movwf	tmp1
;		swapf	tmp1,	w
;		andlw	0x0f
;		call	HEX_Table
;		call	LCD_Char
;		movf	tmp1, w
;		andlw	0x0f
;		call	HEX_Table
;		call	LCD_Char
;		retlw	0x00
;
;Pulse_e		bsf	LCD_PORT, LCD_E
;		nop
;		bcf	LCD_PORT, LCD_E
;		retlw	0x00
;
;LCD_Busy
;		bsf	STATUS,	RP0		;set bank 1
;		movlw	0x0f			;set Port for input
;		movwf	LCD_TRIS
;		bcf	STATUS,	RP0		;set bank 0
;		bcf	LCD_PORT, LCD_RS	;set LCD for command mode
;		bsf	LCD_PORT, LCD_RW	;setup to read busy flag
;		bsf	LCD_PORT, LCD_E
;		swapf	LCD_PORT, w		;read upper nibble (busy flag)
;		bcf	LCD_PORT, LCD_E		
;		movwf	templcd2 
;		bsf	LCD_PORT, LCD_E		;dummy read of lower nibble
;		bcf	LCD_PORT, LCD_E
;		btfsc	templcd2, 7		;check busy flag, high = busy
;		goto	LCD_Busy		;if busy check again
;		bcf	LCD_PORT, LCD_RW
;		bsf	STATUS,	RP0		;set bank 1
;		movlw	0x00			;set Port for output
;		movwf	LCD_TRIS
;		bcf	STATUS,	RP0		;set bank 0
;		return
;
;LCD_Decimal
;		call    Convert
;		btfsc   Flags, LEADZ
;		goto    LCD_TENK
;		movf    TenK, w
;		btfss   STATUS, Z
;		goto    LCD_TENK
;		movf    Thou, w
;		btfss   STATUS, Z
;		goto    LCD_THOU
;		movf    Hund, w
;		btfss   STATUS, Z
;		goto    LCD_HUND
;		movf    Tens, w
;		btfss   STATUS, Z
;		goto    LCD_TENS
;		goto    LCD_ONES
;LCD_TENK
;		movlw	0x05			;test if decimal point 5
;		subwf   Point, w
;		btfss   STATUS    , Z
;		goto	NO_DP5
;		movlw	'.'
;		call	LCD_Char		;display decimal point
;NO_DP5		movf    TenK, w
;		call    LCD_CharD
;		movlw	0x04			;test if decimal point 4
;		subwf   Point, w
;		btfss   STATUS    , Z
;		goto	LCD_THOU
;		movlw	'.'
;		call	LCD_Char		;display decimal point
;LCD_THOU
;		movf    Thou, w
;		call    LCD_CharD
;		movlw	0x03			;test if decimal point 3
;		subwf   Point, w
;		btfss   STATUS    , Z
;		goto	LCD_HUND
;		movlw	'.'
;		call	LCD_Char		;display decimal point
;LCD_HUND
;		movf    Hund, w
;		call    LCD_CharD
;		movlw	0x02			;test if decimal point 2
;		subwf   Point, w
;		btfss   STATUS    , Z
;		goto	LCD_TENS
;		movlw	'.'
;		call	LCD_Char		;display decimal point
;LCD_TENS
;		movf    Tens, w
;		call    LCD_CharD
;		movlw	0x01			;test if decimal point 1
;		subwf   Point, w
;		btfss   STATUS    , Z
;		goto	LCD_ONES
;		movlw	'.'
;		call	LCD_Char		;display decimal point
;LCD_ONES
;		movf     Ones, w
;		call     LCD_CharD
;		return
;
;
;;end of LCD routines
;
;
;;Delay routines
;
;Delay255	movlw	0xff		;delay 255 mS
;		goto	d0
;Delay100	movlw	d'100'		;delay 100mS
;		goto	d0
;Delay50		movlw	d'50'		;delay 50mS
;		goto	d0
;Delay20		movlw	d'20'		;delay 20mS
;		goto	d0
;Delay5		movlw	0x05		;delay 5.000 ms (20 MHz clock)
;d0		movwf	count1
;d1		movlw	0xE7
;		movwf	counta
;		movlw	0x04
;		movwf	countb
;Delay_0		decfsz	counta, f
;		goto	$+2
;		decfsz	countb, f
;		goto	Delay_0
;
;		decfsz	count1	,f
;		goto	d1
;		retlw	0x00
;
;;end of Delay routines
;
;;This routine downloaded from http://www.piclist.com
;Convert:                        ; Takes number in NumH:NumL
;                                ; Returns decimal in
;                                ; TenK:Thou:Hund:Tens:Ones
;        swapf   NumH, w
;	iorlw	B'11110000'
;        movwf   Thou
;        addwf   Thou,f
;        addlw   0XE2
;        movwf   Hund
;        addlw   0X32
;        movwf   Ones
;
;        movf    NumH,w
;        andlw   0X0F
;        addwf   Hund,f
;        addwf   Hund,f
;        addwf   Ones,f
;        addlw   0XE9
;        movwf   Tens
;        addwf   Tens,f
;        addwf   Tens,f
;
;        swapf   NumL,w
;        andlw   0X0F
;        addwf   Tens,f
;        addwf   Ones,f
;
;        rlf     Tens,f
;        rlf     Ones,f
;        comf    Ones,f
;        rlf     Ones,f
;
;        movf    NumL,w
;        andlw   0X0F
;        addwf   Ones,f
;        rlf     Thou,f
;
;        movlw   0X07
;        movwf   TenK
;
;                    ; At this point, the original number is
;                    ; equal to
;                    ; TenK*10000+Thou*1000+Hund*100+Tens*10+Ones
;                    ; if those entities are regarded as two's
;                    ; complement binary.  To be precise, all of
;                    ; them are negative except TenK.  Now the number
;                    ; needs to be normalized, but this can all be
;                    ; done with simple byte arithmetic.
;
;        movlw   0X0A                             ; Ten
;Lb1:
;        addwf   Ones,f
;        decf    Tens,f
;        btfss   3,0
;        goto   Lb1
;Lb2:
;        addwf   Tens,f
;        decf    Hund,f
;        btfss   3,0
;        goto   Lb2
;Lb3:
;        addwf   Hund,f
;        decf    Thou,f
;        btfss   3,0
;        goto   Lb3
;Lb4:
;        addwf   Thou,f
;        decf    TenK,f
;        btfss   3,0
;        goto   Lb4
;
;        retlw	0x00
;
;
;		end


;   File: Remoto.ASM
;    PIC: 16F877A @ 20Mhz
;   Data: 28/05/2003
; Autor: Renie
;
; Descrição:
; Teste para PIC acessar portas etc. com comandos vindo do PC
; 19200 Baud, sem paridade, 1 Stop bit @ 20Mhz XTAL
; TX apartir do PIC em C6 (do PIC p/ P.C.) via MAX232
; RX entrada do pic em C7 (do P.C. p/ PIC) via MAX232
;
; Comandos:
; 0     Reset PIC 
; 1     inibe mensagem de confirmação dos comandos (OK) 21/07/2003
; 2     Reativar confirmação dos comandos  24/07/2003  
;	
; S     Enviar valor p/ endereço + 2 bytes p/endereco (banco,offset) +  1 byte (valor)
; L     Ler valor do endereço + 2 bytes p/endereço (banco,offset) devolve 1 byte
; E     Eco + 1 byte (qtd) + qdt bytes, devolve qtd bytes recebidos   (max 48) 
; 
;
; Any other command causes the error indicator to be set on the device. (C2)
;
;
; C6 - RS232 do PIC p/o MAX232
; C7 - RS232 do MAX232 p/o PIC

;	LIST p=16f877a,c=140
;        errorlevel 1,-(305)
;        include "p16f877a.inc"
;
;Bank0   equ     0x00
;Bank1   equ     0x01
;LOOP    equ     0x24
;TEMP    equ     0x25
;VALUE   equ     0x26
;COUNT   equ     0x27
;Byte0  	equ     0x70  ; 70 a 7F - comum a todos os bancos 
;Byte1  	equ     0x71
;Byte2  	equ     0x72
;Byte3   equ     0x73
;Byte4   equ     0x74
;;;;IRP     equ     0x07
;
;
;Boot    equ     0x28    	; controle primeira vez
;Conf    equ     0x29            ; controle de confirmação (OK) 21/07/2003
;
;	org 	0
;        clrf 	STATUS
;        clrf    Boot
;        clrf    Conf
;        goto 	BootStart
;        org 	4
;ISR     goto 	5            	; Interupt
;	org 	5
;
;
;
;BootStart
;        ;
;        ; Configurar I/O ports
;        ;
;        clrf    STATUS
;        movlw   b'00000000'
;        movwf   PORTA
;        movlw   b'00000000'
;        movwf   PORTB
;        movlw   b'01000000'
;        movwf   PORTC
;        movlw   b'00000000'
;        movwf   PORTD
;        movlw   b'00000000'
;        movwf   PORTE
;
;        banksel TRISA           ; Bank 1
;        movlw 	b'11111111'     ; tudo input
;        movwf   TRISA
;        movlw   b'00000110'
;        movwf   ADCON1          ; PortA input digital
;
;        movlw   b'11111111'     ; tudo input
;        movwf   TRISB
;        movlw   b'10000000'     ; Desabilitar resistores pullup PORTB
;        movwf   OPTION_REG
;
;        movlw   b'11000000' 
;		    		;  RC2 - RC5  output LED's
;                                ;  RC6 - RC7  input(serial)
;        movwf   TRISC
;
;        movlw   b'00000000'     ; todos output
;        movwf   TRISD
;
;        movlw   b'11100000'     ; tudo output (3 bit port)
;        movwf   TRISE
;
;	banksel PORTC
;
;        movlw   d'64'           ; 19.2K Baud 20MHz XTAL
;
;        banksel TRISA           ;  Bank 1
;;
;; Configurar gerador de baud rate
;;
;        movwf   SPBRG           ; bank 1
;        movlw   b'00100100'     ; BRGH = 1(High speed) assincrono
;        movwf   TXSTA           ; no bank 1
;        banksel RCSTA           ; Bank 0
;        movlw   b'10010000'     ; recepção assincrona
;        movwf   RCSTA           ; no bank 0
;        movf    RCREG,W         ; limpar buffer RX bank 0
; ;       movf    RCREG,W 
; ;	movf    RCREG,W
;
;;
;; Principal
;;
;
;        movlw   mReady          ; MSG - Ready
;	call    SendMessage
;	bsf    	Boot, 0      	; desmarcar primeira vez
;
;WaitCom
;;;        call 	LimpaBufferRX
;        call    RecLoop         ; espera e lê RS232 - caracter volta em 'w'
;
;; Identificar comando
;	movwf   TEMP            ; Salva 'W' em TEMP
;        sublw   0x53
;	btfsc   STATUS, Z       ; Se ASCII = 83 'S'
;        goto 	CmdSet          ; S - define  valor para endereço, recebe banco,offset e valor 
;       
;	movfw   TEMP            ; Recupera 'W' de TEMP
;        sublw   0x4C
;	btfsc   STATUS, Z       ; Se ASCII = 76 'L'
;        goto 	CmdLoad         ; L - Ler conteúdo do endereço, recebe banco e offset,
;				;     devolve 1 byte
;
;	movfw   TEMP            ; Recupera 'W' de TEMP
;        sublw   0x45
;	btfsc   STATUS, Z       ; Se ASCII = 69 'E'
;        goto 	CmdEco          ; E - Eco, recebe qtd de bytes (n) a ecoar e devidos n bytes,
;				;     devolve os n bytes recebidos
;
;	movfw   TEMP            ; Recupera 'W' de TEMP
;        sublw   0x36
;	btfsc   STATUS, Z       ; Se ASCII = 54 '6'
;        goto 	CmdVersao       ; Comando 6 - Devolver descrição e versão
;
;	movfw   TEMP            ; Recupera 'W' de TEMP         21/07/2003
;        sublw   0x31                                          ;21/07/2003
;	btfsc   STATUS, Z       ; Se ASCII = 49 '1'            21/07/2003 
;        goto 	CmdNconf        ; Comando 1 - supender confirmação (OK)  21/07/2003
;
;	movfw   TEMP            ; Recupera 'W' de TEMP         24/07/2003
;        sublw   0x32                                          ;24/07/2003
;	btfsc   STATUS, Z       ; Se ASCII = 50 '2'            24/07/2003 
;        goto 	Cmdconf         ; Comando 2 - ativar confirmação (OK)  24/07/2003
;
;NxtCmd1
;        movf    TEMP, W         ; Recuperar W
;        sublw   0x30
;	btfsc   STATUS, Z       ; SE ASCII = 48 '0'
;        goto 	BootStart       ; Comando 0 - Reset
;
;
;;
;; Comando desconhecido
;;
;        bsf     PORTC, 2        ; acender LED C2
;
;        movlw   mError          ; MSG - Erro !
;	call    SendMessage
;	goto 	WaitCom        	; Volta a esperar dados
;
;
;CmdNconf						;21/07/2003
;; Comando 1 - supender confirmação (OK)			 21/07/2003
;	bsf    Conf,0           ; não confirmar com OK	 21/07/2003
;        goto   WaitCom					;21/07/2003
;
;Cmdconf						    	;24/07/2003
;; Comando 2 - reativar confirmação (OK)			 24/07/2003
;	bcf    Conf,0           ;  confirmar com OK	 24/07/2003
;        goto   WaitCom					;24/07/2003
;
;
;
;CmdSet
;; Definir valor para endereço informado (banco,offset, valor)
;	call   RecLoop          ; receber banco do endereço
;        movwf  Byte1            ; guarda banco em temp1 (comum a todos os bancos)
;        call   RecLoop          ; recebe offset do endereço
;        movwf  Byte2            ; guarda offset em temp2 (comum a todos os bancos)     
;	call   RecLoop          ; recebe valor a ser movido p/ o endereço
;        movwf  Byte3		; guarda valor em temp3 (comum a todos os bancos)
;
;	;;;; para implementar: testar para não alterar a configuração da RS232
;	;; não bloquear alterar baud rate, experimentar alterar dinamicamente  24/07/2003
;    
;	btfss  Byte1,0		;24/07/2003 
;	goto   okseguro		;24/07/2003
;	movfw  Byte2		;24/07/2003
;        sublw  0x07		;24/07/2003
;	btfss  STATUS, Z        ;24/07/2003  
;	goto   okseguro		;24/07/2003
;	movlw  b'11000000'	;24/07/2003
;        iorwf  Byte3,f		; garante não alterar TRISC RS232 24/07/2003
;
;
;okseguro    	; setar banco
; 	bcf    STATUS,IRP       ; inicialmente forçar banco 0
;        movfw  Byte1
;	btfsc  Byte1,1          ; endereçamento indireto banco = bit 7 do status (IRP) e 
;                                ;Bit 7 do FSR         
;        bsf    STATUS,IRP   
;	bcf    Byte2,7
;	btfsc  Byte1,0
;        bsf    Byte2,7 
;	movfw  Byte2
;	movwf  FSR              ; offset para endereçamento indireto 
;	movfw  Byte3            ; valor a ser movido 
;        clrf   INDF
;	iorwf  INDF             ; mover o valor usando endereçamento indireto
;        goto   ConfirmCmd 
;
;CmdLoad
;; Ler conteúdo do endereço informado (banco,offset)
;	call   RecLoop          ; receber banco do endereço
;        movwf  Byte1            ; guarda banco em temp1 (comum a todos os bancos)
;        call   RecLoop          ; recebe offset do endereço
;        movwf  Byte2            ; guarda offset em temp2 (comum a todos os bancos)     
;       	; setar banco
; 	bcf    STATUS,IRP       ; inicialmente forçar banco 0
;	bcf    Byte2,7
;        movfw  Byte1
;	btfsc  Byte1,1          ; endereçamento indireto banco = bit 7 do status (IRP) e Bit 7 do FSR         
;        bsf    STATUS,IRP   
;	btfsc  Byte1,0
;        bsf    Byte2,7 
;	movfw  Byte2
;	movwf  FSR              ; offset para endereçamento indireto 
;        clrw
;	iorwf   INDF,w             ; valor a ser movido
;        banksel PIR1  
;	call   TxByte           ; mover o valor usando endereçamento indireto
;        goto   ConfirmCmd 
;
;
;CmdEco
;; Ecoar n bytes recebidos - guarda os bytes de 40h em diante, máximo 48 bytes até 6Fh
;	banksel PIR1
;	call   RecLoop          ; receber quantidade de bytes a guardar
;        movwf  Byte1            ; guarda qtd em Byte1 (comum a todos os bancos)
;        sublw  0x30		; se qtd maior que 48, qtd=48
;	btfsc  STATUS,C
;	goto   pulo1
;        movlw  0x30
;        movwf  Byte1
;pulo1   movfw  Byte1
;	movwf  Byte2            ; contador qtd 
;        movlw  0x40
;        movwf  FSR
;Prox    call   RecLoop
;        movwf  INDF
;        incf   FSR,F
;        decf   Byte2
;        btfss  STATUS,Z
;        goto   Prox
;		; transmitir os bytes recebidos
;        movfw  Byte1
;        movwf  Byte2
;  	movlw  0x40
;	movwf  FSR
;Prox2	movfw  INDF
;        call   TxByte
;	incf   FSR,F
;        decf   Byte2
;	btfss  STATUS,Z
;	goto   Prox2
;        goto   ConfirmCmd 
;
;CmdVersao
;; Retorna nome prog. e versão
;        movlw   mProduct
;        call    SendMessage
;        movlw   mVersion
;        call 	SendMessage
;        goto    WaitCom         ; Espera próximo comando
;
;
;
;
;
;
;ConfirmCmd
;        btfsc   Conf,0					;21/07/2003
;        goto    Backloop        ; não confirmar com OK   21/07/2003
;        movlw   mOK            	; MSG - OK.         
;	call    SendMessage
;Backloop
;        btfsc   Conf,0          ; se não confirmar, não limpar buffer (perde prox dados) 21/07/2003
;  ;;      call    LimpaBufferRX
;	goto 	WaitCom        	; Espera próximo comando
;
;
;
;
;;
;; Recebe byte da RS232
;; Só volta da rotina depois de receber alguma coisa
;;
;RecLoop
;        nop
;        btfss   PIR1,RCIF       ; Checar por qualquer dado em RX
;        goto 	RecLoop         ; Continuar até chegar algo em RX
;        movf    RCREG,W         ; Passa o dado de RX para W
;        return
;
;;
;; Envia byte para USART
;;
;TxByte
;        nop
;        btfss   PIR1,TXIF       ; Buffer TX vazio ?
;        goto    TxByte          ; Não - continua esperando
;
;        movwf   TXREG           ; Buffer TX vazio - enviar o caracter
;        return
;
;LimpaBufferRX
;	movf    RCREG,W        	; limpar buffer RX no banco 0
;        movf    RCREG,W
;	movf    RCREG,W
;        return
;
;
;
;;
;; Enviar mensagem para serial
;;
;SendMessage
;        movwf	LOOP            ; define o OFFSET da tabela
;SendMes2
;        movf    LOOP, W         ; Carrega ponteiro para caracteres
;        call    MsgTable        ; Retona 'W' = caracter ASCII para enviar ou ASCII 0 = EOM
;        movwf   TEMP            ; Salva o caracter recebido
;        ; Testa EOM
;        sublw   0x00
;
;	btfsc   STATUS, Z    	; EOM encontrado?
;        return                  ; sim
;
;        movf    TEMP, W         ; Recupera valor original
;	call    TxByte          ; Envia o caracter para serial
;        incf    LOOP, F         ; Ponteirar próximo caracter
;        goto    SendMes2        ; continua até acabar
;
;
;;******************************************************************
;
;
;;
;; Tabela de mensagens
;;
;MsgTable
;mError	equ     0x00
;        addwf   PCL, F
;        retlw   'E'             ; Erro
;        retlw   'r'
;        retlw   'r'
;        retlw   'o'
;        retlw   ' '
;        retlw   '!'
;	retlw   d'13'
;        retlw   d'10'
;        retlw   d'0'            ; EOM
;
;mOK   	equ     0x09
;        retlw   'O'             ; OK
;        retlw   'K'
;        retlw   '.'
;        retlw   d'13'
;        retlw   d'10'
;        retlw   d'0'            ; EOM
;
;mReady  equ     0x0F
;        retlw   'R'             ; Ready
;        retlw   'e'
;        retlw   'a'
;        retlw   'd'
;        retlw   'y'
;        retlw   '.'
;        retlw   d'13'
;        retlw   d'10'
;        retlw   d'0'            ; EOM
;
;mCRLF   equ     0x18            ; CRLF
;        retlw   d'13'
;        retlw   d'10'
;        retlw   d'0'            ; EOM
;
;mProduct equ    0x1B            ; Descrição
;        retlw   'R'
;        retlw   'e'
;        retlw   'm'
;        retlw   'o'
;        retlw   't'
;        retlw   'o'
;        retlw   ' '
;        retlw   'S'
;        retlw   'e'
;        retlw   'r'
;        retlw   'i'
;        retlw   'a'
;	retlw   'l'
;        retlw   ' '
;        retlw   'P'
;        retlw   'I'
;        retlw   'C'
;        retlw   d'13'
;        retlw   d'10'
;        retlw   d'0'            ; EOM
;
;mVersion equ    0x2F            ; Nr Versão
;        retlw   '1'
;        retlw   '.'
;        retlw   '1'
;        retlw   '/'
;        retlw   '0'
;        retlw   '3'
;        retlw   '.'
;        retlw   d'13'
;        retlw   d'10'
;        retlw   d'0'            ; EOM
;
;        end
;