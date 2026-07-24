; C264 TED timer FREEZE regression.
; $0330 = 0 passes; 1 means the timer moved while frozen;
; 2 means elapsed frozen cycles were recovered after unfreezing.
; $0331-$0336 contain the counter samples.

TEDT2LO					= $ff02
TEDT2HI					= $ff03
TEDCR2					= $ff07
TEDCLOCK				= $ff13
RESULT					= $0330
OBSERVED				= $0331

* = $1000
MAIN:						sei
							lda #$ff
							sta RESULT

							lda TEDCLOCK
							ora #$02							; Keep CPU and timer in the same clock domain.
							sta TEDCLOCK

							lda #$00
							sta TEDT2LO
							lda #$40
							sta TEDT2HI

							lda TEDCR2
							ora #$20							; Freeze raster and timers.
							sta TEDCR2

							lda TEDT2LO
							sta OBSERVED
							lda TEDT2HI
							sta OBSERVED + 1

							ldy #$20
WAIT_OUTER:				ldx #$00
WAIT_INNER:				dex
							bne WAIT_INNER
							dey
							bne WAIT_OUTER

							lda TEDT2LO
							sta OBSERVED + 2
							cmp OBSERVED
							bne MOVED_DURING_FREEZE
							lda TEDT2HI
							sta OBSERVED + 3
							cmp OBSERVED + 1
							bne MOVED_DURING_FREEZE

							lda TEDCR2
							and #$df							; Resume the TED.
							sta TEDCR2

							nop
							nop
							lda TEDT2LO
							sta OBSERVED + 4
							lda TEDT2HI
							sta OBSERVED + 5

							cmp OBSERVED + 1					; A short resume must not consume the long frozen delay.
							bne RECOVERED_FROZEN_CYCLES
							lda #$00
							sta RESULT
PASS:						jmp PASS

MOVED_DURING_FREEZE:		lda #$01
							jmp FAIL
RECOVERED_FROZEN_CYCLES:	lda #$02
FAIL:						sta RESULT
							jmp FAIL

; End.
