# Fuentes TED y hardware C264

Consultadas el 2026-11-06. Se conservan los originales aportados sin alteraciones. Páginas PDF de visor, base 1; no confundir con páginas impresas ni con la numeración de hojas de ingeniería. El texto extraído por OCR se contrasta con la imagen al decidir un bit o una cifra.

## Documentos conservados

- [TED-System-Hardware-Manual.pdf](TED-System-Hardware-Manual.pdf): documento aportado como `(C264) TED System Hardware Manual.pdf`, 48 páginas. Arquitectura pp. 4-6; descripción TED pp. 11-15; temporización/DMA pp. 18-20; CPU pp. 21-28; user port pp. 33-39; vídeo/serial/expansión pp. 40-42; teclado pp. 47-48. [Copia pública equivalente de pagetable](https://www.pagetable.com/docs/ted/TED%20System%20Hardware%20Manual.pdf). [Especificación más completa y copia OCR de Zimmers](https://www.zimmers.net/anonftp/pub/cbm/schematics/computers/plus4/index.html): localizador adicional; no se han inspeccionado íntegramente todas sus ediciones.
- [TED-7360R0-Preliminary.pdf](TED-7360R0-Preliminary.pdf): original aportado, 26 páginas, no idéntico al PDF web de 23 páginas. Modos pp. 3-8; audio pp. 8-10; operación interna pp. 11-12; notas manuscritas pp. 13-14; registros pp. 15-20; señales pp. 21-24; añadido 7360R7 NTSC p. 25; p. 26 sin texto extraíble. [Edición web de 23 páginas](https://www.pagetable.com/docs/ted/TED%207360R0%20Preliminary%20Data%20Sheet.pdf), abril de 1983: usar rótulo de hoja/sección al comparar porque las páginas divergen. En la revisión se inspeccionaron visualmente las páginas locales 14-19, especialmente timers/IRQ/audio y bases de memoria.
- [Plus4-Service-1984.pdf](Plus4-Service-1984.pdf): Commodore, *Service Manual Model Plus/4 Computer*, octubre de 1984, PN-314001-04, 25 páginas, [descarga consultada](https://www.pagetable.com/docs/ted/Service%20Manual%20Model%20Plus%204%20Computer.pdf). Matriz/placa TED en hoja 310164 (1/4), PDF p. 11; ACIA, 6529 user port y PLA en hoja (3/4), PDF p. 15, impresa 9. Ambas hojas inspeccionadas visualmente. La tabla de componentes/alias de designadores debe leerse en la revisión concreta del dibujo.
- [C16-Service-1984.pdf](C16-Service-1984.pdf): Commodore, octubre de 1984, PN-314001-03, 22 páginas; [descarga consultada](https://www.zimmers.net/anonftp/pub/cbm/schematics/computers/plus4/C16_Service_Manual_314001-03_%281984_Oct%29.pdf). Esquema 251788 hojas 1-3 en PDF pp. 12,14,16; hoja 3 inspeccionada visualmente para decodificación y ausencia de chips del user port. No extrapolar los números de página a la edición preliminary del archivo.

Los otros originales se preservan en las skills hermanas: [6523](../../c264-tpi-audit/references/6523-Commodore.pdf), [6529](../../c264-6529b-audit/references/6529B.pdf), [6551](../../c264-acia-audit/references/6551_acia.pdf).

## Contraste independiente consultado

- [VICE ted-sound.c](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/plus4/ted-sound.c): fuente primaria de esa implementación, no especificación del silicio. Identifica explícitamente que su backend no es cycle based y documenta tabla de volumen derivada de YAPE. Útil para comparar saturación, mezcla, DC y generación digital; coincidencia entre emuladores no demuestra medición independiente.
- [VICE ted-timing.c](https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/src/plus4/ted-timing.c): separa estándar, ventana, DMA y coordenadas. Consultado para contraste arquitectónico; no se ha deducido una tabla temporal de constantes que este archivo no define.
- [Archivo de documentación técnica, Michael Steil](https://www.pagetable.com/?p=541): procedencia de escaneos y diferencias de edición. Es un índice editorial, no autoridad adicional independiente del mismo escaneo.
- [Archivo de esquemas originales C16/C116/Plus4](https://www.zimmers.net/anonftp/pub/cbm/schematics/computers/plus4/index.html): localizador por modelo/revisión. Una entrada del índice no prueba que se haya verificado todo el dibujo.

Los enlaces `main`/`master` son mutables. Una futura auditoría que use un emulador como comparador debe registrar commit/versión y configuración antes de guardar resultados como golden. No se afirma haber realizado una medición de hardware ni una auditoría del código con esta investigación.

## Reglas para resolver discrepancias

1. La edición preliminar describe una intención de diseño. La placa de producción decide conexión, presencia de chips y polaridad, pero un esquema no demuestra por sí solo la lógica interna de un chip.
2. No confundir valores programados por KERNAL con reset del chip. Distinguir descripción genérica, variante y configuración de máquina.
3. La fórmula de sonido, valores límite de divisor, fases DMA y fin de cuadro NTSC necesitan evidencia del silicio/revisión objetivo si las fuentes difieren. Mantener esperado provisional y experimento propuesto cuando falte resolución.
4. Enmascarar bits según registros concretos, no por semejanza con VIC-II, CIA o SID. Los errores de OCR pueden invertir alto/bajo o cambiar 0 por 1.
5. Cada hallazgo debe citar documento, página/sección, modelo/revisión, observación y grado de certeza. Una decisión de emulador es contraste, no un tercer manual de fabricante.

## Identidad de las copias conservadas

| Archivo | Bytes | SHA-256 |
|---|---:|---|
| `TED-System-Hardware-Manual.pdf` | 7611971 | `ae073c65599e982b3f7884907b2dfb93a8c75eb01c6daeffb98bf65a38efc855` |
| `TED-7360R0-Preliminary.pdf` | 5905779 | `6ff61272cdd926ba6d9b829d6d6987e5137d096b9251862e9b43c74675f7ce9a` |
| `Plus4-Service-1984.pdf` | 4852956 | `6bafc23c02d9798ccefb3297d337b86e1fef3b33e0652d11cc4ddf28cd4b15f7` |
| `C16-Service-1984.pdf` | 1909733 | `0d1a6f1d26d933d17013652faf9c64f2e7eb333f01f098b93b4c0ddb2e0e172b` |
