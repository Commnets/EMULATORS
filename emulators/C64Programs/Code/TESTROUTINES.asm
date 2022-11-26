; To test the routines
; By Ignacio Cea Forniés
; Copyright Community Networks 2022 - 2023

; The base has to be defined before doing anything...
BASE = $c000

#../C64Programs/Code/RGRAPHICS.asm
#../C64Programs/Code/RSPRITES.asm

; A very simple program to test everything works...
* = $ca00
MAIN:						lda #$01
							sta VICIIBACKGROUND
							lda #$02
							sta VICIIFOREGROUND
							jsr CLEARSCREEN
MAIN_LOOP:					jmp MAIN_LOOP					; For ever...

; End