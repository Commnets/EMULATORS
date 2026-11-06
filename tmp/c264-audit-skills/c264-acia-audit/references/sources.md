# Fuentes ACIA 6551 y Plus/4

Consultadas el 2026-11-06. Páginas PDF base 1.

- [6551_acia.pdf](6551_acia.pdf): original aportado, CSG/MOS Technology 6551, preliminary, 12/80, 8 páginas. No contiene capa de texto; se renderizaron e inspeccionaron las ocho páginas. Bloques p. 2; bus pp. 4-5; cuatro offsets, pines y reloj p. 6; figuras 6/7 Control/Command y reset p. 7; figura 8 Status y datos p. 8.
- [Archivo CSG de hojas originales](https://www.zimmers.net/anonftp/pub/cbm/documents/chipdata/index.html): localiza `6551.zip` de diciembre de 1980. Es el mismo tipo de documento primario, no una medición adicional.
- [Notas originales Synertek SY6551 conservadas en un anexo universitario](https://pure.tue.nl/ws/portalfiles/portal/46750903/333497-1.pdf): tesis de F.G.M.J. Wuts, 1982, TU Eindhoven, 145 páginas; usar exclusivamente el material de fabricante SY6551 del anexo como contraste técnico. PDF p. 125 (rótulo 124): figuras de transmisión/recepción continua; p. 132 (rótulo 131): reconocimiento de Status y DCD/DSR; p. 133 (rótulo 132): Programmed Reset y notas; p. 134 (rótulo 133): divisores. Se consultaron texto y localizadores de esas secciones. No se redistribuye la tesis dentro de la skill. La aplicabilidad exacta de una nota Synertek a otra revisión MOS necesita confirmación.
- [MAME mos6551.cpp](https://raw.githubusercontent.com/mamedev/mame/master/src/devices/machine/mos6551.cpp): contraste de implementación de mapas, buffers y reset; `device_reset()` usa Command=0, frente al $02 de la figura 7 preliminar aportada. No prueba cuál reproduce un lote concreto de silicio. Registrar versión/commit en futuras comparaciones; el enlace master es mutable.
- [Manuales y esquemas de producción conservados](../../c264-ted-audit/references/sources.md): Plus4-Service-1984.pdf p. 15, impresa 9, esquema 310164 hoja 3, inspeccionado visualmente: /CTS interno a GND, CTS externo por 6529 P1, inversores de DTR/RTS/DCD/DSR y cristal Y2 1,8432 MHz. El manual de servicio identifica también un Synertek SYP6551A como opción de componente. El manual C16 establece la máquina sin ACIA de base.

No usar una hoja moderna W65C51N ni sus erratas como oráculo automático. Mantener desacuerdos de reset, casos IRQ y detalle de divisores identificados por variante. Ninguna de estas fuentes demuestra que la ACIA local esté implementada o sea conforme; esa es una revisión posterior.

## Identidad de las copias conservadas

| Archivo | Bytes | SHA-256 |
|---|---:|---|
| `6551_acia.pdf` | 1075853 | `bf7e94a3224cf0046fd63fe234309eb10a7b1405d0a949cad123fc2084fb6d7c` |
