; C64 Assembler framework.
; CIA1 timer regression: recovered CPU cycles must be processed one by one.
; The border becomes green on success and red on failure.
; $0401 keeps the observed low byte of Timer B.
; By Ignacio Cea Fornies.

BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm

CIA1TALO					= $dc04
CIA1TAHI					= $dc05
CIA1TBLO					= $dc06
CIA1TBHI					= $dc07
CIA1ICR					= $dc0d
CIA1CRA					= $dc0e
CIA1CRB					= $dc0f

* = $cb00
MAIN:						sei
							lda #$00
							sta CIA1CRA
							sta CIA1CRB
							lda #$7f
							sta CIA1ICR
							lda CIA1ICR

							lda #$05
							sta CIA1TALO
							lda #$00
							sta CIA1TAHI
							lda #$ff
							sta CIA1TBLO
							lda #$00
							sta CIA1TBHI

							lda #$51							; Timer B counts Timer A underflows, force load and start.
							sta CIA1CRB
							lda #$11							; Timer A counts PHI2 cycles, force load and start.
							sta CIA1CRA

							ldx #$10
BATCH_LOOP:					jsr BATCH_SUB
							dex
							bne BATCH_LOOP

							lda #$00
							sta CIA1CRA
							sta CIA1CRB

							lda CIA1TBLO
							sta SCREENBASE + 1
							cmp #$c0							; Expected value is close to $c8.
							bcc BATCH_FAIL
							cmp #$d1							; Batched subtraction normally leaves a value above this range.
							bcs BATCH_FAIL

BATCH_PASS:					lda #$01
							sta SCREENBASE
							lda #$05
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp BATCH_PASS

BATCH_FAIL:					lda #$02
							sta SCREENBASE
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp BATCH_FAIL

BATCH_SUB:					rts

; End.
