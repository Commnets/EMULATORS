; Routines to manage chars the screen
; By Ignacio Cea Forniés
; Copyright Community Networks 2022 - 2023

#../C64Programs/Code/RGENERAL.asm

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
VICIIFOREGROUND				= $D020							; VICII Address for the foreground
VICIIBACKGROUND				= $D021							; VICII Address for the background
VICIISCROLLX				= $D016							; VICII Address to SCROLL in the X axis
VICIISCROLLY				= $D011							; VICII Address to SCROLL in the Y axis

; ------------------------------
; MATRIXADDRESS
; To calculate the left side up corner address of a position in a matrix defined in the memory.
; The parameters used by this routine are:
; -> MATRIXADDR_XPOSVAR:	The x position of the left up side corner to calculate.
; -> MATRIXADDR_YPOSVAR:	The y position of the left up side corner to calculate.
; -> MATRIXADDR_MAXXVAR		The max number of columns of the matrix.
; -> MATRIXADDR_LOCLOWVAR:	The initial low byte of the matrix address.
; -> MATRIXADDR_LOCHIGHVAR:	The initial hight byte of the matrix address.
; The outcome of this routine are:
; -> MATRIXADDR_LOCLOWRST:	The low byte of the address of most up left corner address after calculation.
; -> MATRIXADDR_LOCHIGHRST:	The low byte of the address of most up left corner address after calculation.
MATRIXADDR_LOCLOWVAR		= VARIABLES_DATAZONE + 0
MATRIXADDR_LOCHIGHVAR		= VARIABLES_DATAZONE + 1
MATRIXADDR_XPOSVAR			= VARIABLES_DATAZONE + 2
MATRIXADDR_MAXXVAR			= VARIABLES_DATAZONE + 3
MATRIXADDR_YPOSVAR			= VARIABLES_DATAZONE + 4
MATRIXADDR_LOCLOWRST		= OUTCOME_DATAZONE + 0
MATRIXADDR_LOCHIGHRST		= OUTCOME_DATAZONE + 1
; Initially the result will be the same than the income...
MATRIXADDRESS:				lda MATRIXADDR_LOCLOWVAR
							sta MATRIXADDR_LOCLOWRST
							lda MATRIXADDR_LOCHIGHVAR
							sta MATRIXADDR_LOCHIGHRST
; Then moves the left up corner matrix address to the correct line
							ldx MATRIXADDR_YPOSVAR
MATRIXADDR_CALCY:			beq MATRIXADDR_X
							clc
							lda MATRIXADDR_MAXXVAR
							adc MATRIXADDR_LOCLOWRST
							sta MATRIXADDR_LOCLOWRST
							bcc MATRIXADDR_MOVENEXTY
							inc MATRIXADDR_LOCHIGHRST
MATRIXADDR_MOVENEXTY:		dex
							jmp MATRIXADDR_CALCY
; Now move the left up corner matrix address to the correct column
MATRIXADDR_X:				ldx MATRIXADDR_XPOSVAR
MATRIXADDR_CALCX:			beq MATRIXADDR_RTS
							clc
							lda #$01							; Step by step...
							adc MATRIXADDR_LOCLOWRST
							sta MATRIXADDR_LOCLOWRST
							bcc MATRIXADDR_MOVENEXTX
							inc MATRIXADDR_LOCHIGHRST
MATRIXADDR_MOVENEXTX:		dex
							jmp MATRIXADDR_CALCX
; Nothing else to do
MATRIXADDR_RTS:				rts

; ------------------------------
; FILLMATRIX
; To fill a box within a matrix with a value
; The parameters used by this routine are:
; -> FILLMX_XPOSVAR:		The x position of the left up side corner of the box.
; -> FILLMX_XLENVAR:		X len of the box. From 1 onwards.
; -> FILLMX_MAXXVAR			Max columns of the matrix
; -> FILLMX_YPOSVAR:		The y position of the left up side corner of the box.
; -> FILLMX_YLENVAR:		Y len of the box. From 1 onwards.
; -> FILLMX_BYTEVAR:		The value to fill the box with.
; -> FILLMX_LOCSLOWVAR:		The initial low byte of the most up left corner matrix address.
; -> FILLMX_LOCSHIGHVAR:	The initial hight byte of the most up left corner matrix address.
FILLMX_LOCSLOWVAR			= MATRIXADDR_LOCLOWVAR
FILLMX_LOCSHIGHVAR			= MATRIXADDR_LOCHIGHVAR
FILLMX_XPOSVAR				= MATRIXADDR_XPOSVAR
FILLMX_YPOSVAR				= MATRIXADDR_YPOSVAR
FILLMX_MAXXVAR				= MATRIXADDR_MAXXVAR
FILLMX_XLENVAR				= VARIABLES_DATAZONE + 5
FILLMX_YLENVAR				= VARIABLES_DATAZONE + 6
FILLMX_BYTEVAR				= VARIABLES_DATAZONE + 7
; First calculate the address where to start to fill up...
FILLMATRIX:					jsr MATRIXADDRESS					; Outcome is in MATRIXADDR_LOCLOWRST and in MATRIXADDR_LOCHIGHRST
; Now from that address fills up the matrix
; Take into account that the value 0 is not allowed
							ldx FILLMX_YLENVAR
							beq FILLMX_RTS
FILLMX_LINE:				ldy FILLMX_XLENVAR
							beq FILLMX_RTS
							lda FILLMX_BYTEVAR
FILLMX_LINELOOP:			dey
							sta (MATRIXADDR_LOCLOWRST),y
							bne FILLMX_LINELOOP
							clc
							lda FILLMX_MAXXVAR
							adc MATRIXADDR_LOCLOWRST
							sta MATRIXADDR_LOCLOWRST
							bcc FILLMX_MOVENEXTY
							inc MATRIXADDR_LOCHIGHRST
FILLMX_MOVENEXTY:			dex
							bne FILLMX_LINE
FILLMX_RTS:					rts

; ------------------------------
; FILLBOXSCREEN
; Fill a box of the screen with a character and with a color
; The parameters used by this routine are:
; -> FILLSCR_XPOSVAR:		The x position of the left up side corner of the screen box.
; -> FILLSCR_XLENVAR:		X len of the box. From 1 to 40 ($01 - $28). The result with other values is not guaranteed.
; -> FILLSCR_YPOSVAR:		The y position of the left up side corner of the screen box.
; -> FILLSCR_YLENVAR:		Y len of the box. From 1 to 25 ($01 - $19). The result with other values is not guaranteed.
; -> FILLSCR_BYTEVAR:		The value to fill the box with.
; -> FILLSCR_COLORVAR:		The value of the color to fill the box with.
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
							lda #$28								; Max number of text columns in the screen/matrix
							sta FILLMX_MAXXVAR
							jsr FILLMATRIX
; ...and then the color
							lda #>COLORRAMBASE
							sta FILLMX_LOCSLOWVAR
							lda #<COLORRAMBASE
							sta FILLMX_LOCSHIGHVAR
							lda FILLSCR_COLORVAR
							sta FILLMX_BYTEVAR
							jsr FILLMATRIX
							rts

; ------------------------------
; CLEARSCREEN
; To clear the screen with the color of the foreground
; No parameters are needed
CLEARSCREEN:				lda #$00
							sta FILLSCR_XPOSVAR
							sta FILLSCR_YPOSVAR
							lda #$28								; Max number of text columns in the screen
							sta FILLSCR_XLENVAR
							lda #$19								; Max number of rown in the screen
							sta FILLSCR_YLENVAR
							lda #$20								; It is the equivalent to a space
							sta FILLSCR_BYTEVAR
							lda VICIIBACKGROUND
							sta FILLSCR_COLORVAR
							jsr FILLBOXSCREEN
							rts

; ------------------------------
; DRAWRECTANGLE
; To draw a rectangle in the screen.
; The elements componing the different parts of the rectangle are received as parameter.
; -> DRAWRECT_XPOSVAR:		The x position of the left up side corner of the rectangle.
; -> DRAWRECT_XLENVAR:		X len of the rectangle. From 2 to 40 ($02 - $28). The result with other values is not guaranteed.
; -> DRAWRECT_YPOSVAR:		The y position of the left up side corner of the rectangle.
; -> DRAWRECT_YLENVAR:		Y len of the rectangle. From 2 to 25 ($02 - $19). The result with other values is not guaranteed.
; -> DRAWRECT_BYTEVAR:		Set of 8 bytes with the ascii char per element in the rectangle (left up side corner, up line, right up,...).
; -> DRAWRECT_COLORVAR:		The value of the color to fill the box with.
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
; Then starts to draw the rectangle
							lda #>SCREENBASE
							sta MATRIXADDR_LOCLOWVAR
							lda #<SCREENBASE
							sta MATRIXADDR_LOCHIGHVAR
							jsr MATRIXADDRESS					; Outcome is in MATRIXADDR_LOCLOWRST and in MATRIXADDR_LOCHIGHRST
; Then draws the shape of the rectangle
							ldx DRAWRECT_YLENVAR
							dex
							beq DRAWRECT_RTS
							ldy DRAWRECT_XLENVAR
							beq DRAWRECT_RTS
; The top part of the rectangle
							lda DRAWRECT_BYTEVAR + 2			; The right up corner character
							dey
							sta (MATRIXADDR_LOCLOWRST),y
							lda DRAWRECT_BYTEVAR + 1			; The up character
DRAWRECT_UPLINELOOP:		dey
							sta (MATRIXADDR_LOCLOWRST),y
							bne DRAWRECT_UPLINELOOP
							lda DRAWRECT_BYTEVAR + 0
							sta (MATRIXADDR_LOCLOWRST),y
; The sides of the rectangle
DRAWRECT_LEFTRIGHTLOOP:		clc
							lda #$28
							adc MATRIXADDR_LOCLOWRST
							sta MATRIXADDR_LOCLOWRST
							bcc DRAWRECT_LEFTRIGHT
							inc MATRIXADDR_LOCHIGHRST
DRAWRECT_LEFTRIGHT:			lda DRAWRECT_BYTEVAR + 4			; The right character
							ldy DRAWRECT_XLENVAR
							dey
							sta (MATRIXADDR_LOCLOWRST),y
							lda DRAWRECT_BYTEVAR + 3			; The left character
							ldy #$00
							sta (MATRIXADDR_LOCLOWRST),y
							dex
							bne DRAWRECT_LEFTRIGHTLOOP
; The bottom part of the rectangle
							lda DRAWRECT_BYTEVAR + 7			; The right down corner character
							ldy DRAWRECT_XLENVAR
							dey
							sta (MATRIXADDR_LOCLOWRST),y
							lda DRAWRECT_BYTEVAR + 6			; The down character
DRAWRECT_DOWNLINELOOP:		dey
							sta (MATRIXADDR_LOCLOWRST),y
							bne DRAWRECT_DOWNLINELOOP
							lda DRAWRECT_BYTEVAR + 5			; The left down corner character
							sta (MATRIXADDR_LOCLOWRST),y
DRAWRECT_RTS:				rts

; ------------------------------
; DRAWTEXT
; To draw a text in the screen
; -> DRAWTEXT_XPOSVAR:		The x position of the first letter of the text.
; -> DRAWTEXT_YPOSVAR:		The y position of the first letter of the text.
; -> DRAWTEXT_TEXTLENVAR:	Text length.
; -> DRAWTEXT_TEXTLOWVAR:	Low byte of the address where the text is kept.
; -> DRAWTEXT_TEXTHIGHVAR:	High byte of the address where the text is kept.
; -> DRAWTEXT_COLORVAR:		Color of the text.
DRAWTEXT_XPOSVAR			= MATRIXADDR_XPOSVAR
DRAWTEXT_YPOSVAR			= MATRIXADDR_YPOSVAR
DRAWTEXT_TEXTLENVAR			= VARIABLES_DATAZONE + 5
DRAWTEXT_TEXTLOWVAR			= VARIABLES_DATAZONE + 6
DRAWTEXT_TEXTHIGHVAR		= VARIABLES_DATAZONE + 7
DRAWTEXT_COLORVAR			= VARIABLES_DATAZONE + 8
; Draw text
DRAWTEXT:					lda #>SCREENBASE
							sta MATRIXADDR_LOCLOWVAR
							lda #<SCREENBASE
							sta MATRIXADDR_LOCHIGHVAR
							lda #$28
							sta MATRIXADDR_MAXXVAR
							jsr MATRIXADDRESS					; Outcome is in MATRIXADDR_LOCLOWRST and in MATRIXADDR_LOCHIGHRST
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
							rts

; End