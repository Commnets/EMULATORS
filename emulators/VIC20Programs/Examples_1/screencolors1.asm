; Set the color of the both background and foreground
; By Ignacio Cea

; MACROS
COLOR	    = $27
SCREENCOLOR = $900F

; Where the code starts
* = $0400

; Code
START:			LDA #COLOR				; First color
				STA SCREENCOLOR
FOREVER:		JMP FOREVER				; Infinite

; That's all
