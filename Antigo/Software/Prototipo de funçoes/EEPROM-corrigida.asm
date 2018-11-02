LE_EEPROM
MOVF ADDR,W      ; carregando o endereço em W
BSF STATUS,RP1
BCF STATUS,RP0   ; banco 2
MOVWF EEADR      ; endereçando a EEPROM
BSF STATUS,RP0   ; banco 3
BCF EECON1,EEPGD ; selecionando a EEPROM e não
                 ; a memória de programa
BSF EECON1,RD    ; comando para a leitura
BCF STATUS,RP0   ; banco 2
MOVF EEDATA,W    ; carregando o dado lido em W
BCF STATUS,RP1
BCF STATUS,RP0   ; voltando ao banco 0
RETURN

GRAVA_EEPROM
BCF INTCON,GIE   ; desabilitando interrupções
MOVF ADDR,W      ; carregando o endereço em W
BSF STATUS,RP1   ; 
BCF STATUS,RP0   ; banco 2
MOVWF EEADR    ; endereçando a EEPROM
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
BSF INTCON,GIE ; reabilitando interrupções
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
BSF EECON1,EEPGD ; selecionando a memória de programa
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
MOVWF EEADR      ; parte baixa do endereço
BCF STATUS,RP1
MOVF E1,W
BSF STATUS,RP1
MOVWF EEADRH     ; parte alta do endereço
BCF STATUS,RP1
MOVF DATAL,W
BSF STATUS,RP1
MOVWF EEDATA     ; parte baixa do dado
BCF STATUS,RP1
MOVF DATAH,W
BSF STATUS,RP1
MOVWF EEDATH     ; parte alta do dado
BSF STATUS,RP0   ; banco 3
BSF EECON1,EEPGD ; selecionando a memória de programa
BCF INTCON,GIE   ; desabilitando interrupções
BSF EECON1,WREN  ; habilitando a escrita
MOVLW 0X55
MOVWF EECON2
MOVLW 0XAA
MOVWF EECON2     ; seqüência para escrita
BSF EECON2,WR    ; iniciando a escrita
BSF INTCON,GIE   ; reabilitando interrupções
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