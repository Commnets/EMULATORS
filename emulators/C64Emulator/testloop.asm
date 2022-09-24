; Set the color of the both background and foreground
; By Ignacio Cea

; MACROS
COLOR = $00
FOREGROUND = $D020
BACKGROUND = $D021

; Where the code starts
* = $C000

; Code
START:			LDA #COLOR				; First color is something
				BEQ NOCOND
				STA BACKGROUND
				STA FOREGROUND
				BNE NOCOND
NOCOND:			JMP START				; Infinite

; That's all
