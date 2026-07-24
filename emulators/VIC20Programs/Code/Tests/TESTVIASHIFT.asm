; VIC-20 VIA shift-register regression for PHI2 and free-running modes.
; $0330 = 0 passes; 1 means no finite IFR2; 2 means read did not clear; 3 means free-run raised IFR2.

VIA2SR = $912a
VIA2ACR = $912b
VIA2IFR = $912d
RESULT = $0330

* = $1000
MAIN:						sei
							lda #$18
							sta VIA2ACR
							lda #$a5
							sta VIA2SR
							ldx #$00
WAIT_SHIFT:				lda VIA2IFR
							and #$04
							bne FINITE_OK
							dex
							bne WAIT_SHIFT
							lda #$01
							jmp FAIL
FINITE_OK:				lda VIA2SR
							lda VIA2IFR
							and #$04
							bne CLEAR_FAIL
							lda #$10
							sta VIA2ACR
							lda #$3c
							sta VIA2SR
							ldx #$00
FREE_WAIT:				lda VIA2IFR
							and #$04
							bne FREE_FAIL
							dex
							bne FREE_WAIT
PASS:						lda #$00
							sta RESULT
							jmp PASS
CLEAR_FAIL:				lda #$02
							jmp FAIL
FREE_FAIL:				lda #$03
FAIL:						sta RESULT
							jmp FAIL

; End.
