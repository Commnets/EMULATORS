; Routines to manage chars the screen
; By Ignacio Cea Forniés
; Copyright Community Networks 2022 - 2023

#../C64Programs/RGENERAL.asm

; Macros
SCREENBASE					= $0400							; Where usually the screen starts
SCREEN1_256BLK				= $0400							; The screen can be easly dividev in blocks of 256K
SCREEN2_256BLK				= $0500
SCREEN3_256BLK				= $0600
SCREEN4_256BLK				= $0700
COLORRAMBASE				= $D800							; Where the ram color starts
COLORRAM1_256BLK			= $D800							; The color am can be easly dividev in blocks of 256K
COLORRAM2_256BLK			= $D900
COLORRAM3_256BLK			= $DA00
COLORRAM4_256BLK			= $DB00
VICIIBACKGROUND				= $D020							; Address in VICII for the background
VICIIFOREGROUND				= $D021							; Address in VICII for the foreground
VICIISCROLLX				= $D016							; Address in VICII to SCROLL in the X axis
VICIISCROLLY				= $D011							; Address in VICII to SCROLL in the Y axis

; Variables used to scroll smoothly
SCROLLSMOOTHX_VAR			= VARIABLESDATA_ZONE + 0		; The value of the current smooth scroll in x axis (from 0 to 7)
SCROLLSMOOTHY_VAR			= VARIABLESDATA_ZONE + 1		; The value of the current smooth scroll in y axis (from 0 to 7)

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
; To scroll smoothly the screen in the x axis to the right
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
