; VIC-20 VIA Timer 2 PHI2 one-shot regression.
; $0330 = 0 passes; 1 means no first IFR5; 2 means an unarmed second IFR5.

VIA2T2CL = $9128
VIA2T2CH = $9129
VIA2ACR = $912b
VIA2IFR = $912d
RESULT = $0330

* = $1000
MAIN:						sei
							lda #$00
							sta VIA2ACR
							lda #$20
							sta VIA2T2CL
							lda #$00
							sta VIA2T2CH
							ldy #$20
WAIT_OUTER:				ldx #$00
WAIT_FIRST:				lda VIA2IFR
							and #$20
							bne FIRST_OK
							dex
							bne WAIT_FIRST
							dey
							bne WAIT_OUTER
							lda #$01
							jmp FAIL
FIRST_OK:				lda VIA2T2CL
							ldx #$00
NO_REARM:				lda VIA2IFR
							and #$20
							bne SECOND_FAIL
							dex
							bne NO_REARM
PASS:						lda #$00
							sta RESULT
							jmp PASS
SECOND_FAIL:				lda #$02
FAIL:						sta RESULT
							jmp FAIL

; End.
