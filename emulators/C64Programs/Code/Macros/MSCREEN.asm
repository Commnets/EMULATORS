; C64 Assembler framework.
; Macros related with the graphic system.
; By Ignacio Cea Forniés.
; Copyright Community Networks 2022 - 2023.

; ------------------------------
SCREENBASE					= $0400							; Where usually the screen starts.
SCREEN1_256BLK				= $0400							; The screen can be easly divided in blocks of 256K...
SCREEN2_256BLK				= $0500							; ...the next variables referes to those blocks.
SCREEN3_256BLK				= $0600
SCREEN4_256BLK				= $0700
COLORRAMBASE				= $d800							; Where the ram color starts.
COLORRAM1_256BLK			= $d800							; The color RAM can be easly dividev in blocks of 256K.
COLORRAM2_256BLK			= $d900							; ...the next variables referes to those blocks.
COLORRAM3_256BLK			= $da00
COLORRAM4_256BLK			= $db00
ROMCHARSET					= $d000							; Where the CPU sees the default char set
ROMCTRLAVAILABLE			= $01							; To control whether roms are or not visible from CPU

; End.