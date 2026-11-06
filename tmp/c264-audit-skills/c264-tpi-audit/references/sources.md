# Fuentes TPI 6523 y 1551

Consultadas el 2026-11-06. Páginas PDF base 1.

- [6523-Commodore.pdf](6523-Commodore.pdf): original aportado, 7 páginas, Commodore Semiconductor Group MPS6523. PDF p. 1 (impresa 2-66): seis registros y estados ilegales; p. 2 arquitectura; pp. 4-5 timing; pp. 6-7 (2-71/2-72) lectura de pines, latch, DDR, CS y reset. No es una hoja específica del encapsulado 6523A-28.
- [Catálogo original CSG de 1981](https://www.bitsavers.org/pdf/commodore/Commodore_Component_Data_Catalog_1981.pdf): contraste de fabricante del mapa 6523, página impresa 2-48. Su numeración y edición difieren del original aportado. Confirma seis registros; no justifica asignar CR/IRQ a offsets 6/7.
- [1551-paddle-251925.png](1551-paddle-251925.png): esquema Commodore 251925 rev. 2 de la interfaz externa; [original descargado](https://www.zimmers.net/anonftp/pub/cbm/schematics/drives/new/1551/paddle-251925.png), inspeccionado visualmente. U2 es 6523A-28; PA0..7 datos, PB0/PB1 STATUS, PC6/PC7 ACK/DAV; RST del TPI dibujado a +5V; decodificador U1 8700-011. Confirmar lectura de una señal concreta en la imagen de resolución original antes de implementar su polaridad.
- [Archivo 1551](https://www.zimmers.net/anonftp/pub/cbm/schematics/drives/new/1551/index.html): distingue dibujo oficial 251860 rev. 4, paddle y reconstrucciones de Jerzy Sobola. Advierte que la reconstrucción de placa principal etiqueta 6523 donde el esquema Commodore indica 6525A. El índice fue consultado; no se ha realizado aquí una auditoría completa del controlador de disco.
- [Archivo CSG de chips](https://www.zimmers.net/anonftp/pub/cbm/documents/chipdata/index.html): distingue 6523 y 6525 con control de interrupciones. Localizador de ediciones, no fuente para inventar comportamiento de estados ilegales.
- [Fuentes de hardware común](../../c264-ted-audit/references/sources.md): manuales C264, C16 y Plus4 conservados una sola vez. Contrastan que el TPI del paddle es expansión, no el latch de teclado interno.

Las ventanas de dispositivo 8/9 deben completarse con tabla del decodificador/firmware o prueba de hardware al auditar esa ruta. Un modulo de memoria local no prueba ese contrato; el esquema no incluye las ecuaciones internas del 8700. No se declara resuelta la lectura de offsets ilegales ni el estado de pines ausentes del encapsulado reducido. Preservar esas incertidumbres en las pruebas.

## Identidad de las copias conservadas

| Archivo | Bytes | SHA-256 |
|---|---:|---|
| `6523-Commodore.pdf` | 199138 | `b504e4e6bf06a275aaaf708e98ba1419ce5e821bda15c616f74fb692de014da3` |
| `1551-paddle-251925.png` | 172769 | `3efac9678ae2606b72168e29a9ba20e0526a65d9829aa04cda5a83bf99e47a4c` |
