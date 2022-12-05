; C64 Assembler framework.
; Macros related with the sprites system.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; ------------------------------
SPRITEENABLE				= $d015							; Sprites on - off.
SPRITEBASEXPOS				= $d000							; Sprite base coordinate X (+ 2*n sprite).
SPRITEBASEYPOS				= $d001							; Sprite base coordinate Y (+ 2*n sprite).
SPRITEMSBCPOS				= $d010							; The most significant bit of the X position for sprites.
SPRITEDWIDTH				= $d01d							; To set double width for sprites.
SPRITEDHEIGHT				= $d017							; To set double height for sprites.
SPRITEBASECOLOR				= $d027							; Sprite base color (+ n sprite).
SPRITEPRIORITY				= $d01b							; To manage the priority of the sprites over the foreground.
SPRITEMCOLOR				= $d01c							; To set the multicolor.
SPRITESCOLOR1				= $d025							; Sprite Shared Color 1.
SPRITESCOLOR2				= $d026							; Sprite Shared Color 2.

; End.