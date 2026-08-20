# Nintendo Game Boy DMG-01 - Hito 0

Estado: **completado documentalmente**.  
Fecha de cierre: 2026-08-17.  
Alcance de esta entrega: especificación y contratos; no contiene implementación.

## Decisión de referencia

El primer objetivo de EMULATORS será una Nintendo Game Boy monocroma `DMG-01`, tomando como referencia determinista el SoC `DMG-CPU B`. Las diferencias con `DMG-CPU`, `DMG-CPU A`, `DMG-CPU C`, MGB, SGB y CGB se conservarán como variantes explícitas, no como comportamiento mezclado silenciosamente.

La CPU se modelará como una familia reutilizable nueva, `FSM83::SM83`. Los bloques integrados del SoC se representarán como chips lógicos separados con nombres vinculados al encapsulado real:

- `GAMEBOY::DMGCPUVideo`.
- `GAMEBOY::DMGCPUSound`.
- `GAMEBOY::DMGCPUTimers`.
- `GAMEBOY::DMGCPUDMA`.
- `GAMEBOY::DMGCPUInterrupts`.
- `GAMEBOY::DMGCPUIO`.

Estos nombres expresan bloques funcionales del mismo `DMG-CPU`; no afirman que existiesen como circuitos integrados físicamente separados.

## Índice de la entrega

- [GameBoy_machine_spec.yaml](GameBoy_machine_spec.yaml): especificación estructurada de la máquina.
- [GameBoy_Hardware_Contract.md](GameBoy_Hardware_Contract.md): alcance, fidelidad, relojes, buses, arranque y señales.
- [GameBoy_Component_Decisions.md](GameBoy_Component_Decisions.md): clasificación `reuse/configure/derive/adapt/new`, propiedad e integración futura.
- [GameBoy_Register_and_Memory_Map.md](GameBoy_Register_and_Memory_Map.md): mapas de memoria, registros y propietarios.
- [GameBoy_SM83_Workbook.md](GameBoy_SM83_Workbook.md): contrato previo a la implementación del procesador.
- [GameBoy_DMGCPU_Chip_Contracts.md](GameBoy_DMGCPU_Chip_Contracts.md): contratos iniciales de los bloques del SoC.
- [GameBoy_IO_Contract.md](GameBoy_IO_Contract.md): joypad, enlace serie, Game Pak y adaptadores de host.
- [GameBoy_Sources.md](GameBoy_Sources.md): jerarquía de fuentes, diagnósticos y política de ROM.
- [GameBoy_Validation_Matrix.md](GameBoy_Validation_Matrix.md): observables y puertas de prueba de los hitos.
- [GameBoy_Risks_and_Unknowns.md](GameBoy_Risks_and_Unknowns.md): incógnitas, riesgos y método de resolución.
- [GameBoy_Milestones.md](GameBoy_Milestones.md): alcance y criterios de salida de los hitos 0 a 8.

## Puerta de salida del Hito 0

| Criterio | Estado | Evidencia |
|---|---|---|
| Modelo y revisión de referencia fijados | Cerrado | `DMG-01`, `DMG-CPU B` |
| Alcance y exclusiones fijados | Cerrado | Contrato de hardware |
| Vocabulario temporal único | Cerrado | T-cycle canónico; M-cycle = 4 T-cycles |
| Componentes y namespaces clasificados | Cerrado | Registro de decisiones |
| Mapa completo de memoria con propietario | Cerrado | Mapa de memoria |
| Rango de registros con propietario o clasificación reservada | Cerrado | Ledger de registros |
| Rutas de interrupción documentadas | Cerrado | Contrato de hardware |
| Estrategia de Boot ROM y medios fijada | Cerrado con dependencia externa | ROM propietaria aportada por el usuario |
| Fuentes y oráculos jerarquizados | Cerrado | Registro de fuentes |
| Incógnitas visibles y asignadas a un hito | Cerrado | Registro de riesgos |
| Criterios de aceptación de hitos posteriores | Cerrado | Matriz y plan de hitos |
| Decisiones estructurales que bloqueen Hito 1 | Ninguna | Revisión del conjunto documental |

## Decisiones que no deben reinterpretarse durante la implementación

1. `SM83` no derivará de `FZ80::CZ80`; la proximidad con 8080/Z80 solo permite reutilizar patrones o código cuya compatibilidad se demuestre independientemente.
2. La unidad canónica por debajo del frontend será el T-cycle de 4,194304 MHz.
3. `GameBoyComputer` será el coordinador determinista; ningún chip consultará tiempo de pared.
4. CPU, PPU y DMA tendrán vistas explícitas de memoria y arbitraje común.
5. Los valores de encendido no se confundirán con el estado dejado por la Boot ROM.
6. Una ROM comercial o Boot ROM propietaria no se incorporará al repositorio.
7. El arranque sintético, si se añade, será un modo explícito y no se presentará como arranque físico.
8. Otro emulador solo podrá servir como comparación diferencial, nunca como único oráculo.

## Siguiente puerta

El Hito 1 puede comenzar cuando se cree el ledger completo de los 256 opcodes base y 256 opcodes `CB`, se resuelvan las preguntas marcadas `bloquea H1` y se confirme que el contrato de transacciones de CORE puede representar los accesos ordenados del SM83 sin perder granularidad observable.

