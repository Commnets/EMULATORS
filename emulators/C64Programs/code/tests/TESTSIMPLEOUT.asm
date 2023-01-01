; C64 Assembler framework.
; To test the routines.
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
MAIN:						lda #$01
							sta VICIIFOREGROUND
							lda #$03
							STA VICIIBACKGROUND
							lda #$01
							sta SCREENBASE
							lda #$01
							sta COLORRAMBASE

FOREVER:					jmp FOREVER								; For ever...

; End.