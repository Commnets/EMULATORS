; C264 TED timer register-load regression.
; $0330 = 0 passes; 1 means a low-byte write did not stop Timer 2;
; 2 means the high-byte write did not restart it.
; $0331-$0336 contain the observed values.

TEDT2LO					= $ff02
TEDT2HI					= $ff03
TEDCLOCK				= $ff13
RESULT					= $0330
OBSERVED				= $0331

* = $1000
MAIN:						sei
							lda TEDCLOCK
							ora #$02
							sta TEDCLOCK

							lda #$00
							sta TEDT2LO
							lda #$40
							sta TEDT2HI

							lda #$34							; A low-byte write must stop the timer.
							sta TEDT2LO
							lda TEDT2LO
							sta OBSERVED
							lda TEDT2HI
							sta OBSERVED + 1

							ldx #$00
WAIT_STOPPED:				dex
							bne WAIT_STOPPED

							lda TEDT2LO
							sta OBSERVED + 2
							cmp OBSERVED
							bne STOP_FAIL
							lda TEDT2HI
							sta OBSERVED + 3
							cmp OBSERVED + 1
							bne STOP_FAIL

							lda #$12							; The high-byte write completes $1234 and starts.
							sta TEDT2HI
							nop
							nop
							lda TEDT2LO
							sta OBSERVED + 4
							lda TEDT2HI
							sta OBSERVED + 5

							cmp #$12
							bne STARTED
							lda OBSERVED + 4
							cmp #$34
							beq START_FAIL

STARTED:					lda #$00
							sta RESULT
PASS:						jmp PASS

STOP_FAIL:					lda #$01
							jmp FAIL
START_FAIL:					lda #$02
FAIL:						sta RESULT
							jmp FAIL

; End.
