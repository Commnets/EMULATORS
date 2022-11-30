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
COLORRAMBASE				= $D800							; Where the ram color starts.
COLORRAM1_256BLK			= $D800							; The color RAM can be easly dividev in blocks of 256K.
COLORRAM2_256BLK			= $D900							; ...the next variables referes to those blocks.
COLORRAM3_256BLK			= $DA00
COLORRAM4_256BLK			= $DB00
VICIIFOREGROUND				= $D020							; VICII Address for the foreground.
VICIIBACKGROUND				= $D021							; VICII Address for the background.
VICIISCROLLX				= $D016							; VICII Address to SCROLL in the X axis.
VICIISCROLLY				= $D011							; VICII Address to SCROLL in the Y axis.

; End.