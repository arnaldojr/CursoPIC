;RELOGIO+LCD+BOT�O+TIMER0+EEPROM
#INCLUDE <P16F877A.INC>
__CONFIG  _BODEN_ON & _CP_OFF & _PWRTE_ON & _WDT_OFF & _LVP_OFF & _XT_OSC

#DEFINE BANK0 BCF STATUS,RP0 ; SETA BANK 0 DE MEMORIA
#DEFINE BANK1 BSF STATUS,RP0 ; SETA BANK 1 DE MEMORIA

CBLOCK 0X20
W_TEMP
STATUS_TEMP
UMSEG
REG1
CONT
SEG
DSEG
MIN
DMIN
HR
DHR
TEMPO1
TEMPO2
ADDR
DADO
E1 
E2 
DATAL
DATAH
ENDC

ORG     0X00
GOTO INICIO 
;----------------------------INICIO DA INTERRUP��O-----------
ORG     0X04  ;VETOR DE INTERRUP��O
MOVWF W_TEMP
SWAPF STATUS,W
MOVWF STATUS_TEMP
MOVLW .217
MOVWF TMR0

INCF UMSEG,F
MOVLW .100
SUBWF UMSEG,W
BTFSS STATUS,Z ;DEU 1SEG?
GOTO VOLTA  ;N�O DEU 1SEG
CLRF UMSEG  ;SIM DEU 1SEG
    
INCF SEG,F   ;TESTA SEG 
MOVLW .10
SUBWF SEG,W
BTFSS STATUS,Z   
GOTO VOLTA
CLRF SEG
;CALL EXIBESEG

INCF DSEG,F  ;TESTA DSEG
MOVLW .6
SUBWF DSEG,W
BTFSS STATUS,Z   
GOTO VOLTA
CLRF DSEG
;CALL EXIBEDSEG

INCF MIN,F   ;TESTA MIN 
MOVLW .10
SUBWF MIN,W
BTFSS STATUS,Z   
GOTO VOLTA
CLRF MIN
;CALL EXIBEMIN

INCF DMIN,F   ;TESTA DMIN 
MOVLW .6
SUBWF DMIN,W
BTFSS STATUS,Z   
GOTO VOLTA
CLRF DMIN
;CALL EXIBEDMIN


MOVLW .2    ;TESTA HR
SUBWF DHR,W
BTFSC STATUS,Z   
GOTO ALTERAHR4
INCF HR,F    
MOVLW .10
SUBWF HR,W
BTFSS STATUS,Z   
GOTO VOLTA
CLRF HR
;CALL EXIBEHR

INCF DHR,F   ;TESTA DHR 
;CALL EXIBEDHR
GOTO VOLTA

ALTERAHR
 INCF HR,F
 GOTO VOLTA

ALTERAHR4
 MOVLW .3
 SUBWF HR,W
 BTFSS STATUS,Z
   
 GOTO ALTERAHR
 CLRF DHR
 CLRF HR
; CALL EXIBEHR
; CALL EXIBEDHR 
 GOTO VOLTA

ALTERADHR
 GOTO VOLTA

VOLTA
SWAPF STATUS_TEMP,W
MOVWF STATUS
SWAPF W_TEMP,F
SWAPF W_TEMP,W

BCF  INTCON,2
RETFIE


 
;----------------FIM DA INTERRUP��O-----------------------

;--------------------INICIO SUB ROTINAS------------------------
 DECODER
 ADDWF PCL,F
 RETLW '0'
 RETLW '1'
 RETLW '2'
 RETLW '3'
 RETLW '4'
 RETLW '5'
 RETLW '6'
 RETLW '7'
 RETLW '8'
 RETLW '9'
;-----------CONFIGURA��O-----------------
CONFIGURA
BANK1      ;ALTERA PARA BANCO 1
BCF STATUS,RP1;
MOVLW 0XFF       ; CONFIGURA I/O DO PORTA
MOVWF TRISA 
MOVLW 0X0F       ; CONFIGURA I/O DO PORTB
MOVWF TRISB 
MOVLW B'11111001'    ; CONFIGURA I/O DO PORTC
MOVWF TRISC 
MOVLW B'00000000'    ; CONFIGURA I/O DO PORTD
MOVWF TRISD 
MOVWF TRISE 
MOVLW B'10000111'    ; CONFIGURA OPTION REG
MOVWF OPTION_REG
MOVLW .7
MOVWF ADCON1
MOVWF CMCON
MOVLW B'10100000'    
MOVWF INTCON   ; HABILITA A INTERRUP��O DO TIMER0
BANK0  
MOVLW .217
MOVWF TMR0
RETURN
;-----------FIM DA CONFIGURA��O-----------------
;-------------------ATRASOS ---------------------

 ATRASO_1MS
 MOVLW .255
 MOVWF REG1
 VOLTA1
 DECF REG1,F
 BTFSS STATUS,Z
 GOTO VOLTA1
 RETURN

 ATRASO_1S
 MOVLW .255
 MOVWF CONT
 VOLT
 CALL ATRASO_1MS 
 DECF CONT,F
 BTFSS STATUS,Z
 GOTO VOLT
 RETURN

ATRASO_100MS
MOVLW .100
MOVWF TEMPO1
TEMPO_1
MOVLW .255
MOVWF TEMPO2
TEMPO_2
DECF TEMPO2,F
BTFSS STATUS,Z
GOTO TEMPO_2
DECF TEMPO1,F
BTFSS STATUS,Z
GOTO TEMPO_1
RETURN

;------------------------------FIM DOS ATRASOS--------------------------------
;-----------------------SUB ROTINA DO DISPLAY--------------------

 INICIA_LCD
 MOVLW 0X38
 CALL ESCREVE_COMANDO
 CALL ATRASO_1MS
 CALL ATRASO_1MS
 CALL ATRASO_1MS
 MOVLW 0X38
 CALL ESCREVE_COMANDO
 MOVLW 0X0C
 CALL ESCREVE_COMANDO
 MOVLW 0X06
 CALL ESCREVE_COMANDO
 MOVLW 0X01
 CALL ESCREVE_COMANDO
 CALL ATRASO_1MS
 RETURN

 ESCREVE_DADO ;ESCREVE CARACTERES NO LCD 
 BSF PORTE,0 ;SINAL DE DADO
 NOP
 MOVWF PORTD ;INFORMA��O NA PORTA D DE DAOD
 NOP
 NOP
 BSF PORTE,1 ;PULSO DE ENABLE
 NOP
 NOP
 BCF PORTE,1 
 CALL ATRASO_1MS ;ATRSO DE 1ms
 RETURN

 ESCREVE_COMANDO ;ENVIA COMANDO AO LCD
 BCF PORTE,0 ;SINAL DE COMANDO
 NOP
 MOVWF PORTD ;INFORMA��O NA PORTA D DE DAOD
 NOP
 NOP
 BSF PORTE,1 ;PULSO DE ENABLE
 NOP
 NOP
 BCF PORTE,1 
 CALL ATRASO_1MS ;ATRSO DE 1ms
 RETURN
;--------------------------FIM DO DISPLAY------------------------------------
;------------EXIBE LCD---------------
EXIBESEG

MOVLW  0X8C    ;CURSOR DO LCD NAPRIMEIRA LINHA PRIMEIRA COLUNA
CALL  ESCREVE_COMANDO
MOVF SEG,W
CALL DECODER
CALL  ESCREVE_DADO
RETURN

EXIBEDSEG

MOVLW  0X8B    ;CURSOR DO LCD NAPRIMEIRA LINHA PRIMEIRA COLUNA
CALL  ESCREVE_COMANDO
MOVF DSEG,W
CALL DECODER
CALL  ESCREVE_DADO
RETURN

EXIBEMIN

MOVLW  0X89    ;CURSOR DO LCD NAPRIMEIRA LINHA PRIMEIRA COLUNA
CALL  ESCREVE_COMANDO
MOVF MIN,W
CALL DECODER
CALL  ESCREVE_DADO
RETURN

EXIBEDMIN

MOVLW  0X88    ;CURSOR DO LCD NAPRIMEIRA LINHA PRIMEIRA COLUNA
CALL  ESCREVE_COMANDO
MOVF DMIN,W
CALL DECODER
CALL  ESCREVE_DADO
RETURN

EXIBEHR

MOVLW  0X86    ;CURSOR DO LCD NAPRIMEIRA LINHA PRIMEIRA COLUNA
CALL  ESCREVE_COMANDO
MOVF HR,W
CALL DECODER
CALL  ESCREVE_DADO
RETURN

EXIBEDHR

MOVLW  0X85    ;CURSOR DO LCD NAPRIMEIRA LINHA PRIMEIRA COLUNA
CALL  ESCREVE_COMANDO
MOVF DHR,W
CALL DECODER
CALL  ESCREVE_DADO
RETURN


;----------------TRATAMENTO BOTOES
ZERA_SEG
CALL ATRASO_100MS
BTFSC PORTB,0  ; BOT0 ESTA APERTADO?
RETURN
BCF  INTCON,GIE
CLRF DSEG
CLRF SEG
BSF  INTCON,GIE
RETURN
 
INCHR
CALL ATRASO_100MS
BTFSC PORTB,3  ; BOT0 ESTA APERTADO?
RETURN    ; N�O ESTA APERTADO

BCF  INTCON,GIE
SWAPF DHR,W
IORWF HR,W
SUBLW 0X23
BTFSS STATUS,Z
GOTO $+5
CLRF HR
CLRF DHR
BSF  INTCON,GIE
RETURN

INCF HR,F
MOVLW .10
SUBWF HR,W
BTFSS STATUS,Z
GOTO $+3
CLRF HR
INCF DHR,F
BSF  INTCON,GIE
RETURN

INCDMIN
CALL ATRASO_100MS
BTFSC PORTB,2  ; BOT0 ESTA APERTADO?
RETURN    ; N�O ESTA APERTADO
BCF  INTCON,GIE
INCF DMIN,F
MOVLW .6
SUBWF DMIN,W
BTFSS STATUS,Z
GOTO  $+2
CLRF DMIN
BSF  INTCON,GIE
RETURN
 
INCMIN
CALL ATRASO_100MS
BTFSC PORTB,1  ; BOT0 ESTA APERTADO?
RETURN    ; N�O ESTA APERTADO
BCF  INTCON,GIE
INCF MIN,F
MOVLW .10
SUBWF MIN,W
BTFSS STATUS,Z
GOTO  $+2
CLRF MIN
BSF  INTCON,GIE
RETURN
 
;-----------------------FIM BOT�ES------------------

;--------------------EEPROM---------------------------

LE_EEPROM
MOVF ADDR,W      ; carregando o endere�o em W
BSF STATUS,RP1
BCF STATUS,RP0   ; banco 2
MOVWF EEADR      ; endere�ando a EEPROM
BSF STATUS,RP0   ; banco 3
BCF EECON1,EEPGD ; selecionando a EEPROM e n�o
                 ; a mem�ria de programa
BSF EECON1,RD    ; comando para a leitura
BCF STATUS,RP0   ; banco 2
MOVF EEDATA,W    ; carregando o dado lido em W
BCF STATUS,RP1
BCF STATUS,RP0   ; voltando ao banco 0
RETURN

GRAVA_EEPROM
BCF INTCON,GIE   ; desabilitando interrup��es
MOVF ADDR,W      ; carregando o endere�o em W
BSF STATUS,RP1   ; 
BCF STATUS,RP0   ; banco 2
MOVWF EEADR    ; endere�ando a EEPROM
BCF STATUS,RP1   ; banco 0
MOVF DADO,W      ; carregando o dado a ser gravado em W
BSF STATUS,RP1   ; banco 2
MOVWF EEDATA   ; carregando W em EEDATA
BSF STATUS,RP0   ; banco 3
BCF EECON1,EEPGD ; selecionando a EEPROM
BSF EECON1,WREN  ; habilitando a escrita
MOVLW 0X55
MOVWF EECON2
MOVLW 0XAA
MOVWF EECON2   ; comandos para a escrita em EECON2
BSF EECON1,WR    ; iniciando a escrita
BCF EECON1,WREN  ; desabilitando a escrita
BCF STATUS,RP0
BCF STATUS,RP1   ; banco 0
BSF INTCON,GIE ; reabilitando interrup��es
CALL ATRASO_1MS
CALL ATRASO_1MS
CALL ATRASO_1MS
CALL ATRASO_1MS
CALL ATRASO_1MS
RETURN

LE_PROGRAMA
MOVF E1,W
BSF STATUS,RP1
MOVWF EEADRH
BCF STATUS,RP1
MOVF E2,W
BSF STATUS,RP1
MOVWF EEADR 
BSF STATUS,RP0   ; banco 3
BSF EECON1,EEPGD ; selecionando a mem�ria de programa
BSF EECON1,RD    ; iniciando a leitura
NOP
NOP              ; tempo para a leitura
BCF STATUS,RP0   ; banco 2
MOVF EEDATA,W
BCF STATUS,RP1   ; banco 0
MOVWF DATAL
BSF STATUS,RP1
MOVF EEDATH,W
BCF STATUS,RP1    ; banco 0
MOVWF DATAH     
RETURN

GRAVA_PROGRAMA

MOVF E2,W
BSF STATUS,RP1
MOVWF EEADR      ; parte baixa do endere�o
BCF STATUS,RP1
MOVF E1,W
BSF STATUS,RP1
MOVWF EEADRH     ; parte alta do endere�o
BCF STATUS,RP1
MOVF DATAL,W
BSF STATUS,RP1
MOVWF EEDATA     ; parte baixa do dado
BCF STATUS,RP1
MOVF DATAH,W
BSF STATUS,RP1
MOVWF EEDATH     ; parte alta do dado
BSF STATUS,RP0   ; banco 3
BSF EECON1,EEPGD ; selecionando a mem�ria de programa
BCF INTCON,GIE   ; desabilitando interrup��es
BSF EECON1,WREN  ; habilitando a escrita
MOVLW 0X55
MOVWF EECON2
MOVLW 0XAA
MOVWF EECON2     ; seq��ncia para escrita
BSF EECON2,WR    ; iniciando a escrita
BSF INTCON,GIE   ; reabilitando interrup��es
NOP
NOP
BCF EECON1,WREN  ; desabilitando a escrita
BCF STATUS,RP0
BCF STATUS,RP1   ; banco 0
RETURN

;ORG 0X1000   ; tabela da mem. de programa
;
;DW 0X34F1
;DW 0X3483

;------------------------------FIM EEPROM-----------------------------

;-----------------------PROGRAMA PRINCIPAL------------------------------------
INICIO

 CALL CONFIGURA
 CLRF PORTB 
MOVLW .0
MOVWF ADDR
CALL LE_EEPROM   ; LENDO A EEPROM
MOVWF DHR

MOVLW .1
MOVWF ADDR
CALL LE_EEPROM   ; LENDO A EEPROM
MOVWF HR

MOVLW .2
MOVWF ADDR
CALL LE_EEPROM   ; LENDO A EEPROM
MOVWF DMIN

MOVLW .3
MOVWF ADDR
CALL LE_EEPROM   ; LENDO A EEPROM
MOVWF MIN

MOVLW .4
MOVWF ADDR
CALL LE_EEPROM   ; LENDO A EEPROM
MOVWF DSEG

MOVLW .5
MOVWF ADDR
CALL LE_EEPROM   ; LENDO A EEPROM
MOVWF  SEG


 CALL INICIA_LCD
 MOVLW 0X80    ;CURSOR DO LCD NAPRIMEIRA LINHA PRIMEIRA COLUNA
 CALL ESCREVE_COMANDO
 MOVLW 'H'
 CALL ESCREVE_DADO
 MOVLW 'O'
 CALL ESCREVE_DADO
 MOVLW 'R'
 CALL ESCREVE_DADO
 MOVLW 'A'
 CALL ESCREVE_DADO
 MOVLW ':'
 CALL ESCREVE_DADO
 MOVLW 0X87    
 CALL ESCREVE_COMANDO
 MOVLW ':'
 CALL ESCREVE_DADO
 MOVLW 0X8A    
 CALL ESCREVE_COMANDO
 MOVLW ':'
 CALL ESCREVE_DADO

LOOP

CALL EXIBEDHR
CALL EXIBEHR
CALL EXIBEDMIN
CALL EXIBEMIN
CALL EXIBEDSEG
CALL EXIBESEG
MOVLW 0X8D    
CALL ESCREVE_COMANDO
MOVLW ' '
CALL ESCREVE_DADO
MOVLW 0X87    
CALL ESCREVE_COMANDO
MOVLW ':'
CALL ESCREVE_DADO
MOVLW .0
MOVWF ADDR
MOVF DHR,W
MOVWF DADO
CALL GRAVA_EEPROM   ; GRAVANDO O VALOR AAH NO ENDERE�O 00H DA EEPROM
MOVLW .1
MOVWF ADDR
MOVF HR,W
MOVWF DADO
CALL GRAVA_EEPROM   ; GRAVANDO O VALOR AAH NO ENDERE�O 00H DA EEPROM
MOVLW .2
MOVWF ADDR
MOVF DMIN,W
MOVWF DADO
CALL GRAVA_EEPROM   ; GRAVANDO O VALOR AAH NO ENDERE�O 00H DA EEPROM
MOVLW .3
MOVWF ADDR
MOVF MIN,W
MOVWF DADO
CALL GRAVA_EEPROM   ; GRAVANDO O VALOR AAH NO ENDERE�O 00H DA EEPROM
MOVLW .4
MOVWF ADDR
MOVF DSEG,W
MOVWF DADO
CALL GRAVA_EEPROM   ; GRAVANDO O VALOR AAH NO ENDERE�O 00H DA EEPROM
MOVLW .5
MOVWF ADDR
MOVF SEG,W
MOVWF DADO
CALL GRAVA_EEPROM   ; GRAVANDO O VALOR AAH NO ENDERE�O 00H DA EEPROM


 BTFSC PORTB,0  ; BOT0 ESTA APERTADO?
 GOTO BOT1  ; N�O ESTA APERTADO
 CALL ZERA_SEG ; ESTA APERTADO
BOT1 
 BTFSC PORTB,3  ; BOT0 ESTA APERTADO?
 GOTO BOT2  ; N�O ESTA APERTADO
 CALL INCHR  ; ESTA APERTADO
BOT2 
 BTFSC PORTB,2  ; BOT0 ESTA APERTADO?
 GOTO BOT3  ; N�O ESTA APERTADO
 CALL INCDMIN  ; ESTA APERTADO
BOT3 
 BTFSC PORTB,1  ; BOT0 ESTA APERTADO?
 GOTO LOOP  ; N�O ESTA APERTADO
 CALL INCMIN   ; ESTA APERTADO
 GOTO LOOP

;;MOVLW 0X10
;MOVWF E1
;MOVLW 0X00
;MOVWF E2
;CALL LE_PROGRAMA ; LENDO O ENDERE�O 0X1000 DA MEM. DE PROGRAMA

END