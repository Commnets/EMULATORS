; VIC-20 VIA Timer 1 PB7 one-shot regression.
; PB7 must go low on T1C-H load and high at time-out. $0330 = 0 passes.

VIA2ORB = $9120
VIA2DDRB = $9122
VIA2T1CL = $9124
VIA2T1CH = $9125
VIA2ACR = $912b
VIA2IFR = $912d
RESULT = $0330

* = $1000
MAIN:						sei
							lda #$80
							sta VIA2DDRB
							sta VIA2ACR
							lda #$20
							sta VIA2T1CL
							lda #$00
							sta VIA2T1CH
							lda VIA2ORB
							and #$80
							bne LOAD_FAIL
WAIT:						lda VIA2IFR
							and #$40
							beq WAIT
							lda VIA2ORB
							and #$80
							beq TIMEOUT_FAIL
PASS:						lda #$00
							sta RESULT
							jmp PASS
LOAD_FAIL:				lda #$01
							sta RESULT
							jmp LOAD_FAIL
TIMEOUT_FAIL:			lda #$02
							sta RESULT
							jmp TIMEOUT_FAIL

; End.
