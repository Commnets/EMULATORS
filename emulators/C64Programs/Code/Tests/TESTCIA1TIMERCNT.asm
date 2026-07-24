; C64 Assembler framework.
; CIA1 Timer A CNT positive-edge regression.
; The border stays yellow while waiting for two rising edges at user-port CNT.
; A falling edge must not decrement the timer. Green means underflow was detected.
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

							lda #$02
							sta CIA1TALO
							lda #$00
							sta CIA1TAHI
							lda #$31							; CNT source, force load and start.
							sta CIA1CRA
							lda #$07
							sta VICIIFOREGROUND

CNT_WAIT:					lda CIA1TALO
							sta SCREENBASE + 1
							lda CIA1ICR
							and #$01
							beq CNT_WAIT

CNT_PASS:					lda #$00
							sta CIA1CRA
							lda #$01
							sta SCREENBASE
							lda #$05
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp CNT_PASS

; End.
