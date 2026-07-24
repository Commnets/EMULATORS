; C64 Assembler framework.
; CIA1 Timer A latch/counter loading regression.
; Green means running writes changed only the latch and force-load copied it later.
; $0401/$0402 keep the stopped counter value observed before force-load.
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

							lda #$00
							sta CIA1TALO
							lda #$08
							sta CIA1TAHI
							lda #$11
							sta CIA1CRA

							lda #$34							; Update latch to $1234 while Timer A is running.
							sta CIA1TALO
							lda #$12
							sta CIA1TAHI
							lda #$00
							sta CIA1CRA

							lda CIA1TALO
							sta SCREENBASE + 1
							tax
							lda CIA1TAHI
							sta SCREENBASE + 2
							cmp #$12
							bne LATCH_FORCE
							cpx #$34
							beq LATCH_FAIL_RUNNING

LATCH_FORCE:				lda #$10							; Force-load while stopped.
							sta CIA1CRA
							lda CIA1TALO
							cmp #$34
							bne LATCH_FAIL_FORCE
							lda CIA1TAHI
							cmp #$12
							bne LATCH_FAIL_FORCE

LATCH_PASS:					lda #$01
							sta SCREENBASE
							lda #$05
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp LATCH_PASS

LATCH_FAIL_RUNNING:			ldx #$01
							jmp LATCH_FAIL
LATCH_FAIL_FORCE:			ldx #$02
LATCH_FAIL:					stx SCREENBASE + 3
							lda #$02
							sta SCREENBASE
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp LATCH_FAIL

; End.
