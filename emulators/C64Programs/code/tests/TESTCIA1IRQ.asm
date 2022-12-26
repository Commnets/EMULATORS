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
#../C64Programs/code/routines/RIRQ2.asm

* = $ca00
IRQPRG1_ADDRESS				= $ca00
IRQPRG1:					ldx #00
IRQPRG1_LOOP:				lda #$01
							sta COLORRAMBASE,x
							sta COLORRAMBASE + 960,x
							inc SCREENBASE,x
							inc SCREENBASE + 960,x
							clc
							inx
							cpx #$28
							bne IRQPRG1_LOOP
;							lda #z01111111
;							sta $dc0d
							lda $dc0d
							.RECOVERREGISTERS
							rti

* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
							jsr CLEARSCREEN

							sei
							lda #z01010011
							sta $dc0f
							lda #$ff
							sta $dc04
							lda #$00
							sta $dc05
							lda #$00
							sta $dc06
							lda #$02
							sta $dc07
							lda #z10000010
							sta $dc0d								; Allow interruptions...

							lda #>IRQPRG1_ADDRESS
							sta $0314
							lda #<IRQPRG1_ADDRESS
							sta $0315

							lda #z00010001
							sta $dc0e
							cli
							
FOREVER:					jmp FOREVER								; For ever...

; End.