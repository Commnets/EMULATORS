; C64 Assembler framework.
; VIC-II bad-line display-state acceptance-window regression.
; A Bad Line Condition present before cycle 12 must remain observable without
; switching the graphics sequencer from idle to display state.
; The lower screen must remain stable while $d011 changes from $14 to $1b.
; By Ignacio Cea Fornies.

VICIISCROLLY				= $d011
VICIIRASTER				= $d012
VICIIFOREGROUND			= $d020
VICIIBACKGROUND			= $d021
SCREENBASE				= $0400
COLORRAMBASE				= $d800
SCREENPOINTERLOW			= $fb
SCREENPOINTERHIGH		= $fc

* = $cb00
MAIN:						sei
							lda #$00
							sta VICIIBACKGROUND
							lda #$01
							sta VICIIFOREGROUND

; Fill every screen row with a different character. A one-row VCBASE error is
; therefore visible immediately as an eight-pixel displacement.
							lda #<SCREENBASE
							sta SCREENPOINTERLOW
							lda #>SCREENBASE
							sta SCREENPOINTERHIGH
							lda #$01
							ldx #$19
FILLROW:					ldy #$00
FILLROWCOLUMN:			sta (SCREENPOINTERLOW),y
							iny
							cpy #$28
							bne FILLROWCOLUMN
							pha
							clc
							lda SCREENPOINTERLOW
							adc #$28
							sta SCREENPOINTERLOW
							bcc FILLROWNOCARRY
							inc SCREENPOINTERHIGH
FILLROWNOCARRY:			pla
							clc
							adc #$01
							dex
							bne FILLROW

; Use a uniform visible color for the complete 1 KB Color RAM window.
							ldx #$00
							lda #$01
FILLCOLOR:				sta COLORRAMBASE,x
							sta COLORRAMBASE + $100,x
							sta COLORRAMBASE + $200,x
							sta COLORRAMBASE + $300,x
							inx
							bne FILLCOLOR

; At line 256 select YSCROLL=4. On the next frame, line 212 initially matches
; that value because 212 & 7 == 4. The short line-212 path then changes to
; YSCROLL=3 before the bad-line display-state acceptance window.
TESTFRAME:				bit VICIISCROLLY
							bpl TESTFRAME
WAITLINE256:				bit VICIISCROLLY
							bpl WAITLINE256
							lda VICIIRASTER
							bne WAITLINE256
							lda #$14
							sta VICIISCROLLY
							lda #$06
							sta VICIIFOREGROUND

WAITNEXTFRAME:			bit VICIISCROLLY
							bmi WAITNEXTFRAME
WAITLINE212:				lda VICIIRASTER
							cmp #$d4
							bne WAITLINE212
							lda #$1b
							sta VICIISCROLLY
							lda #$05
							sta VICIIFOREGROUND
							jmp TESTFRAME

; Expected deep-debug invariants on raster line 212:
; - before cycle 12: BadlineCondition=1, BadlineDetected=0 and IDLE=1.
; - no BA request or c-access sequence is generated for the line.
; - at cycle 59, VC and VCBASE keep the same value.
; A normal bad line must still be accepted on raster line 219 after YSCROLL=3.

; End.
