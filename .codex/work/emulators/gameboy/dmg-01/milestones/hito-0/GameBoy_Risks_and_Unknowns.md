# Riesgos e incógnitas

## Convenciones

- **Bloquea:** primer hito que no debe implementar el comportamiento sin resolverlo.
- **Estado:** `open`, `bounded` si el alcance y método están fijados, `resolved` si existe evidencia aceptada.
- Una incógnita `bounded` no bloquea Hito 0: tiene propietario, impacto y método de cierre.

## Registro

| ID | Tema | Riesgo | Bloquea | Resolución prevista | Estado |
|---|---|---|---:|---|---|
| U-001 | Nombre y alcance SM83/LR35902 | Inferir compatibilidad Z80 inexistente | 1 | Namespace `FSM83`, ledger propio y comparación encoding por encoding | `resolved` arquitectónicamente |
| U-002 | Revisión física | Mezclar comportamientos 0/A/B/C | 1 | Baseline `DMG-CPU B`; etiquetar toda evidencia por SoC | `resolved` arquitectónicamente |
| U-003 | Capacidad de CORE | Aplicar side effects de una instrucción antes del T/M-cycle correcto | 1 | Auditoría de `CPUTransaction`; prueba prototipo sin hardware DMG | `open` |
| U-004 | Reset CPU | Confundir power-on con post-boot | 1 | Traza con Boot ROM y hardware B; dos perfiles separados | `bounded` |
| U-005 | Opcodes ilegales | Tratarlos como NOP o Z80 | 1 | Clasificación de 512 entradas y diagnósticos dedicados | `bounded` |
| U-006 | EI/HALT | Errores de aceptación, wake-up o bug HALT | 1 | Máquina de estados y trazas Mooneye/Blargg en revisión B | `bounded` |
| U-007 | Orden simultáneo global | Divergencia en timer/PPU/DMA/IRQ | 3 | Tests a ±1 T-cycle; registrar primer evento divergente | `bounded` |
| U-008 | DIV/TIMA | Modelo por frecuencia oculta glitches de flanco | 3 | Divisor interno por bits y suite de edge cases | `bounded` |
| U-009 | Recarga TIMA | Escrituras coincidentes producen estado incorrecto | 3 | Estados de overflow/reload explícitos y trazas físicas | `bounded` |
| U-010 | DMA bus blocking | Bloquear demasiado o demasiado poco | 3 | Tests ejecutados desde HRAM y regiones distintas | `bounded` |
| U-011 | Reinicio de DMA | Segunda escritura durante DMA incierta | 3 | Barrido por fase sobre hardware B | `open` |
| U-012 | Lecturas FEA0-FEFF | Asumir 0xFF globalmente | 3 | Test por revisión, dirección y fase de bus | `open` |
| U-013 | PPU Mode 3 | Renderer por línea oculta stalls y FIFO | 4 | Arquitectura dot/FIFO desde el inicio; trazas avanzadas | `bounded` |
| U-014 | STAT IRQ | Tratar fuentes como IRQ independientes en vez de línea agregada | 4 | Modelo de línea y tests de flanco/simultaneidad | `bounded` |
| U-015 | LCD enable/disable | LY/mode/frame incorrectos tras toggles | 4 | Barrido de escrituras por dot y revisión B | `open` |
| U-016 | Sprite priority | Aplicar reglas CGB o sort incorrecto | 4 | Tests por X, OAM index, transparencia y 10/line | `bounded` |
| U-017 | Window counter | Derivarlo de LY en vez de estado interno | 4 | Secuencias enable/WX/WY y golden con traza | `bounded` |
| U-018 | Joypad IRQ | Generar IRQ por evento host en lugar de línea visible | 5 | Modelo de matriz y tests de selección/flanco | `bounded` |
| U-019 | MBC1 multicart/wiring | Una sola regla no cubre variantes | 5 | Contratos de placa/tipo separados; alcance inicial estándar | `bounded` |
| U-020 | APU power/reset | Estado incorrecto de NR52 y registros | 6 | Separar power-on/post-boot y usar dmg_sound/hardware | `bounded` |
| U-021 | Wave RAM activa | Acceso CPU dependiente de fase/revisión | 6 | Tests específicos en DMG-CPU B | `open` |
| U-022 | Sweep | Overflow/negate y trigger incorrectos | 6 | Máquina de estados y vectores de borde | `bounded` |
| U-023 | Audio analógico | Perseguir una forma de onda dependiente de placa | 6 | Limitar H6 a digital; filtros como perfil de presentación | `resolved` por alcance |
| U-024 | Serie externa | Nondeterminismo o deadlock con peer real | 7 | Peer emulado/event stream; bridge host fuera del núcleo | `bounded` |
| U-025 | RTC MBC3 | Tests dependen de hora real | 7 | Clock inyectable y escenarios de delta fijo | `resolved` arquitectónicamente |
| U-026 | Saves | Corrupción por fallo o tamaño incompatible | 5 | Escritura recuperable, validación y no overwrite tras error | `bounded` |
| U-027 | Boot ROM propietaria | Entrega no reproducible o inclusión ilegal | 2 | Dependencia de usuario por hash; boot libre separado | `resolved` por política |
| U-028 | Manual oficial incompleto | Convertir omisión en comportamiento | Todos | Jerarquía de evidencia y unknown explícito | `resolved` por proceso |
| U-029 | Rendimiento por T-cycle | Allocations/lookups en hot path | 4 | Arrays planos, IDs resueltos, cero allocation por ciclo y perfil | `bounded` |
| U-030 | Cambios concurrentes en CORE | Solapamiento con trabajo local existente | 1 | Releer diff y coordinar antes de cualquier edición futura | `open` |

## Incógnitas que bloquean el inicio efectivo del Hito 1

1. U-003: demostrar que CORE representa las transacciones necesarias o aprobar una extensión genérica.
2. U-004: definir estado inicial del núcleo usado por los tests aislados, separado de post-boot.
3. U-005: completar el ledger de opcodes antes de generación masiva.
4. U-030: auditar los cambios locales existentes en CPU/Transaction para evitar sobrescribir trabajo ajeno.

## Política de cierre

Una incógnita se marca `resolved` solo si incluye:

- revisión afectada;
- fuente o ROM de prueba con versión/hash;
- estado inicial y estímulo;
- observación y dominio temporal;
- conclusión y cambios que obliga a realizar;
- test de regresión previsto cuando exista observable estable.

La compatibilidad con juegos no cierra por sí sola una incógnita temporal.

