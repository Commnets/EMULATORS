; VIC-20 VIA CA2 handshake trace test.
; Observe VIA1 CA2 in the debug log: each ORA access must drive CA2 low,
; and the configured active CA1 edge must return it high.

VIA1ORA = $9111
VIA1PCR = $911c
MARKER = $0330

* = $1000
MAIN:						sei
							lda #$08
							sta VIA1PCR
							lda VIA1ORA
							lda #$01
							sta MARKER
							lda #$55
							sta VIA1ORA
							lda #$02
							sta MARKER
FOREVER:					jmp FOREVER

; End.
