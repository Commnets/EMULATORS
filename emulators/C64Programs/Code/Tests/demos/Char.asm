* = $0801

	lda #$00 			; black
	sta $d020			; border background colour
	sta $d021			; screen background colour

	tax					; set X to 0 too

clrscreen:				; set all char codes to 0 
	sta $0400,x			; on the screen
	sta $0500,x			
	sta $0600,x			
	sta $0700,x			
	sta $2000,x			; and set charset data to 0
	dex
	bne clrscreen

	lda #$18			; screen at $0400, chars at $2000
	sta $d018

mainloop:
	lda $d012    
	cmp #$ff			; on raster line $ff?
	bne mainloop		; no, go to mainloop

	ldx counter			; get offset value
	inx					; increment it
	cpx #$28			; if it's $28, start over
	bne juststx
	ldx #$00
	
juststx:
	stx counter

	lda $2000,x			; get byte nr x from chardata
	eor #$ff			; invert it
	sta $2000,x			; store it back

	jmp mainloop		; keep going...

* = $0900
counter = $0900
BYTES $08				; initial value for counter
