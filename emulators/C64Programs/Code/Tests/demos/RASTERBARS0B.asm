; flasher
; This program causes the border to flash 60 times per second
; It has to be executed directy

* = $c000
main:
	sei							; disable interrupts
	lda #>intcode				; get low byte of target routine
	sta $fffe					; put into interrupt vector
	lda #<intcode				; do the same with the high byte
	sta $ffff
	cli							; re-enable interrupts
forever:
	jmp forever					; return to caller

* = $c100
intcode = $c100
inc $d020					; change border colour
rti							; exit back to rom
