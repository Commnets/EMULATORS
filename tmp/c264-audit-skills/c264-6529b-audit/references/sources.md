# Fuentes 6529B y conexión C264

Consultadas el 2026-11-06. Páginas PDF base 1.

- [6529B.pdf](6529B.pdf): original aportado, Commodore Semiconductor Group 6529 Single Port Interface, abril de 1981, 4 páginas. P. 1: pull-ups pasivos, ausencia de modo DDR, tabla CS/RW, reset de alimentación normal y advertencia de alimentación lenta; pp. 3-4: timing de lectura/escritura. La tabla de pedido distingue velocidades 1/2/3 MHz. Conservar el título real 6529 aunque el archivo se llame 6529B.
- [Archivo CSG de originales](https://www.zimmers.net/anonftp/pub/cbm/documents/chipdata/index.html): localiza `6529.zip`, fechado 4/81, y un esquema de sustitución. Se consultó el índice; el circuito de sustitución no se usa aquí como especificación interna del chip.
- [MAME mos6529.cpp](https://raw.githubusercontent.com/mamedev/mame/master/src/devices/machine/mos6529.cpp): contraste independiente de implementación, `read()` combina `m_input & m_output` y el latch inicial de salida es $FF. Es evidencia de esa implementación, no una medición de power-on ni de subida analógica. Enlace master mutable: fijar commit si se usa en un test diferencial futuro.
- [Fuentes de hardware común y PDF conservados](../../c264-ted-audit/references/sources.md): hardware C264 capítulo 7 pp. 33-35 identifica siete bits de user port y P2 compartido con cassette; pp. 47-48 matriz de teclado. Plus4-Service-1984.pdf hoja 310164 (1/4), PDF p. 11, muestra teclado/joystick/TED; hoja (3/4), PDF p. 15, muestra 6529B user port, P1 a CTS del conector y P2 a CST SENSE. Ambas hojas se inspeccionaron visualmente. C16-Service-1984.pdf hojas 251788 (2/3) y (3/3), PDF pp. 14 y 16, sirven para separar teclado de la lógica cassette sin segundo 6529; la hoja 3 se inspeccionó visualmente.

No confundir número de clase B1/B2, designador U de placa y sufijo de velocidad B. Confirmar la revisión de placa antes de copiar un designador. La resolución digital AND exige conductores compatibles y pull-ups; no es un modelo analógico universal de contención. El reset normal del programa/KERNAL no añade un pin /RESET al dispositivo físico.

## Identidad de las copias conservadas

| Archivo | Bytes | SHA-256 |
|---|---:|---|
| `6529B.pdf` | 1758437 | `4f639e562e05153ed4c2220b2c6f253ec79ff05c4103b48e6df60d3c0b6ec1c9` |
