; C64 Assembler framework.
; To test the high resolution graphical modes.
; By Ignacio Cea Fornies.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the files needed
#../C64Programs/code/routines/RSCREENCLEAR.asm

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
BINARY	11001001
BINARY	11001001
BINARY	11001001
BINARY	11001001
BINARY	11001001
BINARY	11001001
BINARY	11001001
BINARY	11001001

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
							ora #$20							; Sets the bit 5 on... bitmap mode
							sta VICIISCROLLY
							lda VICIISCROLLX
							and #$ef
							ora #$10							; Sets the bit 4 on... multicolor bitmap mode
							sta VICIISCROLLX
							lda VICIICTRLMEMREG
							and #$f7
							ora #$08							; The location of the graphical memory is at 8192
							sta VICIICTRLMEMREG

DRAWGRAPHIC:				ldx #$00
DRAWGRAPHIC_LOOP:			lda BLOCKLINE,x
							sta BITMAPMEM,x
							inx 
							cpx #$10
							bne DRAWGRAPHIC_LOOP
							lda #$17
							sta SCREENBASE
							sta SCREENBASE + 1
							lda #$03
							sta COLORRAMBASE
							sta COLORRAMBASE + 1

FOREVER:					jmp FOREVER							; For ever...

; End.