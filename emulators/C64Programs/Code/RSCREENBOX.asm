; C64 Assembler framework.
; Routine to create a box in the screen.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

#../C64Programs/Code/RGENERAL.asm
#../C64Programs/Code/RGRAPHICS.asm

; ------------------------------
; FILLBOXSCREEN
; Fill a box of the screen with a character and with a color.
; The parameters used by this routine are:
; FILLSCR_XPOSVAR		:	The x position of the left up side corner of the screen box.
; FILLSCR_XLENVAR		:	X len of the box. From 1 to 40 ($01 - $28). The result with other values is not guaranteed.
; FILLSCR_YPOSVAR		:	The y position of the left up side corner of the screen box.
; FILLSCR_YLENVAR		:	Y len of the box. From 1 to 25 ($01 - $19). The result with other values is not guaranteed.
; FILLSCR_BYTEVAR		:	The value to fill the box with.
; FILLSCR_COLORVAR		:	The value of the color to fill the box with.
FILLSCR_XPOSVAR				= FILLMX_XPOSVAR
FILLSCR_XLENVAR				= FILLMX_XLENVAR
FILLSCR_YPOSVAR				= FILLMX_YPOSVAR
FILLSCR_YLENVAR				= FILLMX_YLENVAR
FILLSCR_BYTEVAR				= FILLMX_BYTEVAR
FILLSCR_COLORVAR			= VARIABLES_DATAZONE + 8
; First draws the character...
FILLBOXSCREEN:				lda #>SCREENBASE
							sta FILLMX_LOCSLOWVAR
							lda #<SCREENBASE
							sta FILLMX_LOCSHIGHVAR
							lda #$28								; Max number of text columns in the screen/matrix.
							sta FILLMX_MAXXVAR
							jsr FILLMATRIX
; ...and then the color.
							lda #>COLORRAMBASE
							sta FILLMX_LOCSLOWVAR
							lda #<COLORRAMBASE
							sta FILLMX_LOCSHIGHVAR
							lda FILLSCR_COLORVAR
							sta FILLMX_BYTEVAR
							jsr FILLMATRIX
							rts
; ------------------------------

; ------------------------------
; FILLBOXSCREENIN
; Fill a box of the screen with a character and with a color, but always with the limits of the screen.
; The parameters used by this routine are the same than the previous one.
; The other routine will be invoked but limiting the size of the box.
; The initial data of the invocation is maintained.
; Adjust the columns first...
FILLBOXSCREENIN:			lda FILLSCR_YLENVAR						; First of all, saves the data that might change...
							pha
							lda FILLSCR_YPOSVAR
							pha
							tay										; Saved for further analysis...
							lda FILLSCR_XLENVAR
							pha
							lda FILLSCR_XPOSVAR						; Starts to wotk with the columns first...
							pha
							bmi FILLBOXSCREENIN_XPOSNEG				; The left side of the box is out of the visible zone, but what about the right one?
							cmp #$28
							bpl FILLBOXSCREENIN_RTS					; The left side of the box is out of the visibe zone so nothing else to do...
							jmp FILLBOXSCREENIN_XPOSPOS				; The left side is in the visible zone, but what about the end?
FILLBOXSCREENIN_XPOSNEG:	clc										; The left side was out the visible zone, but what about the end?
							adc FILLSCR_XLENVAR
							bmi FILLBOXSCREENIN_RTS					; The box is not within the visible zone so nothing else to do...
							cmp #$28
							bpl FILLBOXSCREENIN_ALLXIN				; The columns of the box fills the full screen width! 
							sta FILLSCR_XLENVAR						; The box is partially in the visible zone (remember that it started outside), but the length has to be adjusted...
							jmp FILLBOXSCREENIN_XPOS0				; ...and the initial position too (to 0)
FILLBOXSCREENIN_ALLXIN:		lda #$28								; The number of columns occupied are all...				
							sta FILLSCR_XLENVAR
FILLBOXSCREENIN_XPOS0:		lda #$00
							sta FILLSCR_XPOSVAR
FILLBOXSCREENIN_XPOSPOS:	clc
							adc FILLSCR_XLENVAR						; The left side was at lest within the visible columns, but what about the right one?
							cmp #$28								; 
							bpl FILLBOXSCREENIN_TESTYLEN			; The right side is out of the visible zone, so adjust it....
							sec
							sbc #$28
							sta FILLSCR_XLENVAR
; Now the rows are adjusted...
FILLBOXSCREENIN_TESTYLEN:	tya										; Starts to test the rows, with a similar rational...
							bmi FILLBOXSCREENIN_YPOSNEG
							cmp #$28
							bpl FILLBOXSCREENIN_RTS
							jmp FILLBOXSCREENIN_YPOSPOS
FILLBOXSCREENIN_YPOSNEG:	clc										; The left side was out the visible zone, but what about the end?
							adc FILLSCR_YLENVAR
							bmi FILLBOXSCREENIN_RTS
							cmp #$19
							bpl FILLBOXSCREENIN_ALLYIN
							sta FILLSCR_YLENVAR
							jmp FILLBOXSCREENIN_YPOS0
FILLBOXSCREENIN_ALLYIN:		lda #$19
							sta FILLSCR_YLENVAR
FILLBOXSCREENIN_YPOS0:		lda #$00
							sta FILLSCR_YPOSVAR
FILLBOXSCREENIN_YPOSPOS:	clc
							adc FILLSCR_YLENVAR
							cmp #$19
							bpl FILLBOXSCREENIN_DRAW
							sec
							sbc #$19
							sta FILLSCR_YLENVAR
FILLBOXSCREENIN_DRAW:		jsr FILLBOXSCREEN						; Finally calls the original routine...
FILLBOXSCREENIN_RTS:		pla										; Put the original values back....
							sta FILLSCR_XPOSVAR
							pla
							sta FILLSCR_XLENVAR
							pla
							sta FILLSCR_YPOSVAR
							pla
							sta FILLSCR_YLENVAR 
							rts
; ------------------------------

; End.