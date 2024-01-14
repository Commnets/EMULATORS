; VIC20 Assembler framework.
; To test the VIA 2 working.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2023 - 2024.

* = $1000
							lda #$01
							sta $9114
							lda #$00
							sta $9115
FOREVER:					jmp FOREVER							; For ever...

; End.