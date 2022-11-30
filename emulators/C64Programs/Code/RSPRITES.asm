; C64 Assembler framework.
; Macros related with the sprites system.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; ------------------------------
SPRITEENABLE				= $D015							; Sprites on - off.
SPRITEBASEXPOS				= $D000							; Sprite base coordinate X (+ 2*n sprite).
SPRITEBASEYPOS				= $D001							; Sprite base coordinate Y (+ 2*n sprite).
SPRITEMSBCPOS				= $D010							; The most significant bit of the X position for sprites.
SPRITEDWIDTH				= $D01D							; To set double width for sprites.
SPRITEDHEIGHT				= $D017							; To set double height for sprites.
SPRITEBASECOLOR				= $D027							; Sprite base color (+ n sprite).
SPRITEPRIORITY				= $D01B							; To manage the priority of the sprites over the foreground.
SPRITEMCOLOR				= $D01C							; To set the multicolor.
SPRITESCOLOR1				= $D025							; Sprite Shared Color 1.
SPRITESCOLOR2				= $D026							; Sprite Shared Color 2.

; End.