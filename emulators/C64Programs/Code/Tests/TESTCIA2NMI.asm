; C64 Assembler framework.
; To test the CIA 1 working.
; By Ignacio Cea Fornies.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the files needed
#../C64Programs/code/routines/RSCREENCLEAR.asm
#../C64Programs/code/routines/RIRQ2.asm

* = $ca00
NMIPRG1_ADDRESS				= $ca00
NMIPRG1:					.SAVEREGISTERS
							ldx #00
NMIPRG1_LOOP:				lda #$06
							sta COLORRAMBASE,x
							sta COLORRAMBASE + (24*40),x
							inc SCREENBASE,x
							inc SCREENBASE + (24*40),x
							clc
							inx
							cpx #$28
							bne NMIPRG1_LOOP
							lda $dc0d
							.RECOVERREGISTERS
							rti

* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$05
							sta VICIIBACKGROUND
							lda #$0d
							sta VICIIFOREGROUND
							jsr CLEARSCREEN

							lda #$ff
							sta $dd04
							lda #$02
							sta $dd05
							lda #$00
							sta $dd06
							lda #$02
							sta $dd07
							lda #z00010001
							sta $dd0e
							lda #z01010001
							sta $dd0f

							lda #z10000010
							sta $dd0d							; Allow interruptions...

							lda #>NMIPRG1_ADDRESS
							sta $0318
							lda #<NMIPRG1_ADDRESS
							sta $0319
					
FOREVER:					jmp FOREVER							; For ever...

; End.