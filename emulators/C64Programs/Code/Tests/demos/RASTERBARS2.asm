;coded by Bitbreaker Oxyron ^ Nuance ^ Arsenic
;feel free to change $d020/$d021 to other registers like $d022/$d023 for effects with multicolor charsets
;as you see, there are plenty of cycles free for more action.
 
* = $2000

tmpa	= $22
tmpx	= $23
tmpy	= $24
tmp_1	= $25

main:
		sei
		lda #$7f
		sta $dc0d
		lda $dc0d
		lda #$01
		sta $d01a
		sta $d019
		lda #$32
		sta $d012
		lda $d011
		and #$3f
		sta $d011
		lda #$34
		sta $01
		lda #<irq1
		sta $fffe
		lda #>irq1
		sta $ffff
		cli
		jmp main
 
irq1:
;irq enter stuff
		sta tmpa
		stx tmpx
		sty tmpy
		lda $01
		sta tmp_1
		lda #$35
		sta $01
		dec $d019
		
		ldx #$01
		dex
		bpl main
		
;do raster
		jsr raster
		
;exit irq
		lda tmp_1
		sta $01
		ldy tmpy
		ldx tmpx
		lda tmpa
		rti
 
raster:
ldx #$00
moreless:
		ldy #$07		;2
		lda tab,x		;4
		sta $d020		;4
		sta $d021		;4
		inx				;2
		cpx #$c8		;2
		beq more		;2
		nop				;2 _20
less:
		lda tab,x		;4
		sta $d020		;4
		sta $d021		;4
		jsr more		;12
		jsr more		;12
		jsr more		;12 _48
		nop				;2
		inx				;2
		cpx #$c8		;2
		beq more		;2
		dey				;2
		beq moreless	;2 / 3 _61 (+2)
		bne less		;3 _63
more:
		rts
 
!align 255, 0
 
;your colors go here
* = $2100
tab = $100
BYTES $01 $02 $03 $04 $05 $06 $07 $08
BYTES $01 $02 $03 $04 $05 $06 $07 $08
BYTES $01 $02 $03 $04 $05 $06 $07 $08 $09
