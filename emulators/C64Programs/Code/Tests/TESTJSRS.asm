; C64 Assembler framework.
; To test the nested JSR
; By Ignacio Cea Fornies.
; Copyright Community Networks 2022 - 2025.

* = $cb00
; A very simple program to test everything works...
MAIN:						
							jsr $cb10
							lda#$01
FOREVER:
							jmp FOREVER

* = $cb10
							jsr $cb20
							lda#$02
							rts
* = $cb20
							lda#$03
							rts
; End.