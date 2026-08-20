# Workbook del procesador FSM83::SM83

## Identidad

- **Núcleo:** Sharp SM83 compatible con el integrado en `DMG-CPU B`.
- **Alias histórico:** LR35902; no se usará para inferir compatibilidad eléctrica o de encapsulado.
- **Decisión:** implementación nueva en namespace `FSM83`.
- **Referencia local:** patrones de CORE/FZ80, no herencia semántica.
- **Fidelidad:** resultados, flags, accesos de bus y ciclos observables.

## Arquitectura

- Bus de datos de 8 bits.
- Espacio de direcciones de 16 bits.
- Valores de 16 bits little-endian en memoria.
- Registros de 8 bits: A, F, B, C, D, E, H, L.
- Pares: AF, BC, DE, HL.
- Registros de 16 bits: SP y PC.
- Flags en F: Z, N, H, C; bits inferiores no son flags de propósito general.
- Stack descendente en el espacio de memoria, con accesos de byte ordenados que deben probarse.
- No hay espacio separado de puertos IN/OUT.

## Estados de ejecución

- Running/fetch/execute.
- HALT.
- STOP.
- Entrada de interrupción.
- CPU bloqueada o con bus restringido por OAM DMA sin congelar el reloj de los periféricos.
- Reset/power-on.

HALT, el bug de HALT, EI diferido, DI, RETI y wake-up con IME desactivado tendrán transiciones explícitas, no booleanos independientes sin máquina de estados.

## Interrupciones

- Cinco fuentes con vectores fijos.
- IF/IE pertenecen al SoC; IME pertenece a la CPU.
- Prioridad ascendente por bit 0-4.
- El muestreo se realiza en fronteras de instrucción válidas.
- La entrada debe documentar orden de desactivación de IME, limpieza de IF, pushes de PC y carga de vector.
- Cada M-cycle de entrada tendrá una traza de bus independiente.

## Espacios de opcode

| Espacio | Entradas | Estado Hito 0 |
|---|---:|---|
| Base | 256 | Estructura definida; clasificación completa pendiente de Hito 1 |
| Prefijo CB | 256 | Estructura definida; clasificación completa pendiente de Hito 1 |

Cada fila del ledger de Hito 1 contendrá:

- bytes y opcode;
- mnemónico canónico y alias;
- operandos y longitud;
- semántica;
- flags por caso;
- accesos de bus ordenados por M/T-cycle;
- duración base y condicional;
- frontera de interrupción;
- clasificación legal/ilegal/alias/model-specific;
- identificadores de tests.

## Familias previstas

- Loads de 8 y 16 bits.
- Loads `LDH` y direccionamiento `0xFF00+C/a8`.
- Aritmética y lógica de 8 bits.
- Operaciones de 16 bits y `ADD SP,r8` / `LD HL,SP+r8`.
- INC/DEC.
- Rotaciones y shifts base/CB.
- BIT/RES/SET.
- Jumps, relative jumps, calls, returns y restarts.
- Stack.
- DAA, CPL, SCF y CCF.
- HALT, STOP, DI y EI.
- Opcodes ilegales explícitos.

## Particiones de prueba

- Valores 00, 01, 7F, 80, FE, FF y patrones AA/55.
- Carry/half-carry de suma y resta en ambos límites de nibble.
- Z/N/H/C preservado, fijado, limpiado o dependiente.
- Direcciones 0000, 00FF, FF00, FFFF y wrap.
- Stack en límites y orden de bytes.
- Branch taken/not taken.
- Condiciones NZ, Z, NC y C.
- Operandos `(HL)`, inmediatos y high-memory.
- Interrupción antes, durante y después de cada frontera muestreable.
- HALT con cada combinación IME y `IE & IF`.
- EI seguido de instrucciones normales, HALT, DI y petición pendiente.
- Opcode ilegal y stream truncado.
- Código automodificable y escritura sobre siguiente fetch en RAM.

## Decisiones de integración con CORE

Antes de escribir instrucciones se demostrará que:

1. `CPUTransaction` conserva orden y tipo de cada acceso.
2. Una instrucción puede progresar por ciclos sin aplicar todos sus side effects al inicio.
3. PPU/timer/APU continúan mientras SM83 está en HALT.
4. DMA puede negar ciertos accesos sin detener todos los ciclos CPU.
5. La interrupción puede sustituir el siguiente fetch en la frontera correcta.

Si CORE no lo permite, se diseñará una extensión genérica mínima. No se añadirá una condición `GameBoy` a CORE.

## Puerta de entrada al código del Hito 1

- Fuente y convención de mnemónicos elegidas.
- 512 encodings clasificados.
- Opcodes ilegales enumerados.
- Reset y estados CPU definidos.
- Trazas esperadas de NOP, load, branch, stack e interrupción.
- Contrato CORE validado o extensión aprobada.

Ninguna familia se declarará completa solo porque produzca registros finales correctos.

