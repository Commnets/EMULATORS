; To flash the border at a specific raster line
; Load the program from the console and then
; Call SYS2049 from the BASIC line
; It cannot be invoked from the command line as it uses the addresses $0314 and $0315.

* = $1000

start:
	sei      			; turn off interrupts
	lda #$7f 			
	ldx #$01 			
	sta $dc0d			; Turn off CIA 1 interrupts
	sta $dd0d			; Turn off CIA 2 interrupts
	stx $d01a			; Turn on raster interrupts
						
	lda #$1b 			
	ldx #$08 			
	ldy #$14 			
	sta $d011			; Clear high bit of $d012, set text mode
	stx $d016			; single-colour
	sty $d018			; screen at $0400, charset at $2000
						
	lda #>int			; low part of address of interrupt handler code
	ldx #<int			; high part of address of interrupt handler code
	ldy #$80 			; line to trigger interrupt
	sta $0314			; store in interrupt vector
	stx $0315			
	sty $d012			
						
	lda $dc0d			; ACK CIA 1 interrupts
	lda $dd0d			; ACK CIA 2 interrupts
	asl $d019			; ACK VIC interrupts
	cli

loop:
	jmp loop			; infinite loop

* = $1200
int = $1200
	inc $d020			; flash border
	
	asl $d019			; ACK interrupt (to re-enable it)
	pla
	tay
	pla
	tax
	pla
	rti					; return from interrupt