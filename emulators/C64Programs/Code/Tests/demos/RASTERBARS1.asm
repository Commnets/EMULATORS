; Simple Rasterbar demo
; by Knoeki of Digital Sounds System
; invoke it directly from the command line 
; with the parameters /c..[PATH]\RASTERBARS1.asm /a$080d
; It could also work from the BASIC line: Load the program and call SYS2061

* = $080d

			sei				;disable interrupts
			lda #$00		;load $00 into A
			sta $d011		;turn off screen. (now you have only borders!)
			sta $d020		;make border black.

main:		ldy #$7a		;load $7a into Y. this is the line where our rasterbar will start.
			ldx #$00		;load $00 into X
loop: 		lda colors,x	;load value at label 'colors' plus x into a. if we don't add x, only the first 
							;value from our color-table will be read.

test:		cpy $d012 		;Compare current value in Y with the current rasterposition.
			bne test   		;is the value of Y not equal to current rasterposition? then jump back 3 bytes (to cpy).
			
			sta $d020		;if it IS equal, store the current value of A (a color of our rasterbar)
							;into the bordercolour

			cpx #51			;compare X to #51 (decimal). have we had all lines of our bar yet?
			beq main		;Branch if EQual. if yes, jump to main.

			inx				;increase X. so now we're gonna read the next color out of the table.
			iny				;increase Y. go to the next rasterline.
			
			jmp loop  ;jump to loop.


* = $0900
colors = $0900
BYTES $06 $06 $06 $0e $06 $0e
BYTES $0e $06 $0e $0e $0e $03
BYTES $0e $03 $03 $0e $03 $03
BYTES $03 $01 $03 $01 $01 $03
BYTES $01 $01 $01 $03 $01 $01
BYTES $03 $01 $03 $03 $03 $0e
BYTES $03 $03 $0e $03 $0e $0e
BYTES $0e $06 $0e $0e $06 $0e
BYTES $06 $06 $06 $00 $00 $00
BYTES $ff

;-----------------------------------------------------------------
;
;if everything goes correctly, you should have a blue rasterbar on
;the middle of your screen. play with the values a bit, and see
;what you can do with it... =)