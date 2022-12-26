; C64 Assembler framework.
; To test the Out routine.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

* = $cb00
; A very simple program to test everything works...
MAIN:						lda #$01
							sta $0400 + (24 * 40)
FOREVER:					jmp FOREVER

; End.