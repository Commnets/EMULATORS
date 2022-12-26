; C64 Assembler framework.
; To test the IRQ.
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
IRQPRG1:					lda #$05
							sta VICIIBACKGROUND

							lda #$ff						; Meaning the VICII IRQs are treated 
							sta VICIIIRQ					; and another different one might come later...
															
							lda #<IRQPRG2_ADDRESS
							sta SETVICIIRIRQ_PRGHVAR
							lda #>IRQPRG2_ADDRESS
							sta SETVICIIRIRQ_PRGLVAR
							lda #$00
							sta SETVICIIRIRQ_ROWLVAR
							lda #$00
							sta SETVICIIRIRQ_ROWHVAR
							jsr SETVICIIRASTERIRQ
							.RECOVERREGISTERS
							
							rti

* = $ca64
IRQPRG2_ADDRESS				= $ca64
IRQPRG2:					lda #$06
							sta VICIIBACKGROUND

							lda #$ff						; Meaning the VICII IRQs are treated 
							sta VICIIIRQ					; and another different one might come later...
							
							lda #<IRQPRG1_ADDRESS
							sta SETVICIIRIRQ_PRGHVAR
							lda #>IRQPRG1_ADDRESS
							sta SETVICIIRIRQ_PRGLVAR
							lda #$96
							sta SETVICIIRIRQ_ROWLVAR
							lda #$00
							sta SETVICIIRIRQ_ROWHVAR
							jsr SETVICIIRASTERIRQ
							.RECOVERREGISTERS
							
							rti

* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND
							jsr CLEARSCREEN
						
							lda #<IRQPRG1_ADDRESS
							sta SETVICIIRIRQ_PRGHVAR
							lda #>IRQPRG1_ADDRESS
							sta SETVICIIRIRQ_PRGLVAR
							lda #$96
							sta SETVICIIRIRQ_ROWLVAR
							lda #$00
							sta SETVICIIRIRQ_ROWHVAR
							jsr SETVICIIRASTERIRQ

FOREVER:					jmp FOREVER							; For ever...

; End.