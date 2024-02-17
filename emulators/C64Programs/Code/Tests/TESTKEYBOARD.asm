; C64 Assembler framework.
; To test the Keyboard.
; By Ignacio Cea Fornies.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the files needed
#../C64Programs/code/routines/RSCREENCLEAR.asm

* = $cb00
; A very simple program to test everything works...
MAIN:						jsr $ea87							; The routine to read the keyboard
							lda $c6
							beq IRQPRG1_OUT
							dex
							stx $c6
							lda $0277
							jsr $ffd2							; The routine to cout the char
IRQPRG1_OUT:				jmp MAIN

; End.