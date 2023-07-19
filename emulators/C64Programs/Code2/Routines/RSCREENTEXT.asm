; C64 Assembler framework.
; Routine to draw a text in the screen.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

#../C64Programs/code/macros/MSCREEN.asm
#../C64Programs/code/routines/RGENERAL.asm

; ------------------------------
; DRAWTEXT
; To draw a text in the screen
; DRAWTEXT_XPOSVAR		:	The x position of the first letter of the text.
; DRAWTEXT_YPOSVAR		:	The y position of the first letter of the text.
; DRAWTEXT_TEXTLENVAR	:	Text length.
; DRAWTEXT_TEXTLOWVAR	:	Low byte of the address where the text is kept.
; DRAWTEXT_TEXTHIGHVAR	:	High byte of the address where the text is kept.
; DRAWTEXT_COLORVAR		:	Color of the text.
DRAWTEXT_XPOSVAR			= MATRIXADDR_XPOSVAR
DRAWTEXT_YPOSVAR			= MATRIXADDR_YPOSVAR
DRAWTEXT_TEXTLENVAR			= VARIABLES_DATAZONE + 5
DRAWTEXT_TEXTLOWVAR			= VARIABLES_DATAZONE + 6
DRAWTEXT_TEXTHIGHVAR		= VARIABLES_DATAZONE + 7
DRAWTEXT_COLORVAR			= VARIABLES_DATAZONE + 8
; Draw text.
DRAWTEXT:					.SAVEREGISTERS
							lda #>SCREENBASE
							sta MATRIXADDR_LOCLOWVAR
							lda #<SCREENBASE
							sta MATRIXADDR_LOCHIGHVAR
							lda #$28
							sta MATRIXADDR_MAXXVAR
							jsr MATRIXADDRESS					; Outcome is in MATRIXADDR_LOCLOWRST and in MATRIXADDR_LOCHIGHRST.
							ldy DRAWTEXT_TEXTLENVAR
DRAWTEXT_LOOPLETTERS:		dey
							lda (DRAWTEXT_TEXTLOWVAR),y
							sta (MATRIXADDR_LOCLOWRST),y
							cpy #$00
							bne DRAWTEXT_LOOPLETTERS
; And then put in color
							lda #>COLORRAMBASE
							sta MATRIXADDR_LOCLOWVAR
							lda #<COLORRAMBASE
							sta MATRIXADDR_LOCHIGHVAR
							jsr MATRIXADDRESS
							ldy DRAWTEXT_TEXTLENVAR
							lda DRAWTEXT_COLORVAR
DRAWTEXT_LOOPCOLOR:			dey
							sta (MATRIXADDR_LOCLOWRST),y
							bne DRAWTEXT_LOOPCOLOR
							.RECOVERREGISTERS
							rts
; ------------------------------

; ------------------------------
; DRAWTEXTIN
; Draw a text within the screen taking into account that the initial position could be aout of the screen
; The parameters used by this routine are the same than the previous one.
; The other routine will be finally invoked but changing the initial data.
DRAWTEXTIN:					.SAVEREGISTERS
							lda DRAWTEXT_TEXTLOWVAR				; First of all, saves the data that might change...
							pha
							lda DRAWTEXT_TEXTHIGHVAR
							pha
							lda DRAWTEXT_TEXTLENVAR
							pha
							lda DRAWTEXT_XPOSVAR
							pha
							tay									; Saves the X position int the Y register as it will be used later...
							lda DRAWTEXT_YPOSVAR				; Starts checking the Y position...
							bmi DRAWTEXTIN_RTS					; The text is out of the visible zone (very up) so nothing else to do...
							cmp #$19
							bpl DRAWTEXTIN_RTS					; The text is out of the visible zone (very down) so nothing else to do...
							tya									; Time to check the X position. Recovers the X position to deal with it...
							bmi	DRAWTEXTIN_XPOSNEG				; The text starts before the beginning of the visible zone (very left), but it could still end up within the visible zone...
							cmp #$28
							bpl DRAWTEXTIN_RTS					; The text is out of the visible zone (very right) so nothing else to do...
							clc									; The text starts in the visibe zone, but what about the end?
							adc DRAWTEXT_TEXTLENVAR
DRAWTEXTIN_XPOSPOS:			cmp #$28
							bmi DRAWTEXTIN_DRAW					; The end is also witin the visible zone so time to draw, evrything now is in...
							sec									; It is not so the length has to be adjusted (reduced)...
							lda #$28
							sbc DRAWTEXT_XPOSVAR
							sta DRAWTEXT_TEXTLENVAR				; The length of the text is reduced!
							jmp DRAWTEXTIN_DRAW					; Now everything is fine. Time to draw.
DRAWTEXTIN_XPOSNEG:			clc									; The beginning of the text if out of the visible zone (very left), but what about the end? is it in?
							adc DRAWTEXT_TEXTLENVAR
							bmi DRAWTEXTIN_RTS					; The end is also out of the visible zone so nothing else to do...
							beq DRAWTEXTIN_RTS					; The 0 is also not vivile...
							sta TEMP00_DATA						; The number of characters in the visible zone changes...It is temporary stored...
							sec
							lda DRAWTEXT_TEXTLENVAR
							sbc TEMP00_DATA						; Here accumulator keeps the number of characters off the visible zone...
							clc
							adc DRAWTEXT_TEXTLOWVAR				; ...that have to be added to the beginning of the text...
							sta DRAWTEXT_TEXTLOWVAR
							bcc DRAWTEXTIN_TEXTLENADJ			; Finally now it is time to calculate whether the end of the text is or not in the visible zone...
							inc DRAWTEXT_TEXTHIGHVAR			; The address of the data has jumped to another page...
DRAWTEXTIN_TEXTLENADJ:		lda TEMP00_DATA						; The temporal data is now the real length.
							sta DRAWTEXT_TEXTLENVAR
							lda #$00
							sta DRAWTEXT_XPOSVAR
							jmp DRAWTEXTIN_XPOSPOS				; Time to see whether the end is still in or has to be also adjusted
DRAWTEXTIN_DRAW:			jsr DRAWTEXT
DRAWTEXTIN_RTS:				pla
							sta DRAWTEXT_XPOSVAR
							pla
							sta DRAWTEXT_TEXTLENVAR
							pla
							sta DRAWTEXT_TEXTHIGHVAR
							pla
							sta DRAWTEXT_TEXTLOWVAR
							.RECOVERREGISTERS
							rts
; ------------------------------

; End.