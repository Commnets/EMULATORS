; Just to text scroll X
; By Ignacio Cea

; MACROS
SCREEN			= $0400					; Where the screen starts
SCROLLX			= $D016					; The address of the VIC II dedeicated to the scroll

; Where the code starts
* = $C000

; Code
START:			LDA SCROLLX
				AND #$F7
				STA SCROLLX				; To reduce the size of the screen in the X axis
				LDA #$01
				STA SCREEN
				BNE START				; Infinite

; That's all
