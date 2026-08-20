# Contrato de E/S y medios

## Capas

Cada interfaz se divide en:

1. Interfaz visible por la máquina.
2. Simulación determinista del dispositivo o protocolo.
3. Formato de medio o flujo.
4. Adaptador de host.

Un evento SDL, una lectura de archivo o el reloj del sistema operativo nunca mutarán directamente un registro CPU-visible.

## Joypad

### Interfaz

- Selector/datos: P1/JOYP en `FF00`.
- Datos: cuatro entradas activas a nivel bajo con pull-up.
- Selección: dos líneas de grupo para direcciones y botones.
- Botones: Right, Left, Up, Down, A, B, Select y Start.
- Ausencia de pulsación: entradas altas.
- Simultaneidad: se representa el estado completo, sin descartar combinaciones opuestas.

### Estado determinista

El adaptador de host genera eventos `(T-cycle, botón, pressed)`. `DMGCPUIO` consume todos los vencidos antes de evaluar la matriz en el punto contractual. Las teclas repetidas del SO no producen pulsaciones extra: solo cambian estado los flancos lógicos.

### Casos de prueba

- Ningún grupo, direcciones, botones y ambos seleccionados.
- Cada botón y combinaciones.
- Right+Left, Up+Down y múltiples botones.
- Cambio de selección con botón ya pulsado.
- Pulsación un T-cycle antes, en y después del muestreo.
- Reset, pérdida de foco y reconexión del adaptador.

## Enlace serie

### Interfaz

- Registro de datos SB (`FF01`).
- Control SC (`FF02`).
- Líneas lógicas SIN, SOUT y SCK.
- Transferencia de ocho bits.
- Reloj interno nominal derivado del master o reloj externo proporcionado por peer.
- Estado desconectado con nivel de entrada definido por evidencia.

### Máquina de estados

| Estado | Entrada | Efecto visible | Salida |
|---|---|---|---|
| Idle | Start no activo | SB estable | Idle |
| Armed-internal | Start + reloj interno | Inicializa contador/fase | Shifting |
| Armed-external | Start + reloj externo | Espera flanco SCK | Shifting/Idle |
| Shifting | Flanco válido | Desplaza SOUT/SIN y contador | Shifting/Complete |
| Complete | Octavo bit | Actualiza SB/SC y pide IRQ | Idle |
| Aborted/reset | Escritura/reset | Estado según contrato | Idle/Armed |

El peer puede ser otra instancia emulada, un stream de prueba o un dispositivo desconectado. El protocolo no bloqueará la ejecución esperando tráfico real.

### Casos de prueba

- Internal/external clock.
- Bits 00, FF, AA, 55 y secuencias.
- Peer ausente.
- Transferencia parcial y reset.
- Cambio de SB/SC durante transferencia.
- Dos masters o ausencia de clock, si el circuito puede observarlo.
- Interrupción en el punto de finalización.

## Game Pak

### Interfaz de máquina

- Ventanas ROM `0000-7FFF`.
- Ventana externa `A000-BFFF`.
- Escrituras de control MBC en el rango ROM.
- Inserción seleccionada antes de power-on en el alcance inicial.
- Hot-plug físico queda diferido; una orden de frontend deberá reiniciar o rechazar el cambio.

### Dispositivo determinista

`GamePak` contiene una implementación de controlador seleccionada por una fábrica validada. El MBC decide banco y almacenamiento; `GameBoyMemory` solo enruta el acceso.

### Formato

El lector debe validar antes de construir el dispositivo:

- tamaño mínimo de cabecera;
- tipo de cartucho;
- tamaño declarado de ROM y RAM;
- longitud real y overflow de cálculos;
- checksum como diagnóstico, sin usarlo para acceder fuera de límites;
- combinaciones tipo/tamaño imposibles;
- datos truncados o adicionales;
- nombre de save derivado de identidad estable, no de puntero o ruta temporal.

### Persistencia

- RAM/RTC se mantienen como estado emulado.
- Escritura de archivo en puntos seguros y mediante operación acotada.
- Write protection, error de disco y archivo corrupto no destruyen el estado previo.
- Save/load registra tipo de MBC y tamaño; un save incompatible se rechaza.

### Controladores por fase

| Controlador | Hito | Alcance |
|---|---:|---|
| ROM-only | 2 | ROM 32 KiB, sin banking |
| MBC1 | 5 | ROM/RAM banking y modos |
| MBC2 | 7 | RAM interna nibble y selección por address bit |
| MBC3 | 7 | ROM/RAM y RTC con latch |
| MBC5 | 7 | Bancos ampliados y RAM |
| Otros | 8+ | Contrato individual |

## RTC

El RTC MBC3 recibirá una interfaz de tiempo, nunca `now()` directamente en el controlador.

- Pruebas: reloj manual determinista.
- Ejecución interactiva: reloj de host convertido en eventos o delta controlado.
- Save state: conserva epoch y estado latched según contrato.
- Cambio hacia atrás, suspensión y grandes saltos deben tener política explícita.

## Pantalla

- Entrada: shade indices 0-3 y límites de frame.
- Salida: framebuffer host 160x144 escalable.
- Paleta visual: configurable, sin alterar BGP/OBP ni prioridades.
- Frame blending/LCD persistence: efecto de presentación opcional.
- Headless: el PPU debe funcionar y poder validarse sin crear ventana.

## Audio

- Entrada: amplitudes digitales L/R fechadas por el APU.
- Resampler: determinista para una configuración fija.
- Buffer host: no retroalimenta el tiempo emulado; underrun/overrun son métricas de frontend.
- Speaker mono y auriculares estéreo son rutas de presentación; el routing digital se valida antes de mezclar.
- Headless: captura de muestras sin dispositivo SDL.

## Boot ROM y ROMs

- Boot ROM externa, 256 bytes, hash configurado.
- Falta o tamaño inválido: error explícito.
- No se descarga automáticamente firmware propietario.
- ROM comercial: solo ruta aportada por usuario.
- Diagnósticos redistribuibles: commit, licencia y hash fijados.

## Reset y reconexión

- Reset de máquina reinicia protocolo, colas emuladas y selección de boot.
- Eventos de host antiguos no sobreviven al reset.
- Un peer serie desconectado no deja un puntero obsoleto.
- Cambiar Game Pak no conserva MBC, latch RTC o RAM de otro cartucho.
- Finalización intenta persistir de forma recuperable y reporta errores.

## Evidencia de cierre

Hito 0 fija capas, propietarios, estados protocolarios y fallos a probar. Polaridades y timing fino que dependan de la revisión permanecen en el registro de incógnitas con un hito asignado.

