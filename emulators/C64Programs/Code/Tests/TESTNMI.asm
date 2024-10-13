; Disable NMI by Ninja/The Dreams/TempesT

*=$cb00
MAIN:
		SEI				; disable IRQ
		LDA #>NMIA
		STA $0318		; change NMI vector
		LDA #<NMIA		; to our routine
		STA $0319
		LDA #$00		; stop Timer A
		STA $DD0E
		STA $DD04		; set Timer A to 0, after starting
		STA $DD05		; NMI will occur immediately
		LDA #$81
		STA $DD0D		; set Timer A as source for NMI
		LDA #$01
		STA $DD0E		; start Timer A -> NMI

						; from here on NMI is disabled
LOOP:
		INC $0400		; change screen memory, proves computer is alive
		LDA #$10		; SPACE pressed?
		AND $DC01
		BNE NOSPC		; if not, branch
		LDA #$81		; if yes, clear Timer A
		STA $DD0D		; as NMI source
		LDA $DD0D		; acknowledge NMI, i.e. enable it

NOSPC:
		JMP LOOP		; endless loop

*=$cc00
NMIA=$cc00
NMI:
		INC $D020		; change border colour, indication for a NMI
		RTI				; exit interrupt
						; (not acknowledged!)
