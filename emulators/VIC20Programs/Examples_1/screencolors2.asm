; Set the color of the both background and foreground
; By Ignacio Cea

; MACROS
COLOR1	    = $27
COLOR2	    = COLOR1 + 2
SCREENCOLOR = $900F

; Where the code starts
* = $0400

; Code
START:			LDA #COLOR1				; First color
LOOP:			STA SCREENCOLOR
				TAX
				INX
				TXA
				JMP LOOP
FOREVER:		JMP FOREVER				; Infinite

; That's all
