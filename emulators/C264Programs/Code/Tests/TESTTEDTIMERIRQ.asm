; C264 TED timer IRQ regression.
; $0330 = 0 passes; 1, 2 or 3 identify a missing timer flag;
; 4 identifies a missing summary IRQ; 5 identifies incorrect W1C acknowledgement.

TEDT1LO					= $ff00
TEDT1HI					= $ff01
TEDT2LO					= $ff02
TEDT2HI					= $ff03
TEDT3LO					= $ff04
TEDT3HI					= $ff05
TEDIRQSTATUS			= $ff09
TEDIRQMASK				= $ff0a
TEDCLOCK				= $ff13
RESULT					= $0330

TIMER1IRQ				= $08
TIMER2IRQ				= $10
TIMER3IRQ				= $40
SUMMARYIRQ				= $80
TIMER23IRQS				= $50
ALLTIMERIRQS			= $58

* = $1000
MAIN:						sei
							lda TEDCLOCK
							ora #$02
							sta TEDCLOCK

							lda #$00
							sta TEDIRQMASK
							lda #$ff
							sta TEDIRQSTATUS

							lda #$80
							sta TEDT1LO
							sta TEDT2LO
							sta TEDT3LO
							lda #$00
							sta TEDT1HI
							sta TEDT2HI
							sta TEDT3HI

							ldy #$20
WAIT_FLAGS:				lda TEDIRQSTATUS
							and #ALLTIMERIRQS
							cmp #ALLTIMERIRQS
							beq FLAGS_READY
							dey
							bne WAIT_FLAGS

							lda TEDIRQSTATUS
							and #TIMER1IRQ
							beq TIMER1_FAIL
							lda TEDIRQSTATUS
							and #TIMER2IRQ
							beq TIMER2_FAIL
							lda TEDIRQSTATUS
							and #TIMER3IRQ
							beq TIMER3_FAIL

FLAGS_READY:				lda #ALLTIMERIRQS
							sta TEDIRQMASK
							lda TEDIRQSTATUS
							and #SUMMARYIRQ
							beq SUMMARY_FAIL

							lda #TIMER1IRQ
							sta TEDIRQSTATUS
							lda TEDIRQSTATUS
							and #ALLTIMERIRQS
							cmp #TIMER23IRQS
							bne ACK_FAIL

							lda #TIMER2IRQ
							sta TEDIRQSTATUS
							lda TEDIRQSTATUS
							and #ALLTIMERIRQS
							cmp #TIMER3IRQ
							bne ACK_FAIL

							lda #TIMER3IRQ
							sta TEDIRQSTATUS
							lda TEDIRQSTATUS
							and #ALLTIMERIRQS
							bne ACK_FAIL

							lda #$00
							sta RESULT
PASS:						jmp PASS

TIMER1_FAIL:				lda #$01
							jmp FAIL
TIMER2_FAIL:				lda #$02
							jmp FAIL
TIMER3_FAIL:				lda #$03
							jmp FAIL
SUMMARY_FAIL:				lda #$04
							jmp FAIL
ACK_FAIL:					lda #$05
FAIL:						sta RESULT
							jmp FAIL

; End.
