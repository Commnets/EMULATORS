; flasher
; This program causes the border to flash 60 times per second
; It has to be executed from BASIC line bacause it doesn't stop it!
; Load the program and call SYS49152. 

* = $c000
main:
	sei							; disable interrupts
	lda #>intcode				; get low byte of target routine
	sta $0314					; put into interrupt vector
	lda #<intcode				; do the same with the high byte
	sta $0315
	cli							; re-enable interrupts
	rts							; return to caller

* = $c100
intcode = $c100
	inc $d020					; change border colour
	jmp $ea31					; exit back to rom
