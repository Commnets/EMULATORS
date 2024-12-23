; To make lines of color just in the border
; Print like a hello in a VIC 20 8k+ expanded.
; The program loadas in the expanded memory and the screen and screen color memory
; are located where it is defined for a VIC20 8k+ expanded.
; It would only work there!
; By Ignacio Cea

; MACROS
BKCOLOR		= $02						; From 0 to f
BRDCOLOR	= $00						; From 0 to 7

; Where the code starts
* = $0400

; Code
; Preparing the system

; VERY CAREFUL!
; Set the location of the screen color memory
				lda $9002
				and #$7f
				ora #00					; with 0 at 37888 = ($9400) and with 1 at 38400 ($9600). The location of the scree matrix also changes
				sta $9002
; Set the location of the screen matrix
				lda $9005
				and #$0f
				ora #$c0
				sta $9005				; see documentation VIC1
; VERY CAREFUL!

; Set the background and the border color as defined...
				lda #BKCOLOR
				asl
				asl
				asl
				asl
				ora #BRDCOLOR
				sta $900f
; All characters in the screen in the background color...
				lda #BKCOLOR
				ldx #$00
LOOPCLEAR:		sta $9400,x
				sta $9500,x
				inx
				bne LOOPCLEAR

; Adjusting the origin of the screen...
				lda $9002
				and #$80
				ora #$16				; The number of columns, in the bits 0 - 6
				sta $9002
				lda $9003
				and #$81
				ora #$2e				; The number of rows, in the bits 1 - 6
				sta $9003

; Adjusting the offset of the screen...
				lda #$0e				; Offset x of the screen
				sta $9000
				lda #$27				; Offset y of the screen
				sta $9001

; Adjusting the size of the characters...
				lda $9003
				and #$fe
				ora #$00				; Height of the characters?
				sta $9003

; Adjusting the auxiliary color...
				lda $900e
				and #$0f
				ora #$40				; Auxiliary color (using in multicolor graphics)
				sta $900e

; Adjusting inverse mode...
				lda $900f
				and #$f7
				ora #$08				; Inverse mode active (8 = no, 0 = yes)
				sta $900f
				
; Main part
; My first letters...
				lda #$01				; First letter
				sta $1000
				sta $1001
				sta $1002
				lda #$02				; Second letter
				sta $1003
				sta $1004
				sta $1005
				lda #$03				; (0 - 7), When bit 3 on, multicolor mode is active...
				sta $9400
				sta $9401
				sta $9402
				lda #$05				; (0 - 7), When bit 3 on, multicolor mode is active...
				sta $9403
				sta $9404
				sta $9405

; To iterate the colors in the screen...
				ldx #BRDCOLOR
LOOP:			lda #BKCOLOR
				asl
				asl
				asl
				asl
				sta $00
				txa
				and #$0f
				ora #$00				; Inverse mode?
				ora $00
				sta $900f
				inx
				cpx #$08				; Border color always from 0 to 7
				bne SLOW
				ldx #$00
				ldy #$ff
SLOW:			dey 
				bne SLOW
				jmp LOOP
FOREVER:		jmp FOREVER				; Infinite

; That's all
