; To show a sprite in the screen and moving across
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
; Related with the sprites
SPRITEENABLE	= $D015					; Sprites on - off 
SPRITEXPOS		= $D000					; The coordinate X of the sprite 1
SPRITEYPOS		= $D001					; The coordinate Y of the sprite 1
SPRITEMSB		= $D010					; The most significatn bit of the X position for sprites
SPRITEDWIDTH	= $D01D					; Location to set double width for sprites
SPRITEDHEIGHT	= $D017					; Location to set double height for sprites
SPRITECOLOR		= $D027					; The color of the sprite 1
SPRITEPRIORITY	= $D01B					; To manage the priority of the sprites over the foreground
SPRITEMCOLOR	= $D01C					; To manage the multicolor

SPRITELOC		= $07F8					; Location where to find the 64 block where the sprite 1 data is

* 				= $0800
; The data for the sprite 1: A cross.
BYTES			$00 $00 $00
BYTES			$00 $00 $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$1F $FF $F8
BYTES			$1F $FF $F8
BYTES			$1F $FF $F8
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $7F $00
BYTES			$00 $00 $00
BYTES			$00 $00 $00

; Just a delay
*				= $0900
DELAY:			PHA
				TXA
				PHA
				TYA
				PHA
				LDY #$04
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
*				= $0A00
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
*				= $0B00
;				JSR CLEAR
MAIN:			LDA #BKCOLOR
				STA BACKGROUND
				LDA #FGCOLOR
				STA FOREGROUND
SETSPRITE:		LDA #$20
				STA SPRITELOC
				LDA #$07
				STA SPRITEXPOS
				LDA #$80
				STA SPRITEYPOS
				LDA #$06
				STA SPRITECOLOR
				LDA SPRITEDWIDTH
				ORA #$01
				STA SPRITEDWIDTH
				LDA SPRITEDHEIGHT
				ORA #$01
				STA SPRITEDHEIGHT
				LDA SPRITEENABLE
				ORA #$01
				STA SPRITEENABLE
				LDA SPRITEPRIORITY
				ORA #$01
				STA SPRITEPRIORITY
MOVE:			LDX #$00
LOOPMOVE:		STX SPRITEXPOS
				JSR DELAY
				INX
				BNE LOOPMOVE
				LDA SPRITEMSB
				AND #$fe
				ORA #$01
				STA SPRITEMSB
LOOPMOVE2:		STX SPRITEXPOS 
				JSR DELAY
				INX
				CPX #$40
				BNE LOOPMOVE2
				LDA SPRITEMSB
				AND #$fe
				ORA #$00
				STA SPRITEMSB
				JMP MOVE				

; That's all
