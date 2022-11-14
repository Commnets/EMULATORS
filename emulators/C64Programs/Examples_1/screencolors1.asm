; Set the color of the both background and foreground
; By Ignacio Cea

; MACROS
COLOR1	   = 2
COLOR2	   = COLOR1 + 2
FOREGROUND = $D020
BACKGROUND = $D021

; Where the code starts
* = $C000

; Code
START:			LDA #COLOR2				; First color is something
				STA BACKGROUND
				STA FOREGROUND
				BNE START				; Infinite

; That's all
