# Contratos de los bloques lógicos DMG-CPU

## Regla común

Los componentes siguientes son particiones de emulación del SoC `DMG-CPU B`. Cada chip poseerá su estado y registros; el decoder de la máquina asignará direcciones sin duplicar almacenamiento. Las señales cruzadas serán interfaces explícitas y deterministas.

Para cada registro, antes de implementarlo se completarán máscara de lectura/escritura, bits abiertos, valor power-on, estado post-boot, side effects y momento exacto. El mapa de propietarios ya está cerrado en `GameBoy_Register_and_Memory_Map.md`.

## DMGCPUInterrupts

### Contrato

- Posee IF (`FF0F`) e IE (`FFFF`).
- Agrega VBlank, STAT, timer, serial y joypad.
- Expone a SM83 la máscara pendiente `IF & IE & 0x1F` y la fuente prioritaria.
- No posee IME ni decide por sí solo cuándo la CPU acepta.
- Las fuentes pueden solicitar bits aunque estén enmascaradas.
- Reset, bits 5-7 y limpieza durante acknowledge requieren evidencia de revisión.

### Señales

- Entradas: request por fuente.
- Salida: línea lógica de interrupción pendiente y selección prioritaria.
- Acknowledge: desde SM83, limpia solo la fuente atendida en el momento contractual.

### Pruebas mínimas

- Cada bit por separado.
- Cinco bits simultáneos.
- Cambio de IE con IF pendiente.
- Escritura de IF coincidente con nueva petición.
- Acknowledge coincidente con otra fuente.
- IME=0 y HALT sin perder la petición.

## DMGCPUTimers

### Contrato

- Posee divisor interno, DIV, TIMA, TMA y TAC.
- Se avanza por T-cycle aunque la CPU esté en HALT.
- Las frecuencias se derivan de bits del divisor y se modelan por flancos, no mediante acumuladores independientes de coma flotante.
- Escribir DIV altera el contador interno y puede crear un flanco observable.
- Cambiar TAC puede crear un flanco observable.
- Overflow, ventana de recarga, copia de TMA y petición IF tendrán estados explícitos.
- STOP y reset se resolverán por revisión.

### Pruebas mínimas

- Todas las selecciones TAC.
- Enable/disable alrededor de un flanco.
- Escritura DIV en cada fase relevante.
- Overflow FF->00 y recarga.
- Escrituras TIMA/TMA durante la ventana de recarga.
- Timer IRQ simultánea con escritura IF/IE.
- HALT/STOP/reset.

## DMGCPUDMA

### Contrato

- Posee registro DMA `FF46` y la máquina de transferencia OAM.
- Lee 160 bytes desde la página seleccionada y escribe OAM en orden.
- Utiliza una vista DMA independiente.
- Expone al árbitro el estado de bloqueo y las regiones que CPU puede usar.
- Una segunda escritura durante transferencia tendrá transición definida por evidencia.
- No congela PPU, timer, serial ni APU.

### Pruebas mínimas

- Origen en cada clase de región válida/inválida.
- Primer y último byte, duración y orden.
- CPU ejecutando desde HRAM y fuera de HRAM.
- Reinicio de DMA en diferentes fases.
- Coincidencia con modo OAM de PPU.
- Reset durante DMA.

## DMGCPUVideo

### Identidad y fidelidad

- Controlador LCD integrado en `DMG-CPU B`.
- 160x144 píxeles, cuatro shade indices.
- Modelo por dot con OAM scan, fetcher, FIFOs, mezcla y raster.
- La respuesta analógica del panel queda en `GameBoyScreen`.

### Estado y registros

- LCDC, STAT, SCY, SCX, LY, LYC, BGP, OBP0, OBP1, WY y WX.
- VRAM de 8 KiB y OAM de 160 bytes mediante vistas de memoria.
- Modos 2, 3, 0 y 1.
- Contador interno de línea de ventana separado de LY.
- Selección de tiles signed/unsigned, mapas, tamaño OBJ y prioridades.
- Línea STAT lógica agregada; interrupción por flanco según evidencia.

### Límites

- No se implementará inicialmente un renderer por scanline que oculte Mode 3 variable.
- No se mezclarán reglas CGB de prioridad o paleta.
- Un golden frame no prueba temporización interna.

### Pruebas mínimas

- 456 dots por línea y 154 líneas.
- Transiciones de modo y LY/LYC.
- Todas las fuentes STAT y su agregación.
- LCD enable/disable en fases sensibles.
- Bloqueo VRAM/OAM.
- Scroll, window start y contador interno.
- 8x8/8x16, límite de 10 sprites, prioridad X/OAM y flips.
- Mode 3 variable con SCX, ventana y sprites.
- VBlank y frame normalizado.

## DMGCPUSound

### Identidad y fidelidad

- APU digital del `DMG-CPU B`.
- Dos canales pulse, un canal wave y uno noise.
- Enrutamiento digital izquierda/derecha.
- Resampling, filtros y mezcla analógica fuera del chip.

### Estado y registros

- NR10-NR14: pulse 1 con sweep.
- NR21-NR24: pulse 2.
- NR30-NR34 y wave RAM: canal wave.
- NR41-NR44: noise.
- NR50-NR52: volumen, routing y power.
- Frame sequencer, length, envelope, sweep, timers de frecuencia, duty y LFSR.

### Señales

- Entrada: reloj T-cycle y escrituras de registros.
- Salida: amplitudes digitales izquierda/derecha con punto temporal documentado.
- No genera callbacks SDL.

### Pruebas mínimas

- Power on/off mediante NR52.
- Trigger de cada canal.
- Length, envelope y sweep en todos los bordes.
- Overflow/negate de sweep.
- Wave RAM mientras canal está activo.
- LFSR 7/15 bits y divisores.
- Routing NR51 y niveles NR50.
- HALT/STOP/reset y diferencias de revisión.

## DMGCPUIO

### Joypad

- Posee P1/JOYP.
- Modela P14/P15 como selectores y P10-P13 como entradas con pull-up.
- Consume eventos de botones ya fechados en tiempo emulado.
- La interrupción depende de transiciones de líneas visibles, no de una callback de tecla directa.

### Serie

- Posee SB y SC.
- Modela shift register, bit counter, SCK interno/externo, SIN y SOUT.
- Al completar ocho bits actualiza estado y solicita Serial IRQ.
- Peer ausente, externo detenido y clock malformado tienen estado definido.

### Pruebas mínimas

- Cada selector de matriz y ambos activos.
- Combinaciones de botones y pull-ups.
- Pulsación alrededor del muestreo y selección.
- Clock serie interno y externo.
- Transferencia normal, parcial, reiniciada y desconectada.
- Reset durante transferencia.

## Señales entre bloques

| Emisor | Señal | Receptor |
|---|---|---|
| `DMGCPUVideo` | VBlank request | `DMGCPUInterrupts` |
| `DMGCPUVideo` | STAT request/line | `DMGCPUInterrupts` |
| `DMGCPUTimers` | Timer request | `DMGCPUInterrupts` |
| `DMGCPUIO` | Serial request | `DMGCPUInterrupts` |
| `DMGCPUIO` | Joypad request | `DMGCPUInterrupts` |
| `FSM83::SM83` | interrupt acknowledge | `DMGCPUInterrupts` |
| `FSM83::SM83` / memory | DMA trigger write | `DMGCPUDMA` |
| `DMGCPUDMA` | bus restriction | Memory arbiter/SM83 |
| `DMGCPUVideo` | VRAM/OAM restriction | Memory arbiter |
| `DMGCPUVideo` | shade index stream/frame | `GameBoyScreen` |
| `DMGCPUSound` | digital L/R samples | `GameBoySoundSystem` |

## Orden de cierre de contratos

1. Interrupts y timers antes de Hito 3.
2. DMA antes de ejecutar tests de bus del Hito 3.
3. Video antes del primer frame del Hito 4.
4. Joypad antes del juego interactivo del Hito 5.
5. Sound antes del Hito 6.
6. Serie completa antes del Hito 7.

