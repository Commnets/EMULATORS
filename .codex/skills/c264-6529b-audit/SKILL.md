---
name: c264-6529b-audit
description: Auditar el MOS 6529B Single Port Interface y su conexión en C16, C116 y Plus/4 en EMULATORS, incluyendo pull-ups pasivos, latch, lectura de pines, alias FD10/FD30, teclado con TED, joystick, CTS y cassette sense. Usar al revisar COMMODORE/C6529B, C264/C6529B1 o C6529B2 y C16SenseTapeRegisters.
---

# Auditoría C264 6529B

Leer [references/6529b.md](references/6529b.md) y [references/sources.md](references/sources.md). Distinguir el chip genérico, las dos funciones de placa y la numeración de clases C++: `C6529B1` es teclado y `C6529B2` user port en el repositorio; los designadores U de un esquema no se deducen de esos sufijos.

## Anclajes locales

- `include/COMMODORE/C6529B/{C6529B,C6529BRegisters}.hpp` y sus `.cpp` en `src/COMMODORE/C6529B/`.
- `include/C264/C6529B1*.hpp`, `C6529B2*.hpp` y sus `.cpp` en `src/C264/`.
- `C264/C16SenseTapeRegisters*`, `C264/Memory*`, `C264/OSIO*`, `C264/TEDRegisters*`, `C264/IO7510PortRegisters*`, `C264/DatasettePort*`.
- `$c264-ted-audit` para el strobe/captura `$FF08`; `$c264-acia-audit` para las señales del user port y CTS.

## Método

1. Elegir modelo y placa. C16/C116 llevan el 6529 de teclado; Plus/4 añade el de user port. La lectura cassette sense del C16 no implica un segundo 6529 instalado.
2. Separar dato escrito, conducción del pin, nivel externo y valor leído. No inventar DDR, interrupciones, timers o un protocolo SPI serie por las siglas Single Port Interface.
3. Revisar alias y resolución de entradas compartidas en el esquema, antes de juzgar la matriz de teclado o eventos del host.
4. Registrar cada hallazgo con fuente/página, ubicación C++, patrón de escritura/lectura, nivel esperado/observado y prueba. Declarar lo pendiente o no implementado sin convertirlo en conformidad.

Para cambios C++ usar `$emulators-framework-cpp-style`; para ejecutar `$emulators-windows-build-runtime`; para introspección `$emulators-fmt-audit`.
