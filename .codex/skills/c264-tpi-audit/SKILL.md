---
name: c264-tpi-audit
description: Auditar MOS/CSG 6523 TPI y su variante 6523A-28 en EMULATORS, denominada TIA en el código, incluyendo puertos, DDR, reset, bus paralelo 1551, selección de dispositivos y alias FEC0-FEFF. Usar al revisar COMMODORE/TIA o la conexión 1551 de C264; no es la TIA de Atari ni un chip instalado de base en C16/Plus4.
---

# Auditoría C264 TPI 6523 (TIA en el código)

Leer [references/tpi.md](references/tpi.md) y [references/sources.md](references/sources.md). Distinguir el 6523 genérico de 40 pines, el 6523A-28 de la interfaz externa 1551 y el 6525A de la placa de la unidad. No importar el control de interrupciones del 6525 al 6523.

## Anclajes locales

- `include/COMMODORE/TIA/TIA.hpp`, `TIARegisters.hpp` y sus `.cpp` en `src/COMMODORE/TIA/`.
- `include/C264/Memory.hpp`, `src/C264/Memory.cpp`: `_IOTIA9`, `_IOTIA8` y `_IOTIA8_SUBSET`.
- `C264/C264*`, `C264/ExpansionPort*`, `C264/IOPBuilder*` y dispositivos de disco realmente conectados. Una ventana de registros no demuestra que exista una unidad o que su protocolo esté implementado.

## Método

1. Identificar chip, encapsulado, esquema y presencia de la interfaz. El C16/Plus4 sin 1551 no debe adquirir un TPI por el nombre de una clase.
2. Separar los seis registros y resolución de pines del protocolo de handshaking implementado por software y cableado.
3. Reconstruir selección de dirección, unidad 8/9 y líneas efectivamente conectadas. No asumir que dos ventanas representan el mismo dispositivo porque el código use un espejo.
4. Para cada hallazgo indicar acceso, DDR, latch, nivel de pin, estado remoto y efecto visible. Añadir fuente/página, ubicación de código, esperado/observado y prueba reproducible.
5. Clasificar conformidad, divergencia, no implementado, no aplicable y pendiente. No exigir tiempos de una 1541 IEC a la interfaz paralela 1551.

Al editar C++ usar `$emulators-framework-cpp-style`; al ejecutar `$emulators-windows-build-runtime`; al cambiar introspección `$emulators-fmt-audit`. Para rutas 1541/IEC sí puede corresponder `$c64-1541-audit`, pero esa skill no especifica la 1551.
