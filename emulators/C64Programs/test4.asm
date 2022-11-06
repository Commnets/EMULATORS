; Iterates the color in the foreground while writes down a set of letters
; By Ignacio Cea

; MACROS
SCREEN			= $0400					; Where the screen starts
COLORRAM		= $D800					; Where the ram color starts

CHARCODES		= $C000					; Where the chars for the screen are defined
NUMCHARS		= $0C					; Number of chars defined
COLORCHARS		= $03					; The color of the chars shown

INICOLOR		= $01					; The initial color to iterate foregound and the color the background will have
FOREGROUND		= $D020					; The address in VICII for the foreground
BACKGROUND		= $D021					; The address in VICII for the background

*				= $C000
BYTES $08 $0f $0c $01 $20 $11 $15 $05 $20 $14 $01 $0C
; = Hola que tal

; Where the code starts
*				= $C100
; Code
CHARS:			LDX #$00
LOOPCHARS:		LDA CHARCODES,X
				STA SCREEN,X
				LDA #COLORCHARS
				STA COLORRAM,X
				INX
				CPX #NUMCHARS
				BNE LOOPCHARS

BKGROUND:		LDX #INICOLOR			; Let's count! First color is black
				STX BACKGROUND
LOOP:			STX FOREGROUND
				INX
				LDY #$A0
DELAY:			NOP
				INY
				BNE DELAY
				CPX #$10
				BNE LOOP				; From color 0 to 15...
				JMP BKGROUND			; Jump to the beginning and starts back

; That's all
