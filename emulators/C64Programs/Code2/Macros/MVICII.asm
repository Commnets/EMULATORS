; C64 Assembler framework.
; Macros related with the VICII chip.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; ------------------------------
; Sprites
SPRITEENABLE				= $d015								; Sprites on - off.
SPRITEBASEXPOS				= $d000								; Sprite base coordinate X (+ 2*n sprite).
SPRITEBASEYPOS				= $d001								; Sprite base coordinate Y (+ 2*n sprite).
SPRITEMSBXPOS				= $d010								; The most significant bit of the X position for sprites.
SPRITEDWIDTH				= $d01d								; To set double width for sprites.
SPRITEDHEIGHT				= $d017								; To set double height for sprites.
SPRITEBASECOLOR				= $d027								; Sprite base color (+ n sprite).
SPRITEPRIORITY				= $d01b								; To manage the priority of the sprites over the foreground.
SPRITEMCOLOR				= $d01c								; To set the multicolor mode for sprites.
SPRITESCOLOR1				= $d025								; Sprite Shared Color 1 (once above is set).
SPRITESCOLOR2				= $d026								; Sprite Shared Color 2 (once above is set).
; Control of the screen
VICIIFOREGROUND				= $d020								; Foreground.
VICIIBACKGROUND				= $d021								; Background 0.
VICIIBACKGROUND1			= $d022								; Background 1, used in multicolor and extended background modes.
VICIIBACKGROUND2			= $d023								; Background 2, used also in those modes.
VICIIBACKGROUND3			= $d024								; Background 3, used also in those modes.
VICIISCROLLY				= $d011								; SCROLL in the Y axis, and also type of graphic mode.
VICIISCROLLX				= $d016								; SCROLL in the X axis, and also activation of multicolor possibilities.
VICIICTRLMEMREG				= $d018								; Control the location of the graphical memory.
; IRQ
VICIIIRQ					= $d019								; The active sources of the last IRQ (if any)
VICIICTRLIRQ				= $d01a								; To control the source of VICII IRQs.
VICIIRASTER					= $d012								; To manage the raster line (set and know)

; End.