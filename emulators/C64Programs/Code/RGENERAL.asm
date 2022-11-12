; General routines
; By Ignacio Cea Forniés
; Copyright Community Networks 2022 - 2023

#../C64Programs/Code/TEMPLATES.asm

; General routines used in different parts of the program

; Very general zone to store variables
; THE VARIABLE BASE has to be defined before this file is loaded
* = BASE
; A zone of 48 bytes for general data is defined
BYTES $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00
BYTES $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00
BYTES $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00
; Temporary variables are defined in the first $10 (16) positions
TEMP0_DATA					= BASE + 0
TEMP1_DATA					= BASE + 1
TEMP2_DATA					= BASE + 2
TEMP3_DATA					= BASE + 3
TEMP4_DATA					= BASE + 4
TEMP5_DATA					= BASE + 5
TEMP6_DATA					= BASE + 6
TEMP7_DATA					= BASE + 7
TEMP8_DATA					= BASE + 8
TEMP9_DATA					= BASE + 9
TEMP10_DATA					= BASE + 10
TEMP11_DATA					= BASE + 11
TEMP12_DATA					= BASE + 12
TEMP13_DATA					= BASE + 13
TEMP14_DATA					= BASE + 14
TEMP15_DATA					= BASE + 15
; The variables for the routines start from the position $10 (16) onwards
; So there is at least 32 positions for defining variables
VARIABLES_DATAZONE			= BASE + 16

; To create a delay
; The length of the delay is passed into the Registers X and Y
DELAY:						TXA
LOOPDELAY:					NOP
							DEX
							BNE LOOPDELAY
							TAX
							DEY
							BNE LOOPDELAY
							RTS

; End