; Set the color of the both background and foreground
; By Ignacio Cea

; MACROS
FOREGROUND = $D020
BACKGROUND = $D021

; Where the code starts
* = $C000

; Code
START:			LDA #$00				; First color is black
				STA BACKGROUND
				STA FOREGROUND
				BEQ START				; Infinite

; That's all
