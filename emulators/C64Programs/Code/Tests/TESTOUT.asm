; C64 Assembler framework.
; To test the Out routine.
; By Ignacio Cea Fornies.
; Copyright Community Networks 2022 - 2023.

* = $cb00
; A very simple program to test everything works...
MAIN:						ldx #$01
LOOP:						inx
							cpx #$05
							bmi LOOP
FOREVER:					brk

; End.