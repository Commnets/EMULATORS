; C64 Assembler framework.
; CIA1 serial-output regression driven by Timer A underflows.
; Green means that eight output bits completed and ICR bit 3 was raised.
; By Ignacio Cea Fornies.

BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm

CIA1TALO					= $dc04
CIA1TAHI					= $dc05
CIA1SDR					= $dc0c
CIA1ICR					= $dc0d
CIA1CRA					= $dc0e

* = $cb00
MAIN:						sei
							lda #$00
							sta CIA1CRA
							lda #$7f
							sta CIA1ICR
							lda CIA1ICR

							lda #$02
							sta CIA1TALO
							lda #$00
							sta CIA1TAHI
							lda #$a5
							sta CIA1SDR
							lda #$88							; Enable serial interrupt source.
							sta CIA1ICR
							lda #$51							; Serial output, continuous Timer A, force load and start.
							sta CIA1CRA

							ldy #$80
SERIAL_OUTER:				ldx #$00
SERIAL_WAIT:				lda CIA1ICR
							and #$08
							bne SERIAL_PASS
							dex
							bne SERIAL_WAIT
							dey
							bne SERIAL_OUTER

SERIAL_FAIL:				lda #$00
							sta CIA1CRA
							lda #$02
							sta SCREENBASE
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp SERIAL_FAIL

SERIAL_PASS:				lda #$00
							sta CIA1CRA
							lda #$01
							sta SCREENBASE
							lda #$05
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp SERIAL_PASS

; End.
