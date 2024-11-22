;----------------------------------
;init VIC
*=$c000
main:
;to complete
				RTS

;----------------------------------
;clear Sprite zone
*=$c12e
clearSpriteAre:

*=$c198
init:
C198  78        SEI							;no interrupts allowed
C199  20 2E C1  JSR $clearSpriteArea	 	;clear sprite area
C19C  20 48 C1  JSR initVIC					;init VIC
C19F  A9 BF     LDA #$BF					;set up IRQ in C1BF
C1A1  A2 C1     LDX #$C1
C1A3  8D 14 03  STA $0314
C1A6  8E 15 03  STX $0315
C1A9  A9 1B     LDA #$1B
C1AB  8D 11 D0  STA $D011					;rows = 25 & scrollY = 3 (normal setup configuration)
C1AE  A9 F7     LDA #$F7
C1B0  8D 12 D0  STA $D012					;raster line IRQ at = 247 (-51 / 8 = 24,5... that's it at the middle of the last text line)
C1B3  A9 01     LDA #$01
C1B5  8D 1A D0  STA $D01A					;enable raster line IRQ
C1B8  A9 7F     LDA #$7F
C1BA  8D 0D DC  STA $DC0D					;no interrupts realted with CIA1
C1BD  58        CLI							;interrupts allowed again
C1BE  60        RTS

;----------------------------------
;init VIC
*=$c148
initVIC=$c148
C148  A2 00     LDX #$00
loopSpritesPosition:
C14A  BD 88 C1  LDA vicIIValues,X
C14D  9D 00 D0  STA $D000,X					;set first 16 values from table. Position of sprites
C150  E8        INX
C151  E0 10     CPX #$10
C153  D0 F5     BNE loopSpritesPosition
C155  A9 FF     LDA #$FF
C157  8D 15 D0  STA $D015					;allow all sprites
C15A  A9 00     LDA #$00
C15C  8D 1C D0  STA $D01C					;none sprite multicolor
C15F  A9 FF     LDA #$FF
C161  8D 17 D0  STA $D017					;none sprite expanded Y
C164  8D 1D D0  STA $D01D					;none sprite expanded X
C167  A9 C0     LDA #$C0
C169  8D 10 D0  STA $D010					;only sprites 8 and 6 in the right
C16C  A9 F8     LDA #$F8
C16E  A2 00     LDX #$00
loopSpriteForm:
C170  9D F8 07  STA $07F8,X					;sets where to read the sprite info (form)
C173  18        CLC
C174  69 01     ADC #$01
C176  E8        INX
C177  E0 08     CPX #$08
C179  D0 F5     BNE loopSpriteForm
C17B  A9 0E     LDA #$0E
C17D  A2 00     LDX #$00
loopSpriteColor:
C17F  9D 27 D0  STA $D027,X					;sets sprite color to white ($0e)
C182  E8        INX
C183  E0 08     CPX #$08
C185  D0 F8     BNE loopSpriteColor
C187  60        RTS

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
C1BF  A2 08     LDX #$08
loopWait:
C1C1  CA        DEX
C1C2  D0 FD     BNE loopWait
C1C4  A2 28     LDX #$28					;40 or so lines
C1C6  EA        NOP							;"timing"
C1C7  EA        NOP
loopLine:
C1C8  EA        NOP
C1C9  EA        NOP
C1CA  CE 16 D0  DEC $D016					;fiddle register
C1CD  EE 16 D0  INC $D016					;I don't know what this is for yet...
C1D0  AC 12 D0  LDY $D012					;loads where the raster is (in Y register): from F7 onwards.
C1D3  88        DEY							;decrease it by one...
C1D4  EA        NOP				
C1D5  98        TYA							;transfer to A register
C1D6  29 07     AND #$07					;keeps only bits 0,1 and 2
C1D8  09 18     ORA #$18					;set bits 3 and 4
C1DA  8D 11 D0  STA $D011					;and store final value back into D011 (25 rows and screen switched on)...to avoid bad line?
C1DD  24 EA     BIT $EA						;+3 cycles
C1DF  EA        NOP							;+1 cycles
C1E0  EA        NOP							;+1 cycles
C1E1  CA        DEX				
C1E2  10 E4     BPL loopLine				;repeat next line
C1E4  A9 1B     LDA #$1B					;sets the right value in the size of the screen (25 rows) and the position of scrollX
C1E6  8D 11 D0  STA $D011		
C1E9  A9 01     LDA #$01
C1EB  8D 19 D0  STA $D019					;clears up the interrupt...
C1EE  20 00 C0  JSR main					;call main code
C1F1  4C 31 EA  JMP $EA31					;finish IRQ and return from the IRQ
