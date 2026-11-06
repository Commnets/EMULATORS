---
name: c264-ted-audit
description: Auditar la fidelidad del TED 7360/8360 en EMULATORS para C16, C116 y Plus/4, incluyendo registros FF00-FF3F, vídeo, sonido, temporizadores, IRQ, DMA, reloj CPU, memoria visible y latch de teclado. Usar al revisar o modificar TED, TEDRegisters, TEDTimer, TEDSoundWrapper o su integración C264.
---

# Auditoría C264 TED

Leer [references/ted.md](references/ted.md) y [references/sources.md](references/sources.md) antes de fijar comportamientos esperados. La referencia separa vídeo, sonido, temporizadores y conexión al sistema; cargar las páginas de los PDF que correspondan al trabajo concreto.

## Aplicación al repositorio

- Núcleo: `include/COMMODORE/TED/{TED,TEDRegisters,TEDTimer,TEDSoundWrapper}.hpp` y sus `.cpp` en `src/COMMODORE/TED/`.
- Conexión: `include/C264/{TED,TEDRegisters,Memory,Sound,Screen,C264}.hpp` y sus `.cpp` en `src/C264/`.
- Bancos: `C264/ROMRAMSwitch*`, `C264/C1C2Selector*`; entrada: `C264/C6529B1*`, `C264/OSIO*` y `C264/IO7510PortRegisters*`.
- Consultar `$c264-6529b-audit` para selección de filas; no convertir TED en una CIA ni atribuirle el puerto de E/S del 7501.

## Método

1. Identificar modelo, RAM, ROM, PAL/NTSC, revisión del TED y nivel de fidelidad buscado. Separar encendido, reset, inicialización KERNAL y snapshot.
2. Registrar por acceso dirección, máscara, valor antes/después, efecto lateral, reloj/fase y señal externa. Distinguir lectura CPU de inspección sin efectos del depurador.
3. Para vídeo, correlacionar escritura, fetch, latch y píxel de salida. Para sonido, correlacionar escritura, divisor, fase, mezcla y muestra. Un resultado visual o audible parecido no prueba temporización correcta.
4. Distinguir especificación preliminar, esquema de producción, medición y decisión de otro emulador. Conservar las discrepancias descritas en las fuentes; no resolverlas haciendo coincidir la guía con el código local.
5. Entregar hallazgos con fuente/página, ubicación C++, secuencia reproducible, esperado/observado, impacto y prueba propuesta. Clasificar cada punto como conforme, divergente, no implementado, fuera de alcance o pendiente de evidencia. No declarar una auditoría completa a partir de un arranque BASIC.

Usar `$emulators-framework-cpp-style` al editar C++; `$emulators-windows-build-runtime` al compilar o ejecutar; `$emulators-fmt-audit` si cambian productores o formatos de `InfoStructure`. Esta skill no exige modificar código al preparar una revisión.
