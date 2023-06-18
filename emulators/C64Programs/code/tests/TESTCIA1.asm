; C64 Assembler framework.
; To test the CIA 1 working.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the files needed
#../C64Programs/code/routines/RSCREENCLEAR.asm

* = $cb00
; A very simple program to test everything works...
MAIN:						jsr $fd15							; To set up the jumping kernal table...
							lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
							jsr CLEARSCREEN

LOOP:						lda #z10111111
							sta $dc00
							lda $dc01
							sta SCREENBASE
							lda #$01
							sta COLORRAMBASE
							jmp LOOP
							
FOREVER:					jmp FOREVER							; For ever...

; End.