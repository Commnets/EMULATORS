; Iterates the color in both foreground and basckground
; By Ignacio Cea

; MACROS
FOREGROUND		= $D020
BACKGROUND		= $D021
INICOLOR		= $00

; Where the code starts
*				= $C000

; Code
START:			LDX #INICOLOR			; Let's count! First color is black
LOOP:			STX BACKGROUND
				STX FOREGROUND
				INX
				CPX #$10
				BNE LOOP				; From colo 0 to 15...
				JMP START				; Jump to the beginning and starts back

; That's all
