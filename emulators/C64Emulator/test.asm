; Simple test that everything works
; By Ignacio Cea

; MACROS
FOREGROUND = $D020
BACKGROUND = $D021

* = $C000

; Now the code

START:			LDA #$00
				STA BACKGROUND
				STA FOREGROUND
				BNE START

; Very simple
