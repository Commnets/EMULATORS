; C264 TED timer clock-phase regression.
; Exercises an odd number of double-clock CPU cycles before forcing single clock.
; $0330 = 0 when the trace is complete.
; $0331-$0338 contain the Timer 2 samples for comparison with the debug log.

TEDT2LO					= $ff02
TEDT2HI					= $ff03
TEDCLOCK				= $ff13
RESULT					= $0330
OBSERVED				= $0331

* = $1000
MAIN:						sei
							lda #$ff
							sta RESULT

							lda TEDCLOCK
							and #$fd							; Allow double clock.
							sta TEDCLOCK

							lda #$00
							sta TEDT2LO
							lda #$40
							sta TEDT2HI

							lda TEDT2LO
							sta OBSERVED
							lda TEDT2HI
							sta OBSERVED + 1

							nop
							bit RESULT							; Five CPU cycles together with the NOP.

							lda TEDCLOCK
							ora #$02							; Force single clock.
							sta TEDCLOCK

							lda TEDT2LO
							sta OBSERVED + 2
							lda TEDT2HI
							sta OBSERVED + 3

							nop
							nop

							lda TEDT2LO
							sta OBSERVED + 4
							lda TEDT2HI
							sta OBSERVED + 5

							lda TEDCLOCK
							and #$fd							; Return to double clock.
							sta TEDCLOCK

							lda TEDT2LO
							sta OBSERVED + 6
							lda TEDT2HI
							sta OBSERVED + 7

							lda #$00
							sta RESULT
END:						jmp END

; End.
