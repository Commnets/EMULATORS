; C64 Assembler framework.
; Routine to create a rectangle in the screen.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

#../C64Programs/Code/RGENERAL.asm
#../C64Programs/Code/RGRAPHICS.asm

; ------------------------------
; DRAWRECTANGLE
; To draw a rectangle in the screen.
; The elements componing the different parts of the rectangle are received as parameter.
; DRAWRECT_XPOSVAR		:	The x position of the left up side corner of the rectangle.
; DRAWRECT_XLENVAR		:	X len of the rectangle. From 2 to 40 ($02 - $28). The result with other values is not guaranteed.
; DRAWRECT_YPOSVAR		:	The y position of the left up side corner of the rectangle.
; DRAWRECT_YLENVAR		:	Y len of the rectangle. From 2 to 25 ($02 - $19). The result with other values is not guaranteed.
; DRAWRECT_BYTEVAR		:	Set of 8 bytes with the ascii char per element in the rectangle (left up side corner, up line, right up,...).
; DRAWRECT_COLORVAR		:	The value of the color to fill the box with.
DRAWRECT_XPOSVAR			= MATRIXADDR_XPOSVAR
DRAWRECT_XLENVAR			= VARIABLES_DATAZONE + 5
DRAWRECT_YPOSVAR			= MATRIXADDR_YPOSVAR
DRAWRECT_YLENVAR			= VARIABLES_DATAZONE + 6
DRAWRECT_COLORVAR			= VARIABLES_DATAZONE + 7
DRAWRECT_BYTEVAR			= VARIABLES_DATAZONE + 8
; First calculates the memory position where the box starts
; Take into account that the value 0 is not allowed
DRAWRECTANGLE:				
; First of all, fills the matriz with color...
							lda #>COLORRAMBASE
							sta FILLMX_LOCSLOWVAR
							lda #<COLORRAMBASE
							sta FILLMX_LOCSHIGHVAR
							jsr FILLMATRIX
; Then starts to draw the rectangle.
							lda #>SCREENBASE
							sta MATRIXADDR_LOCLOWVAR
							lda #<SCREENBASE
							sta MATRIXADDR_LOCHIGHVAR
							jsr MATRIXADDRESS					; Outcome is in MATRIXADDR_LOCLOWRST and in MATRIXADDR_LOCHIGHRST.
; Then draws the shape of the rectangle.
							ldx DRAWRECT_YLENVAR
							dex
							ldy DRAWRECT_XLENVAR
							dey
; The top part of the rectangle.
							lda DRAWRECT_BYTEVAR + 2			; The right up corner character.
							sta (MATRIXADDR_LOCLOWRST),y
							lda DRAWRECT_BYTEVAR + 1			; The up character.
DRAWRECT_UPLINELOOP:		dey
							sta (MATRIXADDR_LOCLOWRST),y
							bne DRAWRECT_UPLINELOOP
							lda DRAWRECT_BYTEVAR + 0
							sta (MATRIXADDR_LOCLOWRST),y
; The sides of the rectangle.
DRAWRECT_LEFTRIGHTLOOP:		clc
							lda #$28
							adc MATRIXADDR_LOCLOWRST
							sta MATRIXADDR_LOCLOWRST
							bcc DRAWRECT_LEFTRIGHT
							inc MATRIXADDR_LOCHIGHRST
DRAWRECT_LEFTRIGHT:			lda DRAWRECT_BYTEVAR + 4			; The right character.
							ldy DRAWRECT_XLENVAR
							dey
							sta (MATRIXADDR_LOCLOWRST),y
							lda DRAWRECT_BYTEVAR + 3			; The left character.
							ldy #$00
							sta (MATRIXADDR_LOCLOWRST),y
							dex
							bne DRAWRECT_LEFTRIGHTLOOP
; The bottom part of the rectangle.
							lda DRAWRECT_BYTEVAR + 7			; The right down corner character.
							ldy DRAWRECT_XLENVAR
							dey
							sta (MATRIXADDR_LOCLOWRST),y
							lda DRAWRECT_BYTEVAR + 6			; The down character.
DRAWRECT_DOWNLINELOOP:		dey
							sta (MATRIXADDR_LOCLOWRST),y
							bne DRAWRECT_DOWNLINELOOP
							lda DRAWRECT_BYTEVAR + 5			; The left down corner character.
							sta (MATRIXADDR_LOCLOWRST),y
DRAWRECT_RTS:				rts
; ------------------------------

; End.