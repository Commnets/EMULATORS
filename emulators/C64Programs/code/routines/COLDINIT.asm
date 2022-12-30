; C64 Assembler framework.
; Routine to copy the set o characters from to a position into rhe RAM.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; ------------------------------
; COLDINIT
; Just do what reset routine does but not jumping to Basic.
COLDINIT:					sei
							cld
							jsr $fd02
							bne $03
							jmp ($8000)
							stx $d016
							jsr $fda3								; Initialize CIA registers
							jsr $fd50								; Initialize Memory pointers
							jsr $fd15								; Initialize Kernal RAM vectors
							jsr $ff5b								; Initialize VICII registers
							cli

							rts

; End.