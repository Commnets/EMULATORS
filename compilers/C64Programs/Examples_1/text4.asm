; A text moves smoothly across the top of the screen...
; By Ignacio Cea

; MACROS
; Related with the screen
SCREEN			= $0400					; Where the screen starts
SCREEN1			= $0500
SCREEN2			= $0600
SCREEN3			= $0700
COLORRAM		= $D800					; Where the ram color starts
BKCOLOR			= $03					; The background
FGCOLOR			= $01					; The foreground 
BACKGROUND		= $D020					; The address in VICII for the background
FOREGROUND		= $D021					; The address in VICII for the foreground
SCROLLX			= $D016					; The address of the VIC II dedeicated to the scroll
; Related with the text to be moved
CHARCODES		= $C000					; Where the chars for the screen are defined
NUMCHARS		= $0C					; Number of chars defined
COLORCHARS		= $06					; The color of the chars shown
SCREENCODES		= $C0F0					; Where the chars to be drawn are

*				= $C000
BYTES $08 $0f $0c $01 $20 $11 $15 $05 $20 $14 $01 $0C
; = Hola que tal

; To draw the text while enter the screen...
FIRSTCHARTODRAW	= $C010					; Where the char to draw is stored...
*				= $C100
DRAWTEXT:		PHA
				TXA
				PHA
				TYA
				PHA
				LDA FIRSTCHARTODRAW
				CMP #$00
				BEQ RESTORECHAR
				TAX
				JMP DRAWTEXTLOOP
RESTORECHAR:	LDA #NUMCHARS
				STA FIRSTCHARTODRAW
				JMP DRAWRTS
DRAWTEXTLOOP:	DEX
				STX FIRSTCHARTODRAW
				LDY #$00
LOOPDRAWTEXT:	LDA CHARCODES,X
				STA SCREEN,Y
				LDA #COLORCHARS
				STA COLORRAM,Y
				INY
				INX
				CPX #NUMCHARS
				BNE LOOPDRAWTEXT
DRAWRTS:		PLA
				TAY
				PLA
				TAX
				PLA
				RTS

; To move a position the text
*				= $C200
MOVETEXT:		PHA
				TXA
				PHA
				TYA
				PHA
				LDX #$26				; $26 = 38 The initial position to move (starting from 0)
LOOPMOVETEXT:	LDA SCREEN,X
				INX
				STA SCREEN,X
				DEX
				LDA COLORRAM,X
				INX 
				STA COLORRAM,X
				DEX
				DEX
				BPL LOOPMOVETEXT		; Including the position 0...
				LDA #$20				; When move, the first character has to be a space...
				STA SCREEN
				LDA #COLORCHARS
				STA COLORRAM
				PLA
				TAY
				PLA
				TAX
				PLA
				RTS

; To scroll smooth the screen...
SCROLLXPOS		= $C011					; The address where the current x scroll position is stored
*				= $C300
SMOOTHMOVE:		PHA
				TXA
				PHA
				TYA
				PHA
				LDA #$00
				STA SCROLLXPOS
				TAX
LOOPSMOVE:		LDA SCROLLX
				AND #$F8
				ORA SCROLLXPOS
				STA SCROLLX
				JSR DELAY
				INX
				STX SCROLLXPOS
				CPX #$08
				BNE LOOPSMOVE
				PLA
				TAY
				PLA
				TAX
				PLA
				RTS

; Just a delay
*				= $C400
DELAY:			PHA
				TXA
				PHA
				TYA
				PHA
				LDY #$03
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
*				= $C500
CLEAR:			LDA #$20
				LDX #$00
LOOPCLEAR:		STA SCREEN,X
				STA SCREEN1,X
				STA SCREEN2,X
				STA SCREEN3,X
				INX
				BNE LOOPCLEAR
				RTS

; The main program
*				= $C600
				JSR CLEAR
MAIN:			LDA #BKCOLOR
				STA BACKGROUND
				LDA #FGCOLOR
				STA FOREGROUND
SCREENSIZE:		LDA SCROLLX
				AND #$F7
				STA SCROLLX
				LDA #NUMCHARS
				STA FIRSTCHARTODRAW
MAINDRAW:		JSR DRAWTEXT
				JSR SMOOTHMOVE
				LDA FIRSTCHARTODRAW
				CMP #NUMCHARS
				BEQ MAINMOVE			; When all elements are in the screen, starts to move them
				JSR DELAY
				JMP MAINDRAW
MAINMOVE:		LDX #$28				; $28 = 40 Screen width
MAINLOOPMOVE:	JSR MOVETEXT
				JSR SMOOTHMOVE
				DEX
				BPL MAINLOOPMOVE
				JMP MAIN;

; That's all
