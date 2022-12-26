; C64 Assembler framework.
; To test the box routine with limits
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the files needed
#../C64Programs/code/routines/RSCREENCLEAR.asm
#../C64Programs/code/routines/RSCREENBOX.asm
#../C64Programs/code/routines/RSCREENRECT.asm
#../C64Programs/code/routines/RSCREENTEXT.asm

* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
;							jsr CLEARSCREEN

; Test the creation of a box.
BOXEXAMPLE:					lda #$ff								; Pos X.
							sta FILLSCR_XPOSVAR
							lda #$ff								; Pos Y.
							sta FILLSCR_YPOSVAR
							lda #$03								; Size X.
							sta FILLSCR_XLENVAR
							lda #$03								; Size Y.
							sta FILLSCR_YLENVAR
							lda #$01								; With letter A.
							sta FILLSCR_BYTEVAR
							lda #$01								; White.
							sta FILLSCR_COLORVAR
							jsr FILLBOXSCREENIN

FOREVER:					jmp FOREVER								; For ever...

; End.