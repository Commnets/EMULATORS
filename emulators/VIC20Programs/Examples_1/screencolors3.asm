; To make lines of colors in the text window

; MACROS
BRDCOLOR	= $00				; The color of the border
BKCOLOR		= $00				; The color of the text screen
SCREENCOLOR = $900F				; The register where to store the color info (both border and background)

; Where the code starts
* = $0400

; Code to make lines in the text window
START:			LDA #BKCOLOR
				AND#$0f
				STA $00
				LDX #BRDCOLOR
LOOP:			TXA
				ASL
				ASL
				ASL
				ORA $00
				STA SCREENCOLOR
				INX
				LDY #$ff
SLOW:			DEY 
				BNE SLOW
				JMP LOOP
FOREVER:		JMP FOREVER				; Infinite

; That's all
