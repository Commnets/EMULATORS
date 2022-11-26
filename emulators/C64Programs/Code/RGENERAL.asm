; General routines
; By Ignacio Cea Forniés
; Copyright Community Networks 2022 - 2023

#../C64Programs/Code/TEMPLATES.asm

; General routines used in different parts of the program

; Very general zone to store variables
; Defined in the page 0 just to speed up their use!
* = $02
; A zone of 48 bytes for general data is defined
BYTES $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00
BYTES $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00
BYTES $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00
; Temporary variables are defined in the first $10 (16) positions
TEMP0_DATA					= $2
TEMP1_DATA					= $3
TEMP2_DATA					= $4
TEMP3_DATA					= $5
TEMP4_DATA					= $6
TEMP5_DATA					= $7
TEMP6_DATA					= $8
TEMP7_DATA					= $9
TEMP8_DATA					= $a
TEMP9_DATA					= $b
TEMP10_DATA					= $c
TEMP11_DATA					= $d
TEMP12_DATA					= $e
TEMP13_DATA					= $f
TEMP14_DATA					= $10
TEMP15_DATA					= $11
; The variables for the routines start from the position $12 (18) to $21 (33)
; So there is 16 positions for defining variables to feed routines
VARIABLES_DATAZONE			= $12
; The variables to produce results start from the position $22 (34) to $31 (49)
; So there is 16 positions for defining outcome for rotuines
OUTCOME_DATAZONE			= $22

; The variable BASE has to be defined before this file is loaded
; The pograms always should start from where this variable is defined onwards
* = BASE

; To create a delay
; The length of the delay is passed into the Registers X and Y
DELAY:						txa
DELAY_LOOP:					nop
							dex
							bne DELAY_LOOP
							tax
							dey
							bne DELAY_LOOP
							rts

; End