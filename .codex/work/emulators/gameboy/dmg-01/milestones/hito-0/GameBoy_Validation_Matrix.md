# Matriz de validación DMG-01

## Convenciones

- Baseline físico: `DMG-01 / DMG-CPU B` salvo indicación distinta.
- `Exacto` significa igualdad de estado y evento en el ciclo indicado.
- Un golden visual o sonoro debe acompañarse de metadatos; nunca se actualiza automáticamente tras un fallo.
- Los commits y hashes de suites se fijarán al comenzar el hito correspondiente.

## Hito 0

| ID | Requisito observable | Fuente/oráculo | Estímulo y observación | Tiempo/tolerancia | Capa | Estado |
|---|---|---|---|---|---|---|
| H0-01 | Modelo y revisión únicos | Registro de fuentes/evidencia B | Revisar spec y decision record | Exacto | Arquitectura | Completo |
| H0-02 | Todos los rangos tienen propietario | Manual + mapa contractual | Cobertura `0000-FFFF` sin solapes ambiguos | Exacto | Arquitectura | Completo |
| H0-03 | Todos los registros tienen propietario o hueco explícito | Manual + ledger | Cobertura FF00-FF7F y FFFF | Exacto | Arquitectura | Completo |
| H0-04 | Relojes usan vocabulario único | Manual + cálculo | Verificar ratios y 70224 T/frame | Cálculo exacto | Arquitectura | Completo |
| H0-05 | Interrupciones trazadas | Manual | Cinco fuentes, bits, prioridades y vectores | Exacto | Arquitectura | Completo |

## Hito 1 - SM83

| ID | Requisito observable | Fuente/oráculo | Estímulo y observación | Tiempo/tolerancia | Capa | Estado |
|---|---|---|---|---|---|---|
| CPU-01 | 256 opcodes base clasificados | Manual + evidencia hardware | Ledger sin entrada implícita | Exacto | Estructural | Planificado |
| CPU-02 | 256 opcodes CB clasificados | Manual + evidencia hardware | Ledger sin entrada implícita | Exacto | Estructural | Planificado |
| CPU-03 | Semántica y flags | Vectores independientes | Particiones 00/01/7F/80/FE/FF/AA/55 | Exacto | Unitario | Planificado |
| CPU-04 | Longitud y decode | Ledger independiente | Decode y disassembly sobre cada encoding | Exacto | Unitario | Planificado |
| CPU-05 | Accesos de bus ordenados | Manual/tests hardware | MR/MW con dirección/dato por ciclo | Exacto por M/T-cycle | Componente | Planificado |
| CPU-06 | Timing condicional | Manual + diagnósticos | Branch taken/not taken y operandos `(HL)` | 0 T de tolerancia | Componente | Planificado |
| CPU-07 | Stack y endian | Cálculo directo | PUSH/POP/CALL/RET en límites | Exacto | Unitario | Planificado |
| CPU-08 | Entrada/retorno IRQ | Manual + Mooneye | IF/IE/IME, push PC, vector y RETI | Exacto por ciclo | Componente | Planificado |
| CPU-09 | EI diferido y DI | Mooneye/Blargg + hardware | IRQ en fronteras adyacentes | Exacto | Componente | Planificado |
| CPU-10 | HALT y halt bug | Mooneye/Blargg + hardware | Matriz IME x IE&IF | Exacto | Componente | Planificado |
| CPU-11 | STOP/reset | Hardware B | Wake sources y PC/clock | Según contrato cerrado | Componente | Pendiente de evidencia |
| CPU-12 | Opcodes ilegales | Hardware B | Ejecutar cada encoding ilegal aislado | Exacto/clasificado | Componente | Pendiente de evidencia |

## Hito 2 - Memoria y boot

| ID | Requisito observable | Fuente/oráculo | Estímulo y observación | Tiempo/tolerancia | Capa | Estado |
|---|---|---|---|---|---|---|
| MEM-01 | Mapa base | Manual | Read/write por frontera de región | Exacto | Componente | Planificado |
| MEM-02 | Echo WRAM | Manual | Patrones en C000-DDFF y E000-FDFF | Exacto | Componente | Planificado |
| MEM-03 | HRAM 127 bytes | Manual | FF80-FFFE, límites con IE | Exacto | Componente | Planificado |
| BOOT-01 | Overlay interno | Patente + manual | Reset y fetch 0000 desde Boot ROM | Exacto | Integración | Planificado |
| BOOT-02 | Desactivación one-way | Patente + boot trace | Escritura FF50 y lectura externa posterior | Exacto | Integración | Planificado |
| BOOT-03 | Secuencia a 0100 | Boot ROM por hash + traza | Ejecutar hasta entrega al cartucho | Primera divergencia | Sistema | Planificado |
| BOOT-04 | Doble reset determinista | Contrato interno | Dos resets, mismas entradas, comparar trazas | Identidad total | Sistema | Planificado |

## Hito 3 - Timer, IRQ y DMA

| ID | Requisito observable | Fuente/oráculo | Estímulo y observación | Tiempo/tolerancia | Capa | Estado |
|---|---|---|---|---|---|---|
| TIM-01 | Frecuencias TAC | Manual + Mooneye | Contar incrementos por selección | 0 T | Componente | Planificado |
| TIM-02 | Escritura DIV/TAC y flancos | Mooneye/hardware B | Barrido por fase | 0 T | Componente | Planificado |
| TIM-03 | Overflow/reload | Mooneye/hardware B | TIMA=FF con escrituras coincidentes | 0 T | Componente | Planificado |
| IRQ-01 | Prioridad simultánea | Manual | Cinco IF activos y máscaras variables | Exacto | Integración | Planificado |
| IRQ-02 | IF write vs request | Hardware B | Escritura coincidente con fuente | 0 T | Integración | Pendiente de evidencia |
| DMA-01 | 160 bytes y orden | Manual + Mooneye | Transferencia desde patrón conocido | 0 T; 160 bytes | Componente | Planificado |
| DMA-02 | CPU bus restriction | Hardware B/Mooneye | Código en HRAM y fuera durante DMA | Exacto por acceso | Integración | Planificado |
| DMA-03 | Restart durante DMA | Hardware B | Reescribir FF46 por fase | 0 T | Integración | Pendiente de evidencia |
| MEM-04 | FEA0-FEFF | Hardware B | Read/write por dirección y fase | Exacto por revisión | Componente | Pendiente de evidencia |

## Hito 4 - PPU

| ID | Requisito observable | Fuente/oráculo | Estímulo y observación | Tiempo/tolerancia | Capa | Estado |
|---|---|---|---|---|---|---|
| PPU-01 | 456 dots y 154 líneas | Manual + cálculo | Frame vacío, registrar modes/LY | 0 dots | Componente | Planificado |
| PPU-02 | Mode transitions | Hardware diagnostics | Observar STAT/LY por dot | 0 dots | Componente | Planificado |
| PPU-03 | VBlank IRQ | Manual | Transición línea 143->144 | 0 dots | Integración | Planificado |
| PPU-04 | STAT line/edges | Hardware diagnostics | Activar fuentes solapadas | 0 dots | Integración | Planificado |
| PPU-05 | VRAM/OAM blocking | Manual + hardware | Accesos en modes 0/1/2/3 | Exacto por acceso | Integración | Planificado |
| PPU-06 | Background/window | dmg-acid2 + captura | Tiles signed/unsigned, WX/WY | Píxel exacto | Sistema | Planificado |
| PPU-07 | Sprite selection | dmg-acid2 + captura | 40 OAM, >10 por línea | Píxel exacto | Sistema | Planificado |
| PPU-08 | Sprite priority | dmg-acid2 + hardware | X igual/distinto, OAM order | Píxel exacto | Sistema | Planificado |
| PPU-09 | Mode 3 variable | Suite avanzada + hardware B | SCX, window y sprite stalls | 0 dots | Componente | Pendiente de suite fijada |
| PPU-10 | LCD enable/disable | Hardware B | Toggle por dot/mode | 0 dots | Componente | Pendiente de evidencia |
| PPU-11 | Boot logo frames | Boot ROM/hash + captura | Reset hasta 0100 | Golden documentado | Sistema | Planificado |

## Hito 5 - Joypad, cartucho y MBC1

| ID | Requisito observable | Fuente/oráculo | Estímulo y observación | Tiempo/tolerancia | Capa | Estado |
|---|---|---|---|---|---|---|
| IO-01 | Matriz JOYP | Manual | Todos los selectores y botones | Exacto | Componente | Planificado |
| IO-02 | Joypad IRQ por flanco | Hardware B | Input alrededor de selección/muestreo | 0 T | Integración | Planificado |
| CART-01 | Parser seguro | Cabecera + casos construidos | Truncados, tamaños y tipos inválidos | Rechazo exacto | Unitario | Planificado |
| CART-02 | ROM-only | ROM construida | Fronteras 0000/3FFF/4000/7FFF | Exacto | Integración | Planificado |
| MBC1-01 | Banking ROM/RAM | Contrato MBC1 + ROM test | Todos los bits/modos y bancos especiales | Exacto | Componente | Planificado |
| SAVE-01 | Persistencia recuperable | Casos construidos | Save válido, truncado, readonly, error | Sin pérdida previa | Integración | Planificado |

## Hito 6 - APU

| ID | Requisito observable | Fuente/oráculo | Estímulo y observación | Tiempo/tolerancia | Capa | Estado |
|---|---|---|---|---|---|---|
| APU-01 | NR52 power/reset | Manual + dmg_sound/hardware | Power off/on y writes bloqueadas | Exacto por ciclo | Componente | Planificado |
| APU-02 | Pulse 1/2 | Patent/manual/tests | Duty, frequency, length, envelope | Muestra digital exacta | Componente | Planificado |
| APU-03 | Sweep | Tests/hardware B | Overflow, negate, trigger | Exacto por sequencer tick | Componente | Planificado |
| APU-04 | Wave channel/RAM | Tests/hardware B | Playback y CPU access por fase | Exacto por revisión | Componente | Pendiente de evidencia |
| APU-05 | Noise | Tests + cálculo LFSR | 7/15-bit y divisores | Secuencia exacta | Componente | Planificado |
| APU-06 | Routing L/R | Patente + cálculo | NR50/NR51 sobre canales aislados | Digital exacto | Componente | Planificado |
| APU-07 | Captura host | Señal digital conocida | Resample headless fijo | Tolerancia documentada | Adaptador | Planificado |

## Hito 7 - Serie y MBC

| ID | Requisito observable | Fuente/oráculo | Estímulo y observación | Tiempo/tolerancia | Capa | Estado |
|---|---|---|---|---|---|---|
| SER-01 | Clock interno | Manual + Mooneye | Transferir patrones 00/FF/AA/55 | 0 T por bit | Componente | Planificado |
| SER-02 | Clock externo | Peer determinista | Flancos controlados y pausas | Exacto | Integración | Planificado |
| SER-03 | Disconnect/reset | Contrato I/O | Transferencia parcial | Estado exacto | Integración | Planificado |
| MBC2-01 | Banking/RAM nibble | ROM test independiente | Address bit, RAM enable y reads | Exacto | Componente | Planificado |
| MBC3-01 | Banking/RTC latch | ROM test + test clock | Bancos, latch, halt y carry | Exacto | Componente | Planificado |
| MBC5-01 | Banking ampliado | ROM test independiente | Bits altos/bajos y RAM | Exacto | Componente | Planificado |

## Hito 8 - Sistema

| ID | Requisito observable | Fuente/oráculo | Estímulo y observación | Tiempo/tolerancia | Capa | Estado |
|---|---|---|---|---|---|---|
| SYS-01 | Construcción/destrucción repetida | Contrato CORE | Crear, reset, ejecutar, destruir xN | Sin leaks/estado stale | Sistema | Planificado |
| SYS-02 | Ejecución prolongada determinista | ROM abierta/hash | Misma entrada y seed | Hash de traza/frame/audio | Sistema | Planificado |
| SYS-03 | Compatibilidad representativa | Matriz de ROMs aportadas | Escenarios versionados | Resultado por escenario | Sistema | Planificado |
| SYS-04 | Rendimiento | Perfil fijo | Sesión headless e interactiva | Umbral documentado | Sistema | Planificado |
| SYS-05 | Límites publicados | Contratos y resultados | Comparar soporte real con guía | Sin claims no probados | Release | Planificado |

## Política de goldens

Cada golden incluirá:

- productor y revisión de hardware;
- ROM, commit y hash;
- estado inicial y entradas;
- warm-up y ventana de captura;
- formato de píxel o muestra;
- normalización aplicada;
- tolerancia y razón;
- comando o procedimiento de regeneración.

Un cambio de golden se revisará como cambio de comportamiento, no como actualización automática de expectativas.

