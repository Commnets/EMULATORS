; To test the routines
; By Ignacio Cea Forniés
; Copyright Community Networks 2022 - 2023

; The base has to be defined before doing anything...
BASE = $C000

; TEMPLATES used in many parts of the program

; To save the main registers in the stack
{ SAVEREGISTERS
							PHA									; Starting from the accumulator...
							TXA
							PHA
							TXY
							PHA
}
							
; To recover the main registers from the stack
{ RECOVERREGISTERS
							PLA
							TAY
							PLA
							TAX
							PLA									; Ending with the accumulator...
}

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

; Routines dedicated to manage the graphics

; Macros
SCREENBASE					= $0400							; Where usually the screen starts
SCREEN1_256BLK				= $0400							; The screen can be easly divided in blocks of 256K...
SCREEN2_256BLK				= $0500							; ...the next variables referes to those blocks
SCREEN3_256BLK				= $0600
SCREEN4_256BLK				= $0700
COLORRAMBASE				= $D800							; Where the ram color starts
COLORRAM1_256BLK			= $D800							; The color RAM can be easly dividev in blocks of 256K
COLORRAM2_256BLK			= $D900							; ...the next variables referes to those blocks
COLORRAM3_256BLK			= $DA00
COLORRAM4_256BLK			= $DB00
VICIIBACKGROUND				= $D020							; VICII Address for the background
VICIIFOREGROUND				= $D021							; VICII Address for the foreground
VICIISCROLLX				= $D016							; VICII Address to SCROLL in the X axis
VICIISCROLLY				= $D011							; VICII Address to SCROLL in the Y axis

; To clear the screen
CLEARSCREEN:				LDA #$20
							LDX #$00
LOOPCLEARSCREEN:			STA SCREEN1_256BLK,X
							STA SCREEN2_256BLK,X
							STA SCREEN3_256BLK,X
							STA SCREEN4_256BLK,X
							INX
							BNE LOOPCLEARSCREEN
							RTS

; --- SMOOTH SCROLL ---
; Variables used to scroll smoothly
SCROLLSMOOTHX_VAR			= VARIABLES_DATAZONE + 0		; The value of the current smooth scroll in x axis (from 0 to 7)
SCROLLSMOOTHY_VAR			= VARIABLES_DATAZONE + 1		; The value of the current smooth scroll in y axis (from 0 to 7)
; To scroll smoothly the screen in the x axis to the right once
; The variable SCROLLSMOOTH_VAR1 keeps the actual value of the smooth (from 0 to 7)
SCROLLXSMOOTHRIGHT:			LDA VICIISCROLLX
							AND #$F8
							ORA SCROLLSMOOTHX_VAR
							STA VICIISCROLLX
							INC SCROLLSMOOTHX_VAR
							RTS

; To scroll smoothly the screen in the x axis to the left
; The variable SCROLLSMOOTH_VAR1 keeps the actual value of the smooth (from 7 to 0)
SCROLLXSMOOTHLEFT:			LDA VICIISCROLLX
							AND #$F8
							ORA SCROLLSMOOTHX_VAR
							STA VICIISCROLLX
							DEC SCROLLSMOOTHX_VAR
							RTS

; To scroll smoothly the screen in the y axis to the bottom
; The variable SCROLLSMOOTH_VAR2 keeps the actual value of the smooth (from 0 to 7)
SCROLLYSMOOTHDOWN:			LDA VICIISCROLLY
							AND #$F8
							ORA SCROLLSMOOTHY_VAR
							STA VICIISCROLLY
							INC SCROLLSMOOTHY_VAR
							RTS

; To scroll smoothly the screen in the y axis to the top
; The variable SCROLLSMOOTH_VAR2 keeps the actual value of the smooth (from 7 to 0)
SCROLLYSMOOTHUP:			LDA VICIISCROLLY
							AND #$F8
							ORA SCROLLSMOOTHY_VAR
							STA VICIISCROLLY
							DEC SCROLLSMOOTHY_VAR
							RTS

; A very simple program to test everything works...
* = $C100
MAIN:						JSR CLEARSCREEN
MAINLOOP:					JMP MAINLOOP					; For ever...

; End