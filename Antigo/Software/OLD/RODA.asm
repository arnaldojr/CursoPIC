;; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;; *                                                                         *
;; *                               EXEMPLO 0                                 *
;; *                           ENGENHARIA MAUÁ 		                        *
;; *            				  4ºANO ELETRÔNICA       				        *
;; *                                                                         *
;; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;; *     DATA : 05/09/2013                                                   *
;; *  	VERSÃO : 0.51                                                       *
;; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;
;; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;; *                             DESCRIÇÃO GERAL                             *
;; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;;
;;  TEMPLATE DE USO GERAL.
;;  ESTE MODELO PODE SER USADO PARA FUTUROS PROJETOS
;;
;;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;;*                     ARQUIVOS DE DEFINIÇÕES                      *
;;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#INCLUDE <P16F877A.INC>	; MICROCONTROLADOR UTILIZADO
;
CBLOCK 0X20
TEMPO0
TEMPO1
TEMPO2

 ENDC	

ATRASO_4MS
	MOVLW	.4			;1 SEGUNDO
	MOVWF	TEMPO0		;1
TEMPO_0
	MOVLW	.245		;1
	MOVWF	TEMPO1		;1
TEMPO_1
	MOVLW	.254		;1
	MOVWF 	TEMPO2		;1
TEMPO_2
	DECF 	TEMPO2,F	; 255
	BTFSS 	STATUS,Z	; 254 + 2
	GOTO 	TEMPO_2		; 508
	DECF 	TEMPO1,F	;1
	BTFSS	STATUS,Z	;1	
	GOTO 	TEMPO_1	
	DECF	TEMPO0,F
	BTFSS	STATUS,Z
	GOTO	TEMPO_0
	GOTO ATRASO_4MS
 
END