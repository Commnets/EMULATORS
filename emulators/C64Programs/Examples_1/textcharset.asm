*=$1000
init:			jsr $e544
				lda #$0d
				sta $d020
				lda #$05
				sta $d021
				lda #$18
				sta $d018
				ldx #$00

write:			lda    msg,x
				jsr    $ffd2
				inx
				cpx    #54
				bne    write
				ldx #$00

setcolor:		lda #$07
				sta $d800,x
				inx
				cpx #$54
				bne setcolor

loop:			jmp loop

*=$1ffe
BYTESFILE ../C64Programs/Examples_1/textcharset.64c

*=$c000
msg=$c000
TEXT "C64 programming tutorial by digitalerr0r of Dark Codex"
