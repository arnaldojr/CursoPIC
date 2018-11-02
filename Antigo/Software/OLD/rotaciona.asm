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
;  TEMPLATE DE USO GERAL.
;  ESTE MODELO PODE SER USADO PARA FUTUROS PROJETOS
;
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     ARQUIVOS DE DEFINIÇÕES                      *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#INCLUDE <P16F877A.INC>	; MICROCONTROLADOR UTILIZADO

	__CONFIG _BOREN_ON & _CP_OFF & _PWRTE_ON & _WDT_OFF & _LVP_OFF & _HS_OSC
	
	
	
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                    PAGINAÇÃO DE MEMÓRIA                         *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;DEFINIÇÃO DE COMANDOS DE USUÁRIO PARA ALTERAÇÃO DA PÁGINA DE MEMÓRIA
#DEFINE	BANK0	BCF STATUS,RP0	;SETA BANK 0 DE MEMÓRIA
#DEFINE	BANK1	BSF STATUS,RP0	;SETA BANK 1 DE MAMÓRIA





;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                         VARIÁVEIS                               *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DOS NOMES E ENDEREÇOS DE TODAS AS VARIÁVEIS UTILIZADAS 
; PELO SISTEMA
	CBLOCK	0x20	;ENDEREÇO INICIAL DA MEMÓRIA DE
					;USUÁRIO
		W_TEMP		;REGISTRADORES TEMPORÁRIOS PARA USO
		STATUS_TEMP	;JUNTO ÀS INTERRUPÇÕES
		
		TEMPO1		;DELAY
		TEMPO2		;DELAY
		
		
					;NOVAS VARIÁVEIS

	ENDC			;FIM DO BLOCO DE MEMÓRIA
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                        FLAGS INTERNOS                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODOS OS FLAGS UTILIZADOS PELO SISTEMA






;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                         CONSTANTES                              *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODAS AS CONSTANTES UTILIZADAS PELO SISTEMA
;FILTRO_TECLA	EQU	.200		; FILTRO P/ EVITAR RUIDOS DOS BOTÕES
;TURBO_TECLA	EQU	.70			; TEMPORIZADOR P/ TURBO DAS TECLAS







;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                           ENTRADAS                              *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODOS OS PINOS QUE SERÃO UTILIZADOS COMO ENTRADA
; RECOMENDAMOS TAMBÉM COMENTAR O SIGNIFICADO DE SEUS ESTADOS (0 E 1)

#DEFINE	BOT1	PORTB,0	;0->APERTADO
						;1->SOLTO	
#DEFINE	BOT2	PORTB,1	;0->APERTADO
						;1->SOLTO
#DEFINE	BOT3	PORTB,2	;0->APERTADO
						;1->SOLTO
#DEFINE	BOT4	PORTB,3	;0->APERTADO
						;1->SOLTO


;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                           SAÍDAS                                *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; DEFINIÇÃO DE TODOS OS PINOS QUE SERÃO UTILIZADOS COMO SAÍDA
; RECOMENDAMOS TAMBÉM COMENTAR O SIGNIFICADO DE SEUS ESTADOS (0 E 1)

#DEFINE	LED1	PORTB,0	;0->APAGADO
						;1->ACESO	
#DEFINE	LED2	PORTB,1	;0->APAGADO
						;1->ACESO
#DEFINE	LED3	PORTB,2	;0->APAGADO
						;1->ACESO
#DEFINE	LED4	PORTB,3	;0->APAGADO
						;1->ACESO

#DEFINE	DISP1	PORTB,7	;0->DESLIGADO
						;1->LIGADO
#DEFINE	DISP2	PORTB,6	;0->DESLIGADO
						;1->LIGADO
#DEFINE	DISP3	PORTB,5	;0->DESLIGADO
						;1->LIGADO
#DEFINE	DISP4	PORTB,4	;0->DESLIGADO
						;1->LIGADO

#DEFINE VENT	PORTC,1	;0->DESLIGADO
						;1->LIGADO

#DEFINE BUZZER	PORTA,5	;0->DESLIGADO
						;1->LIGADO


;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                       VETOR DE RESET                            *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ORG		0x00			;ENDEREÇO INICIAL DE PROCESSAMENTO
	GOTO	CONFIG_
	

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                    INÍCIO DA INTERRUPÇÃO                        *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; ENDEREÇO DE DESVIO DAS INTERRUPÇÕES. A PRIMEIRA TAREFA É SALVAR OS
; VALORES DE "W" E "STATUS" PARA RECUPERAÇÃO FUTURA

	ORG		0x04			;ENDEREÇO INICIAL DA INTERRUPÇÃO
	MOVWF	W_TEMP		;COPIA W PARA W_TEMP
	SWAPF	STATUS,W
	MOVWF	STATUS_TEMP	;COPIA STATUS PARA STATUS_TEMP

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                    ROTINA DE INTERRUPÇÃO                        *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; AQUI SERÁ ESCRITA AS ROTINAS DE RECONHECIMENTO E TRATAMENTO DAS
; INTERRUPÇÕES

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                 ROTINA DE SAÍDA DA INTERRUPÇÃO                  *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; OS VALORES DE "W" E "STATUS" DEVEM SER RECUPERADOS ANTES DE 
; RETORNAR DA INTERRUPÇÃO

SAI_INT
	SWAPF	STATUS_TEMP,W
	MOVWF	STATUS		;MOVE STATUS_TEMP PARA STATUS
	SWAPF	W_TEMP,F
	SWAPF	W_TEMP,W	;MOVE W_TEMP PARA W
	RETFIE

	
	
	
	
	
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*	            	 ROTINAS E SUBROTINAS                          *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; CADA ROTINA OU SUBROTINA DEVE POSSUIR A DESCRIÇÃO DE FUNCIONAMENTO
; E UM NOME COERENTE ÀS SUAS FUNÇÕES.

DELAY_M						;;DELAY

	MOVLW	.255
	MOVWF	TEMPO1

TEMPO_1	
	MOVLW	.255
	MOVWF	TEMPO2
TEMPO_2
	DECF	TEMPO2
	BTFSS	STATUS,Z	
	GOTO	TEMPO_2
	DECF	TEMPO1
	BTFSS	STATUS,Z	
	GOTO	TEMPO_1
	RETURN




;FUNÇÃO ROTACIONA 
FUNCAO	

;ROTACIONA ESQUERDA

	MOVLW	B'00000001'
	MOVWF	PORTD
	RODA1
		BCF		STATUS,C
		CALL	DELAY_M
		RLF		PORTD,F
		BTFSS	STATUS,C	
		GOTO	RODA1
		CALL	DELAY_M
		MOVLW	B'10000000'
		MOVWF	PORTD

;RODAR A DIREITA

	RODA2
		BCF		STATUS,C
		CALL	DELAY_M
		RRF		PORTD,F
		BTFSS	STATUS,C	
		GOTO	RODA2
		CALL	DELAY_M

RETURN



;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     CONFIGURAÇÃO INICIAL DO PROGRAMA            *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	
CONFIG_
	CLRF	PORTA			; GARANTE TODAS AS SAIDAS EM ZERO
	CLRF	PORTB
	CLRF	PORTC
	CLRF	PORTD
	CLRF	PORTE
	
	BANK1					;ALTERA PARA O BANCO 1
	
	MOVLW	B'11011111'
	MOVWF	TRISA			; CONFIGURA I/O DO PORTA

	MOVLW	B'00001111'
	MOVWF	TRISB			; CONFIGURA I/O DO PORTB

	MOVLW	B'11111001'
	MOVWF	TRISC			; CONFIGURA I/O DO PORTC

	MOVLW	B'00000000'
	MOVWF	TRISD			; CONFIGURA I/O DO PORTD

	MOVLW	B'00000000'
	MOVWF	TRISE			; CONFIGURA I/O DO PORTE

	MOVLW	B'11000111'
	MOVWF	OPTION_REG		; CONFIGURA OPTIONS
							; PULL-UPs DESABILITADOS
							; INTER. NA BORDA DE SUBIDA DO RB0
							; TIMER0 INCREM. PELO CICLO DE M?QUINA
							; WDT   - 1:128
							; TIMER - 1:1
					
	MOVLW	B'00000000'		
	MOVWF	INTCON			; CONFIGURA INTERRUPÇÕES
							; 
	MOVLW	B'00000000'		
	MOVWF	PIE1			; CONFIGURA INTERRUP??ES
							; DESABILITA AS INT. DE TMR1 E TMR2

	MOVLW	B'00000111'
	MOVWF	ADCON1			; CONFIGURA CONVERSOR A/D
							; CONFIGURA PORTA E PORTE COMO I/O DIGITAL
							
	MOVLW	B'00000111'
	MOVWF	CMCON		;DEFINE O MODO DE OPERAÇÃO DO COMPARADOR ANALÓGICO


	BANK0					;ALTERA PARA O BANCO 0
	GOTO    MAIN



;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     INICIALIZAÇÃO DAS VARIÁVEIS                 *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                     ROTINA PRINCIPAL                            *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
MAIN

	BSF		DISP1
	CALL    FUNCAO
	BCF	    DISP1

	BSF     DISP2
	CALL    FUNCAO
	BCF     DISP2

	BSF		DISP3
	CALL    FUNCAO
	BCF     DISP3

	BSF     DISP4
	CALL    FUNCAO
	BCF     DISP4

	GOTO MAIN

;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;*                       FIM DO PROGRAMA                           *
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	END

TMP_TX EQU H'20' ;Value to hold received value

ORG 0x00 ;Start vector
GOTO main
ORG 0x04 ;Interrupt vector
GOTO interrupt

main:
;---Use PORTA as I/O
CLRF PORTA
MOVLW D'7' ;Use PORTA as I/O
MOVWF CMCON ;

BSF STATUS,RP0 ;Switch to BANK1
CLRF TRISD ;Set all pins of PORTA as output
;BSF TRISB,4 ;Set PORTB.4 as input
BCF OPTION_REG,7

;---Configure peripheral interrupts
MOVLW B'00100000' ;Disable all peripheral interrupts except receiver
MOVWF PIE1 ;Peripheral interrupt enable/disable

;---Configure general interrupts
MOVLW B'01000000' ;Disable all interrupts except peripheral
MOVWF INTCON ;Interrupt control register

;---Configure SPBRG for desired baud rate
MOVLW D'129' ;We will use 9600 
MOVWF SPBRG ;baud at 20MHz

;---Configure TXSTA
MOVLW B'00100100' ;Configure TXSTA as :
MOVWF TXSTA ;
;8 bit transmission - 6.bit
;Transmit enabled - 5.bit
;Asynchronous mode - 4.bit
;Enable high speed baud rate - 2.bit

BCF STATUS,RP0 ;Switch to BANK0

MOVLW B'10000000' ;Enable serial port
MOVWF RCSTA ;Receive status reg

CLRF TMP_TX

BSF INTCON,7 ;Enable all unmasked interrupts
BSF RCSTA,4 ;Enable USART receive

MAIN_LOOP: ;Continous loop
;BTFSC PORTB,4 ;Check if the button is pressed
GOTO MAIN_LOOP ;If not goto continous loop
MOVF TMP_TX,W ;Load TMP_TX  
MOVWF TXREG ;to TXREG
;We load TMP_TX on the interrupt routine,
;when an information received from RX.
GOTO MAIN_LOOP ;Continous loop


interrupt:
BCF INTCON,7 ;Disable all interrupts
BTFSS PIR1,5 ;Check if the RCIF flag is set
GOTO quit_int ;If not return back to the main loop
MOVF RCREG,W ;Move the received byte to W
MOVWF PORTA ;Move W to PORTA
MOVWF TMP_TX
BCF PIR1,5
quit_int:
RETFIE

END

