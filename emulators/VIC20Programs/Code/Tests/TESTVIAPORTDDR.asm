; VIC-20 VIA port/DDR regression.
; $0330 = 0 passes; 1 means output pins failed; 2 means input pull-ups failed.

VIA2ORB = $9120
VIA2DDRB = $9122
RESULT = $0330

* = $1000
MAIN:						sei
							lda #$ff
							sta VIA2DDRB
							lda #$aa
							sta VIA2ORB
							lda VIA2ORB
							cmp #$aa
							bne OUTPUT_FAIL
							lda #$00
							sta VIA2DDRB
							lda VIA2ORB
							cmp #$ff
							bne INPUT_FAIL
PASS:						lda #$00
							sta RESULT
							jmp PASS
OUTPUT_FAIL:				lda #$01
							sta RESULT
							jmp OUTPUT_FAIL
INPUT_FAIL:				lda #$02
							sta RESULT
							jmp INPUT_FAIL

; End.
