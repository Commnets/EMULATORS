; Smooth horizontal scroll (test ti) for a set of letters...
; By Ignacio Cea

; MACROS
; Related with the screen
SCREEN0			= $0400					; Where the screen starts
SCREEN1			= $0500
SCREEN2			= $0600
SCREEN3			= $0700
SCREEN			= $0428					; Where the screen starts
COLORRAM		= $D828					; Where the ram color starts
BKCOLOR			= $01					; The initial color of the background
FGCOLOR			= $03					; The initial color of the foregound
FOREGROUND		= $D020					; The address in VICII for the foreground
BACKGROUND		= $D021					; The address in VICII for the background
SCROLLX			= $D016					; The address of the VIC II dedeicated to the scroll
SCROLLXPOS		= $C0F0					; The address where the current x scroll position is stored
; Related with the text in the screen
CHARCODES		= $C000					; Where the chars for the screen are defined
NUMCHARS		= $0C					; Number of chars defined
COLORCHARS		= $06					; The color of the chars shown
*				= $C000
BYTES $08 $0f $0c $01 $20 $11 $15 $05 $20 $14 $01 $0C
; = Hola que tal

; The code				
; Just a delay
*				= $C100
DELAY:			PHA
				TXA
				PHA
				TYA
				PHA
				LDY #$0C
LOOPYNOP:		LDX #$FF
LOOPXNOP:		NOP
				DEX
				BNE LOOPXNOP
				DEY
				BNE LOOPYNOP
				PLA
				TAY
				PLA
				TAX
				PLA
				RTS

; To clear the screen
*				= $C200
CLEAR:			LDA #$20
				LDX #$00
LOOPCLEAR:		STA SCREEN0,X
				STA SCREEN1,X
				STA SCREEN2,X
				STA SCREEN3,X
				INX
				BNE LOOPCLEAR
				RTS

; The main program
*				= $C300
				JSR CLEAR
				LDX #$00				; To draw the text in the screen
LOOPCHARS:		LDA CHARCODES,X
				STA SCREEN,X
				LDA #COLORCHARS
				STA COLORRAM,X
				INX
				CPX #NUMCHARS
				BNE LOOPCHARS

SCREENCOLOR:	LDA #BKCOLOR			; To set the color of the screen
				STA BACKGROUND
				LDA #FGCOLOR
				STA FOREGROUND

SCREENSIZE:		LDA SCROLLX
				AND #$F7
				STA SCROLLX
		
MOVE:			LDA #$00
				STA SCROLLXPOS
				TAX
LOOPMOVE:		LDA SCROLLX
				AND #$F8
				ORA SCROLLXPOS
				STA SCROLLX
				JSR DELAY
				INX
				STX SCROLLXPOS
				CPX #$08
				BNE LOOPMOVE
				JMP MOVE				; Jump to the beginning and starts back

; That's all
