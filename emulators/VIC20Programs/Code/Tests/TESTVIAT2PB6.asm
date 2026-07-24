; VIC-20 VIA Timer 2 PB6 negative-pulse regression.
; $0330 = 0 passes; 1 means early IFR5; 2 means missing IFR5; 3 means unwanted rearm.

VIA2ORB = $9120
VIA2DDRB = $9122
VIA2T2CL = $9128
VIA2T2CH = $9129
VIA2ACR = $912b
VIA2IFR = $912d
RESULT = $0330

* = $1000
MAIN:						sei
							lda #$40
							sta VIA2DDRB
							sta VIA2ORB
							lda #$20
							sta VIA2ACR
							lda #$02
							sta VIA2T2CL
							lda #$00
							sta VIA2T2CH
							lda #$00
							sta VIA2ORB
							lda VIA2IFR
							and #$20
							bne EARLY_FAIL
							lda #$40
							sta VIA2ORB
							lda #$00
							sta VIA2ORB
							lda VIA2IFR
							and #$20
							beq MISSING_FAIL
							lda VIA2T2CL
							lda #$40
							sta VIA2ORB
							lda #$00
							sta VIA2ORB
							lda VIA2IFR
							and #$20
							bne REARM_FAIL
PASS:						lda #$00
							sta RESULT
							jmp PASS
EARLY_FAIL:				lda #$01
							jmp FAIL
MISSING_FAIL:				lda #$02
							jmp FAIL
REARM_FAIL:				lda #$03
FAIL:						sta RESULT
							jmp FAIL

; End.
