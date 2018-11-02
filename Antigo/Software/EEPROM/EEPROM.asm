 
#INCLUDE<P16F877A.INC>
 
ADDR EQU  20H
DADO EQU  21H
E1  EQU  22H
E2  EQU  23H
DATAL EQU  24H
DATAH EQU  25H

ORG 0X00

GOTO INICIO   ; PULA PARA INICIO
 
INICIO
BSF  STATUS,RP0
BCF  STATUS,RP1
MOVLW 0XFF       ; PORTAS
MOVWF TRISA
MOVLW 0X0F
MOVWF TRISB
MOVLW 0B11111001
MOVWF TRISC
MOVLW 0X00
MOVWF TRISD
MOVWF TRISE
BCF  STATUS,RP0
MOVLW .0
MOVWF ADDR
CALL LE_EEPROM   ; LENDO A EEPROM
MOVLW .2
MOVWF ADDR
MOVLW 0XAA
MOVWF DADO
CALL GRAVA_EEPROM   ; GRAVANDO O VALOR AAH NO ENDERE�O 02H DA EEPROM
MOVLW 0X10
MOVWF E1
MOVLW 0X00
MOVWF E2
CALL LE_PROGRAMA ; LENDO O ENDERE�O 0X1000 DA MEM. DE PROGRAMA
 
LOOP

GOTO LOOP
 
 

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
MOVF ADDR,W      ; carregando o endere�o em W
BSF STATUS,RP1   ; banco 3
BCF STATUS,RP0   ; banco 2
MOVWF EEADR    ; endere�ando a EEPROM
BCF STATUS,RP1   ; banco 0
MOVF DADO,W      ; carregando o dado a ser gravado em W
BSF STATUS,RP1   ; banco 2
MOVWF EEDATA   ; carregando W em EEDATA
BSF STATUS,RP0   ; banco 3
BCF EECON1,EEPGD ; selecionando a EEPROM
BSF EECON1,WREN  ; habilitando a escrita
BCF INTCON,GIE   ; desabilitando interrup��es
MOVLW 0X55
MOVWF EECON2
MOVLW 0XAA
MOVWF EECON2   ; comandos para a escrita em EECON2
BSF EECON1,WR    ; iniciando a escrita
BSF INTCON,GIE ; reabilitando interrup��es
BCF EECON1,WREN  ; desabilitando a escrita
BSF STATUS,RP0
BSF STATUS,RP1   ; banco 3
BTFSC EECON1,WR
GOTO $-1         ; espera a �ltima escrita terminar
BCF STATUS,RP0
BCF STATUS,RP1   ; banco 3
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
 
 
ORG 0X1000   ; tabela da mem. de programa
 
DW 0X34F1
DW 0X3483

END



