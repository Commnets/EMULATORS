; C64 Assembler framework.
; CIA1 Timer A one-shot underflow regression.
; Green means that ICR bit 0 was raised and START was cleared.
; By Ignacio Cea Fornies.

BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm

CIA1TALO					= $dc04
CIA1TAHI					= $dc05
CIA1ICR					= $dc0d
CIA1CRA					= $dc0e

* = $cb00
MAIN:						sei
							lda #$00
							sta CIA1CRA
							lda #$7f
							sta CIA1ICR
							lda CIA1ICR

							lda #$20
							sta CIA1TALO
							lda #$00
							sta CIA1TAHI
							lda #$19							; One-shot, force load and start.
							sta CIA1CRA

							ldy #$20
UNDERFLOW_OUTER:			ldx #$00
UNDERFLOW_WAIT:				lda CIA1ICR
							and #$01
							bne UNDERFLOW_REACHED
							dex
							bne UNDERFLOW_WAIT
							dey
							bne UNDERFLOW_OUTER
							ldx #$01
							jmp UNDERFLOW_FAIL

UNDERFLOW_REACHED:			lda CIA1CRA
							and #$01
							beq UNDERFLOW_PASS
							ldx #$02
							jmp UNDERFLOW_FAIL

UNDERFLOW_PASS:				lda #$01
							sta SCREENBASE
							lda #$05
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp UNDERFLOW_PASS

UNDERFLOW_FAIL:				stx SCREENBASE + 1
							lda #$02
							sta SCREENBASE
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp UNDERFLOW_FAIL

; End.
