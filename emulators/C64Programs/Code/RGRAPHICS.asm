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
VICIIBACKGROUND				= $D020							; VICII Address for the background
VICIIFOREGROUND				= $D021							; VICII Address for the foreground
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
; The outcome of thie routine are:
; -> MATRIXADDR_LOCLOWRST:	The low byte of the address of most up left corner address after calculation.
; -> MATRIXADDR_LOCHIGHRST:	The low byte of the address of most up left corner address after calculation.
MATRIXADDR_XPOSVAR			= VARIABLES_DATAZONE + 0
MATRIXADDR_YPOSVAR			= VARIABLES_DATAZONE + 1
MATRIXADDR_MAXXVAR			= VARIABLES_DATAZONE + 2
MATRIXADDR_LOCLOWVAR		= VARIABLES_DATAZONE + 3
MATRIXADDR_LOCHIGHVAR		= VARIABLES_DATAZONE + 4
MATRIXADDR_LOCLOWRST		= OUTCOME_DATAZONE + 0
MATRIXADDR_LOCHIGHRST		= OUTCOME_DATAZONE + 1
; Initially the result will be the same than the income...
MATRIXADDRES:				lda MATRIXADDR_LOCLOWVAR
							sta MATRIXADDR_LOCLOWRST
							lda MATRIXADDR_LOCHIGHVAR
							sta MATRIXADDR_LOCHIGHRST
; Then moves the left up corner matrix address to the correct line
							ldx MATRIXADDR_YPOSVAR
MATRIXADDR_CALCY:			beq MATRIXADDR_X
							lda MATRIXADDR_MAXXVAR
							adc MATRIXADDR_LOCLOWRST
							sta MATRIXADDR_LOCLOWRST
							bvc MATRIXADDR_MOVENEXTY
							inc MATRIXADDR_LOCHIGHRST
MATRIXADDR_MOVENEXTY:		dex
							jmp MATRIXADDR_CALCY
; Now move the left up corner matrix address to the correct column
MATRIXADDR_X:				ldx MATRIXADDR_XPOSVAR
MATRIXADDR_CALCX:			beq MATRIXADDR_RTS
							lda #$01							; Step by step...
							adc MATRIXADDR_LOCLOWRST
							sta MATRIXADDR_LOCLOWRST
							bvc MATRIXADDR_MOVENEXTX
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
; -> FILLMX_XLENVAR:		X len of the box.
; -> FILLMX_MAXXVAR			Max columns of the matrix
; -> FILLMX_YPOSVAR:		The y position of the left up side corner of the box.
; -> FILLMX_YLENVAR:		Y len of the box.
; -> FILLMX_BYTEVAR:		The value to fill the box with.
; -> FILLMX_LOCSLOWVAR:		The initial low byte of the most up left corner matrix address.
; -> FILLMX_LOCSHIGHVAR:	The initial hight byte of the most up left corner matrix address.
FILLMX_XPOSVAR				= VARIABLES_DATAZONE + 5
FILLMX_XLENVAR				= VARIABLES_DATAZONE + 6
FILLMX_MAXXVAR				= VARIABLES_DATAZONE + 7
FILLMX_YPOSVAR				= VARIABLES_DATAZONE + 8
FILLMX_YLENVAR				= VARIABLES_DATAZONE + 9
FILLMX_BYTEVAR				= VARIABLES_DATAZONE + 10
FILLMX_LOCSLOWVAR			= VARIABLES_DATAZONE + 11
FILLMX_LOCSHIGHVAR			= VARIABLES_DATAZONE + 12
; Firs calculate the address where to start to fill up...
FILLMATRIX:					lda FILLMX_LOCSLOWVAR
							sta MATRIXADDR_LOCLOWVAR
							lda FILLMX_LOCSHIGHVAR
							sta MATRIXADDR_LOCHIGHVAR
							lda FILLMX_XPOSVAR
							sta MATRIXADDR_XPOSVAR
							lda FILLMX_YPOSVAR
							sta MATRIXADDR_YPOSVAR
							lda FILLMX_MAXXVAR
							sta MATRIXADDR_MAXXVAR
							jsr MATRIXADDRES					; Outcome is in MATRIXADDR_LOCLOWRST and in MATRIXADDR_LOCHIGHRST
; Now from that address fills up the matrix...
							ldx FILLMX_YPOSVAR
							beq FILLMX_RTS
FILLMX_LINE:				ldy FILLMX_XLENVAR
							beq FILLMX_RTS
							lda FILLMX_BYTEVAR
FILLMX_LINELOOP:			dey
							sta (MATRIXADDR_LOCLOWRST),y
							bne FILLMX_LINELOOP
							lda FILLMX_MAXXVAR
							adc MATRIXADDR_LOCLOWRST
							sta MATRIXADDR_LOCLOWRST
							bvc FILLMX_MOVENEXTY
							inc MATRIXADDR_LOCHIGHRST
FILLMX_MOVENEXTY:			dex
							bne FILLMX_LINE
FILLMX_RTS:					rts

; ------------------------------
; FILLBOXSCREEN
; Fill a box of the screen with a character and with a color
; The parameters used by this routine are:
; -> FILLSCR_XPOSVAR:		The x position of the left up side corner of the screen box.
; -> FILLSCR_XLENVAR:		X len of the box.
; -> FILLSCR_MAXXVAR		Max columns of the matrix
; -> FILLSCR_YPOSVAR:		The y position of the left up side corner of the screen box.
; -> FILLSCR_YLENVAR:		Y len of the box.
; -> FILLSCR_BYTEVAR:		The value to fill the box with.
; -> FILLSCR_COLORVAR:		The value of the color to fill the box with.
FILLSCR_XPOSVAR				= VARIABLES_DATAZONE + 5
FILLSCR_XLENVAR				= VARIABLES_DATAZONE + 6
FILLSCR_YPOSVAR				= VARIABLES_DATAZONE + 8
FILLSCR_YLENVAR				= VARIABLES_DATAZONE + 9
FILLSCR_BYTEVAR				= VARIABLES_DATAZONE + 10
FILLSCR_COLORVAR			= VARIABLES_DATAZONE + 13
; First draws the character...
FILLBOXSCREEN:				lda #>SCREEBASE
							sta FILLMX_LOCSLOWVAR
							lda #<SCREEBASE
							sta FILLMX_LOCSHIGHVAR
							lda #$28								; Max number of text columns in the screen
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
							lda #$20								; It is the equivalent to an space
							sta FILLSCR_BYTEVAR
							lda VICIIBACKGROUND
							sta FILLSCR_COLORVAR
							jsr FILLBOXSCREEN
							rts

; End