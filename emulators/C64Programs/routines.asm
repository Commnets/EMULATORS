; Common routines for all programs
; By Ignacio Cea

*				= $CA00
; Where the variables used by the routines are stored (if any)
BYTES $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00
				
; Just a delay
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
CLEAR:			LDA #$20
				LDX #$00
LOOPCLEAR:		STA SCREEN,X
				STA SCREEN1,X
				STA SCREEN2,X
				STA SCREEN3,X
				INX
				BNE LOOPCLEAR
				RTS
