;coded by Graham/Oxyron

*= $c000
main:
	lda #$7f
	sta $dc0d
	and $d011
	sta $d011
	lda #$32
	sta $d012
	
	sei
	lda #<irq
	sta $0314
	lda #>irq
	sta $0315
	lda #$01
	sta $d01a
	cli
	rts

*=$c030
irq=$c030
	lda #$ff
	sta $d019
;	
;	ldx #$05
;LD:	dex
;	bne LD
;
;	ldx #$00
;LC:	ldy #$08
;LA:	lda colors,x
;	sta $d020
;	sta $d021
;	inx
;	dey
;	beq LC
;
;	txa
;	ldx #$07
;LB:	dex
;	bne LB
;	tax
;
;	cpx #$8c
;	bcc a

	jmp $ea31

*= $c100
colors=$c100
BYTES $09 $02 $08 $0a $0f $07 $01 $07 $0f $0a $08 $02 $09 $00
BYTES $06 $04 $0e $05 $03 $0d $01 $0d $03 $05 $0e $04 $06 $00
BYTES $09 $02 $08 $0a $0f $07 $01 $07 $0f $0a $08 $02 $09 $00
BYTES $06 $04 $0e $05 $03 $0d $01 $0d $03 $05 $0e $04 $06 $00
BYTES $09 $02 $08 $0a $0f $07 $01 $07 $0f $0a $08 $02 $09 $00
BYTES $06 $04 $0e $05 $03 $0d $01 $0d $03 $05 $0e $04 $06 $00
BYTES $09 $02 $08 $0a $0f $07 $01 $07 $0f $0a $08 $02 $09 $00
BYTES $06 $04 $0e $05 $03 $0d $01 $0d $03 $05 $0e $04 $06 $00
BYTES $09 $02 $08 $0a $0f $07 $01 $07 $0f $0a $08 $02 $09 $00
BYTES $06 $04 $0e $05 $03 $0d $01 $0d $03 $05 $0e $04 $06 $00