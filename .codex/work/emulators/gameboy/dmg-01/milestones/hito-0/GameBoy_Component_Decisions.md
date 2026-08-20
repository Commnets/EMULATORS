# Registro de decisiones de componentes

## Regla de clasificación

- `reuse`: usar sin cambiar comportamiento.
- `configure`: reutilizar el componente variando parámetros soportados.
- `derive`: heredar cuando el contrato del padre sea realmente compatible.
- `adapt`: envolver una interfaz válida sin afirmar identidad de hardware.
- `new`: crear comportamiento nuevo.

## Decisiones

| Componente | Variante/rol | Candidato local | Decisión | Namespace | Justificación y frontera | Validación futura |
|---|---|---|---|---|---|---|
| SM83 | Núcleo de `DMG-CPU B` | `FZ80::CZ80` | `new` | `FSM83` | Comparte conceptos con 8080/Z80, pero registro, opcodes, flags, interrupciones y puertos no cumplen el contrato Z80 | Ledger de 512 entradas, semántica y bus |
| Instrucciones SM83 | Base y prefijo CB | `FZ80::Instruction` | `adapt/new` | `FSM83` | Reutilizar patrón de `InstructionDefined`; no reutilizar una instrucción si accede a registros o flags Z80 implícitos | Tests por opcode y frontera |
| Interrupción SM83 | Cinco vectores maskables | CORE y FZ80 | `new` sobre CORE | `FSM83` | IME, EI diferido, HALT y prioridad DMG requieren modelo específico | Trazas de entrada/retorno |
| Memoria DMG | Mapa y arbitraje | Memorias MSX/ZX Spectrum | `new` | `GAMEBOY` | Se reutiliza el patrón de storages/subsets/views; el mapa, echo, overlay y bloqueo son específicos | Tests de cada rango y vista |
| Boot overlay | ROM interna 256 bytes | Overlays de Memory | `new` | `GAMEBOY` | One-way disable y estado físico/post-boot propios | Traza de reset y FF50 |
| PPU | LCD monocroma | `ZXSPECTRUM::ULA`, `MSX::VDP` | `new` | `GAMEBOY` | `GraphicalChip`, raster y screen memory son patrones; fetcher/FIFO, modos y reglas DMG son nuevos | Dot traces y golden frames |
| APU | Cuatro canales | SoundChip/PSG existentes | `new` | `GAMEBOY` | Puede reutilizar infraestructura de muestreo; canales y side effects son específicos | Registro/secuenciador/muestras |
| Timer | DIV/TIMA/TMA/TAC | Timers CIA como patrón | `new` | `GAMEBOY` | El modelo por flanco y recarga es específico | Diagnósticos de flancos |
| OAM DMA | 160 bytes | CORE stop/transactions | `new` | `GAMEBOY` | Necesita vista DMA y bloqueo dependiente de bus | Traza de 160 transferencias |
| IRQ aggregator | IF/IE | `CPUInterruptSystem` | `adapt/new` | `GAMEBOY` + `FSM83` | IF/IE son SoC; IME y aceptación son CPU | Simultaneidad y prioridad |
| Joypad | Matriz 2x4 | OS input adapters | `new/adapt` | `GAMEBOY` | Simulación nueva, host adapter reutilizable | Matriz, pull-ups y edges |
| Serie | SB/SC y link | CORE I/O | `new` | `GAMEBOY` | Protocolo y líneas DMG propios | Internal/external clock |
| Game Pak | Conector y dispositivo | Cartridge C64/ZX Spectrum | `adapt/new` | `GAMEBOY` | Reutilizar separación periférico/datos; banking no es compatible | Insert/remove/header tests |
| MBC1/2/3/5 | Controladores | Ninguno | `new` | `GAMEBOY` | Cada controlador tendrá contrato propio | Bancos y persistencia |
| RTC MBC3 | Reloj de cartucho | Ninguno | `new` | `GAMEBOY` | Necesita tiempo inyectable y latch | Tests con reloj fijo |
| Screen | Presentación 160x144 | `MCHEmul::Screen` | `adapt` | `GAMEBOY` | Hardware entrega shade index; host decide representación | Captura normalizada |
| SoundSystem | Sink de host | `MCHEmul::SoundSystem` | `adapt` | `GAMEBOY` | APU no dependerá de SDL | Buffer y resampling |
| Computer | Placa y scheduler | `MCHEmul::Computer` | `derive` | `GAMEBOY` | Patrón compatible; wiring y T-cycle específicos | Smoke de doble reset |
| Emulator | Configuración/aplicación | `MCHEmul::Emulator` | `derive` | `GAMEBOY` | Builders y CLI siguen la infraestructura | Inicialización y destrucción |
| FileIO | `.gb` y saves | FileIO existentes | `derive/new parser` | `GAMEBOY` | Contenedor nuevo; infraestructura común | Archivos truncados/corruptos |
| CommandBuilder | Diagnóstico | Builders existentes | `derive` | `GAMEBOY` | Reusar comandos estándar y añadir estado DMG | Contrato InfoStructure/fmt |

## Decisión sobre la familia CPU

El namespace será `FSM83`, no `FZ80`, `F8080` ni `FLR35902`.

- `SM83` identifica el núcleo de CPU sin atribuirle todos los periféricos del DMG-CPU.
- `LR35902` se conservará como alias histórico en documentación, no como base arquitectónica.
- Los registros `A/F`, `B/C`, `D/E`, `H/L`, `SP` y `PC` no justifican herencia Z80.
- No hay espacio separado de puertos ni registros `IX/IY`, alternativos, `I/R` o modos IM del Z80.
- Las instrucciones específicas, opcodes ilegales y prefijo CB se clasificarán desde cero.

## Decisión sobre los chips lógicos

Los nombres acordados son:

- `DMGCPUVideo`.
- `DMGCPUSound`.
- `DMGCPUTimers`.
- `DMGCPUDMA`.
- `DMGCPUInterrupts`.
- `DMGCPUIO`.

Cada uno derivará de la abstracción CORE que mejor corresponda. No compartirán una bolsa de estado mutable. Las señales comunes se expresarán mediante:

- una interfaz de arbitraje de memoria;
- líneas o eventos lógicos de interrupción;
- contadores derivados del scheduler;
- referencias no propietarias enlazadas después de construir los propietarios.

## Propiedad y orden de construcción previsto

1. Configuración de modelo y fuentes de tiempo.
2. Almacenamientos físicos y `GameBoyMemory`.
3. `FSM83::SM83`.
4. `DMGCPUInterrupts` y `DMGCPUTimers`.
5. `DMGCPUDMA`.
6. `DMGCPUVideo`.
7. `DMGCPUSound`.
8. `DMGCPUIO`.
9. Dispositivos `GamePak`, pantalla, sonido e input.
10. `GameBoyComputer`, que recibe y posee el conjunto según el patrón CORE.

El orden de destrucción será inverso. Observadores y peers serie deberán desconectarse antes de destruir sus publishers.

## Patrones locales elegidos

- `include/FZ80` y `tests/TestInstZ80`: estructura de familia de CPU, decode y trazas; no compatibilidad semántica.
- `include/ZXSpectrum/ULA.hpp`: chip gráfico sincronizado, memoria con vista de vídeo y sound wrapper.
- `include/MSX/PortManager.hpp`: separación entre decodificación y dispositivo, solo como patrón conceptual porque DMG usa E/S memory-mapped.
- `include/ZXSpectrum/Memory.hpp`: storages, subsets y vistas CPU/chip.
- `include/C64/Cartridge.hpp`: periférico con datos, inserción y cambio de mapa; MBC se diseña de nuevo.
- `include/CORE/Computer.hpp`: propiedad y ciclo de placa.
- `include/EMULATORS/Emulator.hpp`: configuración, builders y frontend.

## Superficie futura del repositorio

La siguiente estructura es una previsión, no archivos autorizados por Hito 0:

```text
include/FSM83/
src/FSM83/
projects/FSM83/
tests/TestSM83/
tests/TestInstSM83/

include/GameBoy/
src/GameBoy/
projects/GameBoy/
emulators/GameBoyEmulator/
emulators/GameBoyEmulatorC/
emulators/GameBoyPrograms/
tests/TestGameBoy/
```

Los nombres exactos de proyectos se confirmarán al iniciar el primer cambio de solución. No se añadirá un proyecto por cada chip: los bloques DMG pertenecen a la biblioteca `GameBoy`; la CPU reutilizable sí merece su propia biblioteca.

## Cambios potenciales en CORE

No se aprueba todavía ningún cambio de CORE. Antes del Hito 1 se auditará si las transacciones existentes representan:

- una instrucción dividida en M-cycles y T-cycles;
- orden de lectura/escritura por ciclo;
- interrupción aceptada entre instrucciones;
- CPU detenida por HALT sin congelar PPU/timer/APU;
- bloqueo selectivo por DMA sin detener todo el reloj;
- side effects de memoria dependientes de la fase.

Si falta capacidad, se propondrá la extensión mínima y reutilizable. No se introducirá lógica Game Boy en CORE.

