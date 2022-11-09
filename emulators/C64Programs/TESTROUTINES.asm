; To test the routines
; By Ignacio Cea Forniés
; Copyright Community Networks 2022 - 2023

; The base has to be defined
BASE = $C000

#../C64Programs/RGRAPHICS.asm

; A very simple program...
MAIN:						JSR CLEARSCREEN
MAINLOOP:					JMP MAINLOOP					; For ever...
