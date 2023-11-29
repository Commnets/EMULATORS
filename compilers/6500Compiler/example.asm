; Simple example to compile a 6500 program
; (C) By Ignacio Cea

; The main program
*				= $0B00
MAIN:			LDA #$00
				STA $C000
FOREEVER:		JMP FOREEVER

; That's all
