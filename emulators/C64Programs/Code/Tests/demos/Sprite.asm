; A white full sprite crossing the screen.

* = $0801

start:
	lda #$01
	sta $d015				; Turn sprite 0 on
	sta $d027				; Make it white
	lda #$40				
	sta $d000				; set x coordinate to 40
	sta $d001				; set y coordinate to 40
	lda #$80				
	sta $07f8				; set pointer: sprite data at $2000

mainloop:
	lda $d012
	cmp #$ff    			; raster beam at line $ff?
	bne mainloop			; no: go to mainloop

	lda dir 				; in which direction are we moving?
	beq down				; if 0, down

; moving up
	ldx coord   			; get coord
	dex         			; decrement it
	stx coord   			; store it
	stx $d000   			; set sprite coords
	stx $d001			
	cpx #$40    			; if it's not equal to $40...
	bne mainloop			; just go back to the mainloop
				
	lda #$00    			; otherwise, change direction
	sta dir
	jmp mainloop

down:
	ldx coord				; this should be familiar
	inx
	stx coord
	stx $d000
	stx $d001
	cpx #$e0
	bne mainloop

	lda #$01
	sta dir
	jmp mainloop

* = $0900
coord = $0900
BYTES $040
* = $0901
BYTES $00
dir = $0901
* = $2000
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
BYTES $ff $ff $ff
