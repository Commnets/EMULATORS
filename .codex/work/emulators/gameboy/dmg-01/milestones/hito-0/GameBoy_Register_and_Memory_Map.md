# Mapa de memoria y registros DMG-01

## Mapa visible por CPU

| Inicio | Fin | Tamaño | Propietario | Observaciones |
|---:|---:|---:|---|---|
| `0000` | `3FFF` | 16 KiB | Game Pak | Banco ROM 0; `0000-00FF` puede quedar oculto por Boot ROM |
| `4000` | `7FFF` | 16 KiB | Game Pak/MBC | Banco ROM seleccionable |
| `8000` | `9FFF` | 8 KiB | `DMGCPUVideo` | VRAM; acceso CPU condicionado por modo LCD |
| `A000` | `BFFF` | 8 KiB window | Game Pak/MBC | RAM, RTC o selección del controlador |
| `C000` | `DFFF` | 8 KiB | `GameBoyMemory` | WRAM externa al SoC |
| `E000` | `FDFF` | 7680 B | `GameBoyMemory` | Eco de `C000-DDFF` |
| `FE00` | `FE9F` | 160 B | `DMGCPUVideo` | OAM; acceso CPU condicionado por PPU/DMA |
| `FEA0` | `FEFF` | 96 B | Región no utilizable | Valor y efectos por revisión pendientes |
| `FF00` | `FF7F` | 128 B | Bloques `DMGCPU*` | E/S memory-mapped y huecos reservados |
| `FF80` | `FFFE` | 127 B | `GameBoyMemory` | HRAM interna al SoC |
| `FFFF` | `FFFF` | 1 B | `DMGCPUInterrupts` | IE |

## Overlay de arranque

- Tras power-on/reset, la ROM interna ocupa `0000-00FF` en la vista CPU.
- El Game Pak sigue siendo propietario físico de la misma dirección externa.
- Una escritura de desactivación en `FF50` cambia la selección a Game Pak.
- La reactivación sin reset físico no se soportará salvo evidencia contraria para la revisión.
- La vista debug debe poder mostrar por separado ROM interna y externa.

## Ledger de registros

`Estado inicial` distingue `power-on` de `post-boot`; no se fijará un valor común sin evidencia.

| Dirección | Nombre | Acceso general | Propietario | Estado inicial contractual |
|---:|---|---|---|---|
| `FF00` | P1 / JOYP | R/W asimétrico | `DMGCPUIO` | Por revisar power-on; post-boot por traza |
| `FF01` | SB | R/W | `DMGCPUIO` | Por revisión |
| `FF02` | SC | R/W con bits abiertos | `DMGCPUIO` | Por revisión |
| `FF03` | Reservado | No definido | Memoria/decoder | No asumir `FF` |
| `FF04` | DIV | R; escritura reinicia divisor | `DMGCPUTimers` | Diferenciar power-on/post-boot |
| `FF05` | TIMA | R/W | `DMGCPUTimers` | Por revisión |
| `FF06` | TMA | R/W | `DMGCPUTimers` | Por revisión |
| `FF07` | TAC | R/W con bits abiertos | `DMGCPUTimers` | Por revisión |
| `FF08-FF0E` | Reservados | No definido | Memoria/decoder | Ledger por dirección en Hito 3 |
| `FF0F` | IF | R/W con bits no usados | `DMGCPUInterrupts` | Por revisión |
| `FF10` | NR10 | R/W asimétrico | `DMGCPUSound` | Post-boot no equivale a power-on |
| `FF11` | NR11 | R/W asimétrico | `DMGCPUSound` | Igual |
| `FF12` | NR12 | R/W | `DMGCPUSound` | Igual |
| `FF13` | NR13 | W / lectura abierta según bits | `DMGCPUSound` | Igual |
| `FF14` | NR14 | R/W asimétrico; trigger | `DMGCPUSound` | Igual |
| `FF15` | Reservado | No definido | Memoria/decoder | No asumir `FF` |
| `FF16` | NR21 | R/W asimétrico | `DMGCPUSound` | Por revisar |
| `FF17` | NR22 | R/W | `DMGCPUSound` | Por revisar |
| `FF18` | NR23 | W / lectura parcial | `DMGCPUSound` | Por revisar |
| `FF19` | NR24 | R/W asimétrico; trigger | `DMGCPUSound` | Por revisar |
| `FF1A` | NR30 | R/W | `DMGCPUSound` | Por revisar |
| `FF1B` | NR31 | W / lectura parcial | `DMGCPUSound` | Por revisar |
| `FF1C` | NR32 | R/W con bits abiertos | `DMGCPUSound` | Por revisar |
| `FF1D` | NR33 | W / lectura parcial | `DMGCPUSound` | Por revisar |
| `FF1E` | NR34 | R/W asimétrico; trigger | `DMGCPUSound` | Por revisar |
| `FF1F` | Reservado | No definido | Memoria/decoder | No asumir `FF` |
| `FF20` | NR41 | W / lectura parcial | `DMGCPUSound` | Por revisar |
| `FF21` | NR42 | R/W | `DMGCPUSound` | Por revisar |
| `FF22` | NR43 | R/W | `DMGCPUSound` | Por revisar |
| `FF23` | NR44 | R/W asimétrico; trigger | `DMGCPUSound` | Por revisar |
| `FF24` | NR50 | R/W | `DMGCPUSound` | Por revisar |
| `FF25` | NR51 | R/W | `DMGCPUSound` | Por revisar |
| `FF26` | NR52 | R/W asimétrico; power | `DMGCPUSound` | Revisión crítica |
| `FF27-FF2F` | Reservados | No definido | Memoria/decoder | Ledger por dirección en Hito 6 |
| `FF30-FF3F` | Wave RAM | R/W condicionado | `DMGCPUSound` | Acceso durante reproducción por evidencia |
| `FF40` | LCDC | R/W | `DMGCPUVideo` | Power-on/post-boot separados |
| `FF41` | STAT | R/W asimétrico | `DMGCPUVideo` | Bits de modo/LYC calculados |
| `FF42` | SCY | R/W | `DMGCPUVideo` | Por revisión |
| `FF43` | SCX | R/W | `DMGCPUVideo` | Por revisión |
| `FF44` | LY | R; escritura con efecto por verificar | `DMGCPUVideo` | Derivado del raster |
| `FF45` | LYC | R/W | `DMGCPUVideo` | Por revisión |
| `FF46` | DMA | R/W/trigger | `DMGCPUDMA` | Por revisión |
| `FF47` | BGP | R/W | `DMGCPUVideo` | Por revisión |
| `FF48` | OBP0 | R/W | `DMGCPUVideo` | Por revisión |
| `FF49` | OBP1 | R/W | `DMGCPUVideo` | Por revisión |
| `FF4A` | WY | R/W | `DMGCPUVideo` | Por revisión |
| `FF4B` | WX | R/W | `DMGCPUVideo` | Por revisión |
| `FF4C-FF4F` | Reservado/CGB | No definido en DMG | Memoria/decoder | Mantener fuera del camino DMG |
| `FF50` | Boot disable | Escritura one-way | `GameBoyMemory` | Activo tras reset físico |
| `FF51-FF7F` | Reservado/CGB/undocumented | Por clasificar | Memoria/decoder | No inventar registros DMG |
| `FFFF` | IE | R/W con bits no usados | `DMGCPUInterrupts` | Por revisión |

## IF, IE e IME

- Bits 0-4 de IF/IE corresponden a VBlank, STAT, timer, serie y joypad.
- Bits restantes tendrán lectura/escritura documentada por revisión; no forman nuevas fuentes.
- IME no es un registro memory-mapped: pertenece al estado interno de `FSM83::SM83`.
- La petición de una fuente fija IF incluso si IE o IME no permiten atenderla, sujeto a prueba de la fuente concreta.

## Vistas y restricciones

| Recurso | Vista CPU | Vista PPU | Vista DMA |
|---|---|---|---|
| Cart ROM/RAM | Según MBC y arbitraje DMA | No directa | Puede ser origen según evidencia |
| VRAM | Condicionada por modo LCD | Acceso interno | Origen posible por verificar |
| WRAM/echo | Normal salvo DMA | No directa | Origen |
| OAM | Condicionada por PPU y DMA | Acceso interno | Destino |
| HRAM | Conserva camino CPU durante DMA según contrato a validar | No | No |
| Registros | Side effects por propietario | Señales internas | Solo los explícitos |

Las reglas exactas de acceso denegado, valor de lectura y orden simultáneo son parte de las pruebas, no constantes globales.

## Game Pak y banking

El mapa reserva desde el principio las ventanas de ROM y RAM, pero la implementación se escalona:

1. ROM-only.
2. MBC1.
3. MBC2.
4. MBC3 con RTC inyectable.
5. MBC5.
6. Variantes adicionales bajo contrato separado.

La cabecera no se considerará confiable hasta validar tamaño, tipo, límites aritméticos y longitud real del archivo.

## Cierre del ledger en Hito 0

Todos los rangos y registros documentados tienen propietario. Los huecos están clasificados como reservados o desconocidos, no como RAM. Máscaras, valores de reset y side effects finos se completan en el contrato del componente antes de implementarlo.

