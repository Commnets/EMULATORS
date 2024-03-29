; Iterates the color only in the foreground, and mantains the other one constant
; By Ignacio Cea

; MACROS
FOREGROUND		= $D020
BACKGROUND		= $D021
INICOLOR		= $00

; Where the code starts
*				= $C000

; Code
START:			LDX #INICOLOR			; Let's count! First color is black
				STX BACKGROUND
LOOP:			STX FOREGROUND
				INX
				LDY #$80
DELAY:			NOP
				INY
				BNE DELAY
				CPX #$10
				BNE LOOP				; From color 0 to 15...
				JMP START				; Jump to the beginning and starts back

; That's all
