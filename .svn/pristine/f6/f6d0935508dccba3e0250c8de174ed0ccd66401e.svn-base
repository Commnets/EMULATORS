; Set the color of both foreground and background from 0 to 15
; By Ignacio Cea

; MACROS
FOREGROUND = $D020
BACKGROUND = $D021

; Where the code starts
* = $C000

; Code
START:			LDX #$00				; Let's count! First color is black
LOOP:			STX BACKGROUND
				STX FOREGROUND
				INX
				CPX #$10
				BNE LOOP				; From colo 0 to 15...

; That's all
