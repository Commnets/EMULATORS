; C64 Assembler framework.
; CIA1 Timer A PB6 toggle-output regression.
; Green means PB6 changed state on Timer A underflow.
; By Ignacio Cea Fornies.

BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm

CIA1PRB					= $dc01
CIA1DDRB					= $dc03
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
							lda CIA1DDRB
							ora #$40
							sta CIA1DDRB

							lda #$20
							sta CIA1TALO
							lda #$00
							sta CIA1TAHI
							lda #$17							; PBON, toggle, force load and start.
							sta CIA1CRA
							lda CIA1PRB
							and #$40
							sta TEMP00_DATA

							ldy #$20
PB_OUTER:					ldx #$00
PB_WAIT:					lda CIA1ICR
							and #$01
							bne PB_CHECK
							dex
							bne PB_WAIT
							dey
							bne PB_OUTER
							ldx #$01
							jmp PB_FAIL

PB_CHECK:					lda CIA1PRB
							eor TEMP00_DATA
							and #$40
							bne PB_PASS
							ldx #$02
							jmp PB_FAIL

PB_PASS:					lda #$00
							sta CIA1CRA
							lda #$01
							sta SCREENBASE
							lda #$05
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp PB_PASS

PB_FAIL:					stx SCREENBASE + 1
							lda #$00
							sta CIA1CRA
							lda #$02
							sta SCREENBASE
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp PB_FAIL

; End.
