; A text crossed the screen through a specific line of the screen...
; By Ignacio Cea

; MACROS
SCREEN			= $0400					; Where the screen starts
COLORRAM		= $D800					; Where the ram color starts

CHARCODES		= $C000					; Where the chars to cross the screen are defined
NUMCHARS		= $0C					; Number of chars defined (the number of letters of the full text)
COLORCHARS		= $03					; The color of the chars shown

FOREGROUND		= $D020					; The address in VICII for the foreground
BACKGROUND		= $D021					; The address in VICII for the background
INICOLOR		= $00					; The color for both

; The letters
*				= $C000
BYTES $08 $0f $0c $01 $20 $11 $15 $05 $20 $14 $01 $0C
; = Hola que tal

; Additional mamory space to manage the program
; Both are directions if the first page to speed up their manage...
CURRENTCOLUMN	= $03					; The column of the screen where the first letter of the text will be drawn next (it can be negative)
PARTIALCOLUMN	= $04					; The column of the text what it is supposed to be drawn!

; Where the code starts
*				= $C100

; Code
; To set the color of the back and foreground...
START:			LDA #INICOLOR
				STA FOREGROUND
				STA BACKGROUND
				
; Every line of the screen will go "virtually" from the position 
; "-length of the text" (a negative number) up to "40 = $28".
; So initially the text will be out of the screen, 
; it will appear little by little in it 
; and the it will leave the screen by the right also little by little...
				LDA #$00
				SEC
				SBC #NUMCHARS
LOOPCOLUMN:		STA CURRENTCOLUMN	; Initially it will be a negative number!
				STA PARTIALCOLUMN

				LDX #$00
LOOPCHARS:		TXA
				ADC PARTIALCOLUMN
				STA PARTIALCOLUMN
				BMI NEXTCHAR
				
DRAWCHAR:		TAY
				LDA CHARCODES,X
				STA SCREEN,Y
				LDA #COLORCHARS
				STA COLORRAM,Y
				
NEXTCHAR:		INX
				CPX #NUMCHARS
				BNE LOOPCHARS
				
				LDY #$00
DELAY:			NOP
				INY
				BNE DELAY
				
NEXTCOLUMN:		INC CURRENTCOLUMN
				LDA CURRENTCOLUMN
				CMP #$28
				BNE LOOPCOLUMN
				
				JMP START

; That's all
