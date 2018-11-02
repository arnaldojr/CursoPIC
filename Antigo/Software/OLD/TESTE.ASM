
#INCLUDE<P16f877a.inc>	
VAR1    equ     20
VAR2    equ     21
VAR3    equ     22
CONT    equ     23
        org     0
        goto    inicio

inicio
BSF 	STATUS,RP0
BCF  	STATUS,RP1		;BANK1
MOVLW	B'00000000'
MOVWF	TRISD
MOVLW	B'00001111'
MOVWF	TRISB
BCF 	STATUS,RP0
BCF 	STATUS,RP1		;BANK0
CLRF	PORTB
CLRF	PORTD

comeco  movlw   B'00000110'
        movwf   PORTD
		movlw   B'10000000'
		movwf   PORTB
;		call	tempo
	;	movlw   B'00000000'
;		movwf   PORTB
;		call	tempo
        goto    comeco




;
;tempo   movlw   0x03
;        movwf   VAR1
;temp1   movlw   0x0f
;        movwf   VAR2
;temp2   movlw   0xff
;        movwf   VAR3
;        decfsz  VAR3,F
;        goto    $-1
;        decfsz  VAR2,F
;        goto    temp2
;        decfsz  VAR1,F
;        goto    temp1
;        return
;
;
;
	
END ; - directiva "END" fim do programa

;	  	 ___a___
;		|		|
;	   f|		|b
;		|		|
;		|___g___|
;		|		|
;		|		|
;	   e|		|c
;		|___d___|
;				.h
;RD0 = a
;RD1 = b
;RD2 = c
;RD3 = d
;RD4 = e
;RD5 = f
;RD6 = g
;RD7 = h
;DIG1 = B'00000110'
;DIG2 = B'01011011'
;DIG3 = B'01001111'
;DIG4 = B'01100110'
;DIG5 = B'01101101'
;DIG6 = B'01111101'
;DIG7 = B'01000111'
;DIG8 = B'01111111'
;DIG9 = B'01101111'
;DIG0 = B'00111111'