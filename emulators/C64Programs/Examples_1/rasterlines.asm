*=$1000
loop:	lda $d012
		cmp #$ff
		bne loop
		inc $d021
		jmp loop