# Plan de hitos Nintendo Game Boy DMG

## Hito 0 - Especificación cerrada

**Estado:** completado documentalmente.

Entregables:

- Identidad `DMG-01 / DMG-CPU B`.
- Contrato temporal por T-cycle.
- Decisión `FSM83` y chips lógicos `DMGCPU*`.
- Mapas de memoria, registros, buses e interrupciones.
- Fuentes, política de ROM, riesgos y matriz de validación.
- Clasificación de componentes e integración futura.

Salida: no queda una decisión estructural abierta que impida preparar el ledger del SM83.

## Hito 1 - SM83 autónomo

### Entrada

- Resolver U-003/U-004/U-005/U-030.
- Completar las 256 entradas base y 256 CB.
- Elegir convención de mnemónicos y oráculos.

### Alcance

- Arquitectura, registros, flags y estados.
- Fetch/decode, addressing y familias de instrucciones.
- Accesos de memoria ordenados.
- Interrupciones, EI/DI/RETI, HALT/STOP.
- Desensamblado e InfoStructure.

### Salida

- Todos los encodings clasificados.
- Semántica y timing con tests independientes.
- Reset e interrupciones con trazas.
- Ninguna instrucción ilegal cae por defecto en NOP.

## Hito 2 - Reset, memoria y Boot ROM

### Alcance

- `GameBoyMemory` y vistas iniciales.
- Game Pak ROM-only.
- Overlay 0000-00FF y FF50.
- `GameBoyComputer` mínimo sin frontend completo.
- Secuencia de boot o diagnóstico de firmware ausente.

### Salida

- Doble reset determinista.
- Primeros accesos documentados.
- Boot real llega a `0x0100` con cartucho diagnóstico válido.
- ROM propietaria sigue fuera del repositorio.

## Hito 3 - Interrupciones, timer y OAM DMA

### Alcance

- IF/IE y rutas de cinco fuentes.
- DIV/TIMA/TMA/TAC por flancos.
- DMA de 160 bytes y arbitraje.
- HRAM y regiones no utilizables según evidencia disponible.

### Salida

- Suites de timer/interrupt/DMA seleccionadas pasan en baseline B.
- Trazas de simultaneidad registran el primer evento divergente.
- CPU en HALT no congela periféricos.

## Hito 4 - PPU y primer frame

### Alcance

- Raster, modos, fetcher y FIFOs.
- Fondo, ventana, sprites y paletas DMG.
- STAT/VBlank, LY/LYC y restricciones VRAM/OAM.
- Headless framebuffer y `GameBoyScreen` básico.

### Salida

- Timing de línea/frame verificado.
- dmg-acid2 coincide con referencia normalizada.
- Tests de modos/STAT y Mode 3 seleccionados pasan.
- Boot logo produce frames y secuencia esperados.

## Hito 5 - Joypad, MBC1 y primera ejecución jugable

### Alcance

- Matriz JOYP y adaptador de input encolado.
- Parser Game Pak robusto.
- MBC1 estándar, RAM y persistencia.
- Frontend mínimo, comandos y formatters esenciales.

### Salida

- Input determinista en tests headless.
- Banking y saves MBC1 pasan límites y errores.
- Una ROM homebrew/diagnóstica interactiva funciona sin tiempo de host en el núcleo.

## Hito 6 - APU digital

### Alcance

- Cuatro canales, frame sequencer, power y wave RAM.
- Routing L/R, captura headless, wrapper de host y resampling.
- Perfil analógico inicial documentado.

### Salida

- Tests digitales seleccionados pasan.
- Capturas reproducibles con formato/tolerancia.
- No hay dependencia SDL dentro de `DMGCPUSound`.

## Hito 7 - Serie y MBC principales

### Alcance

- Serial interno/externo y peer determinista.
- MBC2, MBC3+RTC y MBC5.
- Save/RTC robustos y test clock.

### Salida

- Transferencias normales, parciales y desconectadas probadas.
- Banking y persistencia por controlador.
- RTC reproducible sin reloj real en tests.

## Hito 8 - Compatibilidad y release candidate

### Alcance

- Matriz representativa de software DMG.
- Trazas, vídeo, audio y regresiones.
- Rendimiento y uso prolongado.
- Documentación de comandos, formatos, configuración y límites.

### Salida

- Build limpio de bibliotecas, tests y ejecutables.
- Matriz de compatibilidad con hashes y escenarios.
- Comportamientos verificados, aproximados y no soportados separados.
- Arrancar juegos no sustituye las pruebas de componentes.

## Regla de avance

Un hito puede preparar infraestructura del siguiente, pero no se marca completo hasta cerrar todos sus observables. Un fallo conocido puede diferirse solo si no invalida la finalidad del hito y aparece con ID en el registro de riesgos.

