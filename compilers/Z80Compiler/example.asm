; A very simple test Z80 compiler program

* = $C000
MAIN:	LD HL,$0010
LOOP:	LD (HL),$02
		SET 7,(HL)
		DEC	HL
		CP	H
		JR	NZ,LOOP