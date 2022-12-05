; C64 Assembler framework.
; To test the high resolution graphical modes.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/Code/RGENERAL.asm

#../C64Programs/Code/RSCREENCLEAR.asm
#../C64Programs/Code/RSCREENBOX.asm
#../C64Programs/Code/RSCREENRECT.asm
#../C64Programs/Code/RSCREENTEXT.asm

; Data
* = $ca10
; Block for a diagonal line...
BINARY	10000000
BINARY	01000000
BINARY	00100000
BINARY	00010000
BINARY	00001000
BINARY	00000100
BINARY	00000010
BINARY	00000001
BINARY	00000001
BINARY	00000010
BINARY	00000100
BINARY	00001000
BINARY	00010000
BINARY	00100000
BINARY	01000000
BINARY	10000000

BLOCKLINE = $ca10
BITMAPMEM = $2000

* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
							jsr CLEARSCREEN

; Change the graphics mode...
; Bitmap mode
SETGRAPHICMODE:				lda VICIISCROLLY
							and #$df
							ora #$20								; Sets the bit 5 on... bitmap mode
							sta VICIISCROLLY
							lda VICIICTRLMEMREG
							and #$f7
							ora #$08								; The location of the graphical memory is at 8192
							sta VICIICTRLMEMREG

DRAWGRAPHIC:				ldx #$00
DRAWGRAPHIC_LOOP:			lda BLOCKLINE,x
							sta BITMAPMEM,x
							inx 
							cpx #$10
							bne DRAWGRAPHIC_LOOP
							lda #$f2
							sta SCREENBASE
							sta SCREENBASE + 1

FOREVER:					jmp FOREVER								; For ever...

; End.