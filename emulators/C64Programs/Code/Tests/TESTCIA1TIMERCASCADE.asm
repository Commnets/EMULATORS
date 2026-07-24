; C64 Assembler framework.
; CIA1 Timer B cascade regression.
; Timer B must count Timer A underflows and eventually set ICR bit 1.
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

							lda #$10
							sta CIA1TALO
							lda #$00
							sta CIA1TAHI
							lda #$03
							sta CIA1TBLO
							lda #$00
							sta CIA1TBHI
							lda #$51							; Timer B counts Timer A underflows.
							sta CIA1CRB
							lda #$11
							sta CIA1CRA

							ldy #$40
CASCADE_OUTER:				ldx #$00
CASCADE_WAIT:				lda CIA1ICR
							and #$02
							bne CASCADE_PASS
							dex
							bne CASCADE_WAIT
							dey
							bne CASCADE_OUTER

CASCADE_FAIL:				lda #$00
							sta CIA1CRA
							sta CIA1CRB
							lda #$02
							sta SCREENBASE
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp CASCADE_FAIL

CASCADE_PASS:				lda #$00
							sta CIA1CRA
							sta CIA1CRB
							lda #$01
							sta SCREENBASE
							lda #$05
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp CASCADE_PASS

; End.
