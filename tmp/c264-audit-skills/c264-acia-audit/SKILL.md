---
name: c264-acia-audit
description: Auditar ACIA MOS/Synertek 6551/6551A y su conexión en Plus/4 en EMULATORS, incluyendo FD00-FD0F, UART, datos, estado, command/control, reset, baudrate, IRQ, módem y user port. Usar al revisar COMMODORE/ACIA o comunicaciones C264; distinguir ausencia en C16/C116 y variantes CMOS posteriores.
---

# Auditoría C264 ACIA 6551

Leer [references/acia.md](references/acia.md) y [references/sources.md](references/sources.md). Las tablas preliminares no resuelven por sí solas todas las condiciones de IRQ; identificar variante y evidencia antes de fijar una prueba temporal.

## Anclajes locales

- `include/COMMODORE/ACIA/{ACIA,ACIARegisters}.hpp` y sus `.cpp` en `src/COMMODORE/ACIA/`.
- `include/C264/{C264,Memory}.hpp`, `src/C264/{C264,Memory}.cpp`: `CommodorePlus4::standardChips`, `_IOACIA`, `_IONOMAPPED0_SUBSET`.
- `C264/C6529B2*`, `C264/IO7510PortRegisters*`, `C264/ExpansionPort*` y backend de comunicaciones que exista realmente. Ver `$c264-6529b-audit` para CTS y cassette sense.

## Método

1. Identificar 6551, 6551A o variante concreta, cristal, placa, ROM y periférico. El C16/C116 estándar no lleva ACIA; no evaluar su ventana como si fuera Plus/4.
2. Separar contrato de cuatro registros, estado UART, líneas de módem, conexión a CPU y transporte host. Una API de bytes no demuestra temporización de bits.
3. Correlacionar por timestamp bus CPU, transferencias entre buffers/shift registers, bits serie, estado e IRQ; distinguir lectura CPU de peek del depurador.
4. Para cada hallazgo aportar fuente/página, acceso o secuencia, esperado/observado, línea C++, impacto y prueba. Marcar por separado implementación ausente, divergencia y comportamiento aún no resuelto.

Usar `$emulators-framework-cpp-style` al editar C++, `$emulators-windows-build-runtime` al compilar/ejecutar y `$emulators-fmt-audit` al cambiar introspección. No introducir erratas W65C51N como requisitos de un 6551 NMOS del Plus/4.
