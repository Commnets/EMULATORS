; To test the routines
; By Ignacio Cea Forniés
; Copyright Community Networks 2022 - 2023

; The base has to be defined before doing anything...
BASE = $C000

#../C64Programs/Code/RGRAPHICS.asm

; A very simple program to test everything works...
* = $C100
MAIN:						JSR CLEARSCREEN
MAINLOOP:					JMP MAINLOOP					; For ever...

; End