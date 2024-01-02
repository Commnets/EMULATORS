; To make lines of color just in the border
; By Ignacio Cea

; MACROS
BKCOLOR		= $02
BRDCOLOR	= $00
SCREENCOLOR = $900F

; Where the code starts
* = $0400

; Code
START:			LDX #BRDCOLOR
LOOP:			LDA #BKCOLOR
				ASL
				ASL
				ASL
				STA $00
				TXA
				AND #$0f
				ORA $00
				STA SCREENCOLOR
				INX
				LDY #$ff
SLOW:			DEY 
				BNE SLOW
				JMP LOOP
FOREVER:		JMP FOREVER				; Infinite

; That's all
