;----------------------------------
;init VIC
*=$c000
main:
;to complete
				inc $d020
				RTS

;----------------------------------
;clear Sprite zone
*=$c100
clearSpriteArea:
				LDA #$AA
				LDX #$3F
loopClear:
				STA $3E00,X
				STA $3E40,X
				STA $3E80,X
				STA $3EC0,X
				STA $3F00,X
				STA $3F40,X
				STA $3F80,X
				STA $3FC0,X
				DEX
				BPL loopClear
				RTS

;----------------------------------
;init VIC
*=$c200
initVIC=$c200
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
				LDA #$00
				LDX #$00
loopSpriteColor:
				LDA vicIIValues+$10,X
				STA $D027,X					;sets sprite color to white ($0e)
				CLC
				ADC #$01
				INX
				CPX #$08
				BNE loopSpriteColor
				RTS

;----------------------------------
;data set into VIC registers
;defines the position of the sprites in the screen
;all of them above F7 that is where the raster IRQ starts
*=$c300
vicIIValues=$c300
BYTES $00 $F7 $30 $F7 $60 $F7 $90 $F7
BYTES $C0 $F7 $F0 $F7 $20 $F7 $50 $F7
BYTES $00 $01 $02 $03 $04 $05 $00 $07

;----------------------------------
;from where to start the execution of the program
;write in basic "sys 49560" to start
*=$c400
init:
				SEI							;no interrupts allowed
				JSR clearSpriteArea	 		;clear sprite area
				JSR initVIC					;init VIC
				LDA #$00					;set up IRQ in C200
				LDX #$C5
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
				LDA #$01
				STA $D019					;no pending interrupts related with the VICII
				CLI							;interrupts allowed again
				RTS							;a interrupt could happen just after CLI and just before RTS...which wouldn't be good!

;----------------------------------
;main IRQ routine
*=$c500
;how many cycles from this point onwards?
;+up to 3 cycles to finish the instruction under execution
;+7:cycles used to start the IRQ
;+3:PHA					; save A
;+2:TXA					; copy X
;+3:PHA					; save X
;+2:TYA					; copy Y
;+3:PHA					; save Y
;+2:TSX					; copy stack pointer
;+4:LDA	LAB_0100+4,X	; get stacked status register
;+2:AND	#$10			; mask BRK flag
;+3:BEQ	LAB_FF58		; branch if not BRK, that it will be the case...
;+0:JMP	(LAB_0316)		; else do BRK vector (iBRK), because this instruction is not taken...
;LAB_FF58:
;+5:JMP	(LAB_0314)		; do IRQ vector (iIRQ)
;from 36 uo to 39 cycles
;so VICII internal cycle = 36/39
irq:
				LDX #$08					;+2
;so up to now 2 additional cycles
loopWait:
				DEX							;+2
				BNE loopWait				;+3,+2
;so up to now 41 additional cycles = 2 + (5 cyles * 8 times - 1)
;let's divide this loop in 2 phases: 
;when the VICII internal cycle 62 is reached, and sprite info started to be read, and the rest
;the first phase is reached when the: 38/41 + (5 cycles * x times) = 55 (3 cycles before reading the first); x = in the middle of the loop 2nd or 3rd...
;As there are 8 sprites info to read, the CPU stops until the cycle 12th is reached in the next line...
;and then there would be between 21 and 24 additional cycles spend in the rest of the loop.
;so after the loop the VICII internal cycle would be = 12 + 21/12 + 24 = 33/36, that is in the middle of the line $f8...
				LDX #$28					;+2: 40 or so lines
				NOP							;+2
				NOP							;+2
;at this point the VICII internal cycle would be = 39/42
loopLine:
				NOP							;+2
				NOP							;+2
				DEC $D016					;+6: fiddle register
;at the first iteration at this position the VICII cycle will be = 39 + 10/42 + 10 = 49/52
				INC $D016					;+6: I don't know what this is for yet...
;at the first iteration at this position the VICII cycle will be = 39 + 16/42 + 16 = 55/58
;at this moment the VICII starts to read again the information of the sprites, so it stops until everything is in cycle 12 and the raster line = $fa
;the cpu stops, but the content is store in the memory...
				LDY $D012					;+4: loads where the raster is (in Y register): from F8 onwards.
				DEY							;+2: decrease it by one...
				NOP							;+2
				TYA							;+2: transfer to A register
				AND #$07					;+2: keeps only bits 0,1 and 2
				ORA #$10					;+2: set bits 3 and 4
				STA $D011					;+4: and store final value back into D011 (25 rows and screen switched on)...to avoid bad line?
				BIT $EA						;+3
				NOP							;+2
				NOP							;+2
				DEX							;+2 				
				BPL loopLine				;+3: repeat next line
;at the first iteration at this position the VICII cycle will be = 12+30 = 42
;so it starts back for 38 more lines, using the same cycles and the same behaviour...
				LDA #$1B					;sets the right value in the size of the screen (25 rows) and the position of scrollX
				STA $D011		
				LDA #$01
				STA $D019					;clears up the interrupt...
				JSR main					;call main code
				JMP $EA31					;finish IRQ and return from the IRQ
