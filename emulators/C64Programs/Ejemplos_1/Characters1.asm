; Program to copy the character rom to ram...

; Routine to copy the characters
* = $C000
CHARSET:		sei         ; disable interrupts while we copy
				ldx #$08    ; we loop 8 times (8x255 = 2Kb)
				lda #$33    ; make the CPU see the Character Generator ROM...
				sta $01     ; ...at $D000 by storing %00110011 into location $01
				lda #$d0    ; load high byte of $D000
				sta $fc     ; store it in a free location we use as vector
				ldy #$00    ; init counter with 0
				sty $fb     ; store it as low byte in the $FB/$FC vector

LOOP:			lda ($fb),y ; read byte from vector stored in $fb/$fc
				sta ($fb),y ; write to the RAM under ROM at same position
				iny         ; do this 255 times...
				bne loop    ; ..for low byte $00 to $FF
				inc $fc     ; when we passed $FF increase high byte...
				dex         ; ... and decrease X by one before restart
				bne loop    ; We repeat this until X becomes Zero
				lda #$37    ; switch in I/O mapped registers again...
				sta $01     ; ... with %00110111 so CPU can see them
				cli         ; turn off interrupt disable flag
				rts         ; return from subroutine

; Main program				
* = $C100
