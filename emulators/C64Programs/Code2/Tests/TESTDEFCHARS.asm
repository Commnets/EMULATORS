; C64 Assembler framework.
; To test the definition of new characters.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; The base has to be defined before doing anything...
; and also to include the general file!
BASE = $c000
#../C64Programs/code/routines/RGENERAL.asm
; The rest of the includes...
#../C64Programs/code/routines/RSCREENCLEAR.asm
#../C64Programs/code/routines/RCOPYCHARSET.asm

CHARDESTINATION = $3000

* = $ca00
; An example of character
BINARY 11100000
BINARY 11100000
BINARY 01110000
BINARY 00111000
BINARY 00011100
BINARY 00001110
BINARY 00000111
BINARY 00000111
; An example of character
BINARY 00000111
BINARY 00000111
BINARY 00001110
BINARY 00011100
BINARY 00111000
BINARY 01110000
BINARY 11100000
BINARY 11100000
; An example of character in multicolor...
BINARY 11100100
BINARY 11100100
BINARY 11100100
BINARY 11100100
BINARY 11100100
BINARY 11100100
BINARY 11100100
BINARY 11100100
CHARDEF = $ca00

* = $cb00
; A very simple program to check whether copying the chars works...
MAIN:						lda #$06
							sta VICIIBACKGROUND
							lda #$0e
							sta VICIIFOREGROUND

COPYCHARS:					lda #<CHARDESTINATION
							sta COPYCHAR_TEXTHIGHVAR
							jsr COPYCHARSET

SETGRAPHICMODE:				lda VICIISCROLLX					; Sets the bit 4 on... multicolor text mode...
							and #$ef
							ora #$10
							sta VICIISCROLLX
							lda VICIISCROLLX					; Just to test if the shift doesn't affect!
							and #$f8
							ora #$07
							sta VICIISCROLLX
							lda #$07
							sta VICIIBACKGROUND1
							lda #$08
							sta VICIIBACKGROUND2
							lda #$09
							sta VICIIBACKGROUND3

DEFCHARS:					ldy #$00
DEFCHARS_LOOP:				lda CHARDEF,y
							sta CHARDESTINATION,y
							iny
							cpy #$18							; Only two chars to test
							bne DEFCHARS_LOOP

NEWCHARON:					jsr ACTIVATECHARSET					; The new char set is activated

SHOWCHARS:					lda #$00
							sta SCREENBASE
							lda #$01
							sta SCREENBASE + 1
							lda #$02
							sta SCREENBASE + 2
							lda #$01
							sta COLORRAMBASE
							sta COLORRAMBASE + 1
							lda #$0a							; Flag multicolor...
							sta COLORRAMBASE + 2

FOREVER:					jmp FOREVER							; For ever...
