;----------------------------------
;init VIC
*=$c000
main:
;to complete
				RTS

;----------------------------------
;clear Sprite zone
*=$c12e
clearSpriteArea:
				RTS

*=$c198
init:
				SEI							;no interrupts allowed
				JSR clearSpriteArea	 		;clear sprite area
				JSR initVIC					;init VIC
				LDA #$BF					;set up IRQ in C1BF
				LDX #$C1
				STA $0314
				STX $0315
				LDA #$1B
				STA $D011					;rows = 25 & scrollY = 3 (normal setup configuration)
				LDA #$F7
				STA $D012					;raster line IRQ at = 247 (-51 / 8 = 24,5... that's it at the middle of the last text line)
				LDA #$01
				STA $D01A					;enable raster line IRQ
				LDA #$7F
				STA $DC0D					;no interrupts realted with CIA1
				CLI							;interrupts allowed again
				RTS

;----------------------------------
;init VIC
*=$c148
initVIC=$c148
initVIC:
				LDX #$00
loopSpritesPosition:
				LDA vicIIValues,X
				STA $D000,X					;set first 16 values from table. Position of sprites
				INX
				CPX #$10
				BNE loopSpritesPosition
				LDA #$FF
				STA $D015					;allow all sprites
				LDA #$00
				STA $D01C					;none sprite multicolor
				LDA #$FF
				STA $D017					;none sprite expanded Y
				STA $D01D					;none sprite expanded X
				LDA #$C0
				STA $D010					;only sprites 8 and 6 in the right
				LDA #$F8
				LDX #$00
loopSpriteForm:
				STA $07F8,X					;sets where to read the sprite info (form)
				CLC
				ADC #$01
				INX
				CPX #$08
				BNE loopSpriteForm
				LDA #$0E
				LDX #$00
loopSpriteColor:
				STA $D027,X					;sets sprite color to white ($0e)
				INX
				CPX #$08
				BNE loopSpriteColor
				RTS

;----------------------------------
;data set into VIC registers
;defines the position of the sprites in the screen
;all of them above F7 that is where the raster IRQ starts
*=$c188
vicIIValues=$c188
BYTES $00 $F7 $30 $F7 $60 $F7 $90 $F7
BYTES $C0 $F7 $F0 $F7 $20 $F7 $50 $F7

;----------------------------------
;main IRQ routine
*=$c1bf
irq:
				LDX #$08
loopWait:
				DEX
				BNE loopWait
				LDX #$28					;40 or so lines
				NOP							;"timing"
				NOP
loopLine:
				NOP
				NOP
				DEC $D016					;fiddle register
				INC $D016					;I don't know what this is for yet...
				LDY $D012					;loads where the raster is (in Y register): from F7 onwards.
				DEY							;decrease it by one...
				NOP				
				TYA							;transfer to A register
				AND #$07					;keeps only bits 0,1 and 2
				ORA #$18					;set bits 3 and 4
				STA $D011					;and store final value back into D011 (25 rows and screen switched on)...to avoid bad line?
				BIT $EA						;+3 cycles
				NOP							;+1 cycles
				NOP							;+1 cycles
				DEX				
				BPL loopLine				;repeat next line
				LDA #$1B					;sets the right value in the size of the screen (25 rows) and the position of scrollX
				STA $D011		
				LDA #$01
				STA $D019					;clears up the interrupt...
				JSR main					;call main code
				JMP $EA31					;finish IRQ and return from the IRQ
