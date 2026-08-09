; C64 Assembler framework.
; CIA1 serial-output regression continuing an active transfer with one-shot Timer A underflows.
; Green means that the serial ICR flag was raised; red means that the test is still waiting.
; By Ignacio Cea Fornies.

BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm

CIA1TALO					= $dc04
CIA1TAHI					= $dc05
CIA1SDR					= $dc0c
CIA1ICR					= $dc0d
CIA1CRA					= $dc0e
RESULT						= $02

* = $cb00
MAIN:						sei
							lda #$02
							sta SCREENBASE
							sta COLORRAMBASE
							sta VICIIFOREGROUND

							lda #$00
							sta CIA1CRA
							lda #$7f
							sta CIA1ICR
							lda CIA1ICR

							lda #$ff
							sta CIA1TALO
							lda #$00
							sta CIA1TAHI
							lda #$a5
							sta CIA1SDR
							lda #$51							; Arm the transfer without reaching a continuous underflow.
							sta CIA1CRA
							lda #$40
							sta CIA1CRA
							lda #$01
							sta CIA1TALO
							lda #$00
							sta CIA1TAHI

NEXT_ONE_SHOT:				lda #$59							; Continue it with one-shot force-load/start cycles.
							sta CIA1CRA
WAIT_ONE_SHOT:				lda CIA1ICR
							sta RESULT
							and #$01
							beq WAIT_ONE_SHOT
							lda RESULT
							and #$08
							beq NEXT_ONE_SHOT

SERIAL_PASS:				lda #$00
							sta CIA1CRA
							lda #$01
							sta SCREENBASE
							lda #$05
							sta COLORRAMBASE
							sta VICIIFOREGROUND
							jmp SERIAL_PASS

; End.
