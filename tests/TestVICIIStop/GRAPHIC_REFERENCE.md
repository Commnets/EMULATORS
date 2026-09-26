# Referencia independiente del dibujo VIC-II

## Alcance

Referencia reducida PAL de texto monocromo y multicolor, con DEN previamente observado en
la línea $30, banco cero, matriz en $0400, caracteres en $1000 y
borde vertical abierto. No representa el VIC-II completo ni ejecuta VICE.
Las etiquetas de los escenarios sintéticos indican la temporización de $D011 reconstruida de
sus logs S5; no son reproducciones de sus RAM, sprites o modos originales.
En particular, el escenario sintético 10000Members NO reproduce su modo gráfico inválido 7.
El replay capturado de 10000Members descrito al final sí incorpora bitmap y modo 7.

La memoria de estímulo puede ser común. La referencia posee sus propios
buffers de matriz/color, VC, VMLI, índice de presentación, etapas de datos,
XSCROLL muestreado y píxeles pendientes. No usa cabeceras, constantes ni
métodos del VIC-II de producción para obtener resultados esperados.

## Fuentes y normalización

Revisión del comportamiento de las siguientes fuentes oficiales de VICE,
consultadas el 15-09-2026. Son enlaces de procedencia, no dependencias de red
durante la compilación o ejecución:

- https://github.com/VICE-Team/svn-mirror/blob/main/vice/src/viciisc/vicii-fetch.c
  (`vicii_fetch_graphics`, `vicii_fetch_matrix`).
- https://github.com/VICE-Team/svn-mirror/blob/main/vice/src/viciisc/vicii-cycle.c
  (`vicii_cycle`: g, dibujo, badline, BA/c, escritura CPU posterior).
- https://github.com/VICE-Team/svn-mirror/blob/main/vice/src/viciisc/vicii-mem.c
  (`vicii_read` y `d01112_read`: lectura del raster actual).
- https://github.com/VICE-Team/svn-mirror/blob/main/vice/src/c64/c64cpusc.c
  (`CLK_INC` y `FETCH_OPCODE`: orden entre ciclos CPU y avance del VIC-II).
- https://github.com/VICE-Team/svn-mirror/blob/main/vice/src/viciisc/vicii-draw-cycle.c
  (`draw_graphics8`, `dmli`, `cycle_flags_pipe`, `pixel_buffer`).
- https://github.com/VICE-Team/svn-mirror/blob/main/vice/src/viciisc/vicii-chip-model.c
  (tabla PAL y empaquetado de X en grupos de ocho puntos).
- https://www.cebix.net/VIC-Article.txt
  (Bauer: ventana de 40 columnas, XSCROLL y separación acceso/presentación).

Se comparan coordenadas de PRESENTACIÓN, compartidas por texto y sprites.
Las posiciones PAL absolutas X=$18..$157 para el texto y X=$10..$15f para la
composición se convierten a las coordenadas visibles de `ScreenMemory`: X=32..351
y X=24..359 respectivamente. El origen visible PAL está en X=$1f0, ocho puntos
antes del cero absoluto tras el retorno horizontal.
No se compara el instante eléctrico de la salida
de vídeo ni el número de llamadas al dibujador con `dbuf_offset`.

La referencia mantiene el X visible de las señales retrasadas junto a los píxeles
cuando pasan por la salida pendiente. La posición de cada grupo procede de
la tabla PAL (phi1 comienza en $194, módulo 504, empaquetado a ocho puntos).
Antes de empaquetar se normaliza respecto del origen visible $1f0. EMULATOR
obtiene sus coordenadas de su Raster real y construye el mismo
DrawContext que su pipeline actual. No hay ajuste elegido tras comparar
los resultados ni una expectativa `sourceCycle = cycle - 1`.

Los anclajes se comprueban antes de comparar la línea normal: primer
carácter en `ScreenMemory` X=32+XSCROLL, 40 identidades/colores en orden y
últimos píxeles hasta X=351. Dos mutantes comprueban que el comparador rechaza un
desplazamiento de ocho puntos y un color alterado.

El harness llama a los helpers reales de bus/dibujo, pero no a la simulación
CPU/IRQ completa. En este Raster PAL RC-RCA=4 y la escritura phi2 queda
fuera del grupo alineado actual (primer píxel posterior=8). Esa relación
se comprueba con assert; no se generaliza a NTSC ni a geometrías distintas.

## Matriz de comprobación

| Comprobación | Estímulo | Observación independiente | Estado inicial |
|---|---|---|---|
| Anclajes normales | XSCROLL 0..7 | 40 columnas y colores, ventana $18..$157 | Pendiente de ejecutar |
| Sensibilidad | Imagen desplazada 8 puntos/color cambiado | El comparador rechaza ambos mutantes | Pendiente de ejecutar |
| Línea display | 8 valores XSCROLL | Datos g/c, BA, VC/VMLI y 320 píxeles | Pendiente de ejecutar |
| Badline regular | Condición activa antes de ciclo 14 | RC=0, matriz válida y dibujo | Pendiente de ejecutar |
| Badline tardía desde idle/display | Escrituras 17, 21, 31, 34, 45, 53, 54 | Primera divergencia de bus y/o píxel | Pendiente de ejecutar |
| XSCROLL antes/durante/después del contenido | Ciclos 10, 25, 54, 60; 6->4, 6->2, 2->6 | Muestreo y continuidad | Pendiente de ejecutar |
| Decodificador multicolor | Byte literal $1b, color RAM 3/$b | Colores 00/01/10/11 y prioridad; hires dentro de MCM | Pendiente de compilar/ejecutar |
| Borde fijo 38/40 columnas | XSCROLL 0..7 | Contenido [$1f,$14f) o [$18,$158), borde independiente de XSCROLL | Pendiente de compilar/ejecutar |
| MCM normal | 38/40 columnas, 8 scrolls, colores hires/MC/mixtos | Bus, prioridad y color del texto; composición real | Pendiente de compilar/ejecutar |
| MCM badline regular/tardía | 38/40 columnas, 8 scrolls; escrituras A/B 31/21 | Matriz, texto y composición del borde separados | Pendiente de compilar/ejecutar |
| MCM XSCROLL dinámico | 38/40 columnas, ciclos 10/25/54/60, tres transiciones | Continuidad del texto y salida final | Pendiente de compilar/ejecutar |
| Estabilizador 10000Members | CMP $D012 iniciado en 50:59, 50:60 y 50:61, seguido de BEQ $00 | Lecturas $32/$32/$33; ramas 3/3/2 ciclos; continuación 51:3/51:4/51:4 | Fallo previo confirmado en 50:60; corrección productiva pendiente de compilar/ejecutar |

Se conservan los 84 escenarios iniciales: 16 normales/regulares, 56 tardíos y
12 escrituras XSCROLL. Se añaden 120 escenarios MCM: 48 normales, 16 badlines
regulares, 32 tardíos A/B y 24 transiciones XSCROLL. Cada nuevo escenario
compara el bus/texto y, en una ejecución separada, la composición real mediante
`actualizeMainBorderStatus` y `drawVisibleZone`, incluidos sus caminos de borde
completo, mixto y vacío. La segunda ejecución no reutiliza el resultado del
dibujador de texto ni avanza dos veces el mismo secuenciador.

Los fondos D021/D022/D023 son 2/5/7 y el borde D020 es 14. Color RAM permite
todos hires, todos MC o alternancia de ambos. Se compara el color del píxel
por separado de su prioridad: el par MC 01 tiene color D022 y es background.
Los anclajes literales y de geometría no leen máscaras de producción.
MCM y CSEL permanecen constantes durante cada línea; solo se escriben D011
y XSCROLL dentro del alcance original.

Resultado de la ejecución anterior, antes de esta ampliación: 76/84 aprobados;
ocho discrepancias en el caso idle con escritura D011 en ciclo 54 (primera
divergencia de bus en 55). La suite completa se interrumpió por una limpieza
del fixture que reiniciaba Raster pero conservaba ROW=167. Ahora esa limpieza
sincroniza ROW con Raster antes de proyectar BA; no se elimina el assert de VICII.
Los nuevos resultados quedan pendientes de compilación y ejecución.
No hay expectativa automática de que una discrepancia sea una corrección
confirmada: primero hay que revisar su fase y el alcance de este modelo.

## Ejecución

Compilar TestVICIIStop en Debug|Win32 (Debug|x86 desde Emulators.sln).
Ejecutar el binario recién compilado:

```powershell
& .\exe\x86\TestVICIIStopD.exe --graphic-reference
```

Sin argumentos se conserva la suite anterior y se añaden las mismas
comprobaciones. No se borran ni se cambian expectativas anteriores.

Una diferencia imprime primero ciclo de bus y valores actual/esperado,
o línea/X, foreground, color y origen esperado del píxel. Devuelve código
1 ante discrepancias o fallo de anclajes; no convierte fallos conocidos
en aprobados. Código 0 indica aprobación solo dentro del alcance declarado.

El conjunto original no cubre ECM/bitmap/modos inválidos, cambios de modo, CSEL o colores,
sprites, IRQ/CPU, borde vertical dinámico, NTSC, efectos
de varias líneas ni las demos completas. No justifica por sí sola añadir
una etapa a VICII ni garantiza ausencia de regresiones visuales.

## Replay de líneas LetsScrollitA/B

Se añaden `testLetsScrollitALineReplay` y `testLetsScrollitBLineReplay`, ejecutables
por separado con `exe/x86/TestVICIIStopD.exe --demo-line-replay`. También se incluyen
en `--graphic-reference` y en la suite completa. No se modifica VICII ni se añade
ninguna expectativa de foreground a estos replays: sólo bus y colores de salida.
Las comprobaciones de foreground anteriores se conservan sin cambios.

| Fixture | Ventana PAL y ciclos absolutos | Estímulo capturado |
|---|---|---|
| A | Líneas 52–54, 62135898–62136086 | Banco 1, D018=$ee, D016=$17; D011 en 52/3, 53/31 y 54/13; primer c inválido con color 3 |
| B | Líneas 147–156, 31812675–31813304 | Banco 0, D018=$1c inicialmente, D016=$13; carga completa de matriz en 147, D011 tardío en 155/21, D018=$1e en 155/60, cambios de D021 |

Son 819 ciclos de evidencia S5: A contiene 24 direcciones RAM conocidas y B 102.
Los registros originales no indican sprites activos/dibujados en estas ventanas.
Se desactivan en el fixture; no se reproduce el programa CPU, sus IRQ o el frame completo.
El nibble del opcode CPU se suministra por ciclo para los c-accesses inválidos.
Las escrituras VIC se aplican después del bus del mismo ciclo.

La referencia mantiene su matriz/color, RC, VCBASE y latches entre líneas.
EMULATOR usa `advanceRasterPosition` para el avance y housekeeping reales;
no se restaura el estado de cada línea desde su snapshot. B comienza en una
badline con cuarenta c-accesses válidos que cargan la matriz antes del caso tardío.
A comienza en idle: los elementos retenidos de matriz/color 23–39 se reconstruyen
de sus g-accesses en 54; los 0–22 se sobrescriben por c-accesses en 53 antes de usarse.
Esto es un checkpoint condicionado, NO la prueba de su historia anterior completa.

Un observador `VICIITestRAM::readValue` captura las direcciones realmente leídas
por `treatGraphicAccessCycle`, sin repetir su fórmula ni realizar otro g-access.
Se comparan por separado los eventos contra el log de EMULATOR (regresión) y
contra el estado calculado por VICIIGraphicReference (referencia independiente).
Una segunda ejecución recorre las mismas líneas usando `drawVisibleZone`;
la primera usa `drawGraphics`. No se avanza dos veces el mismo secuenciador.
Se informa la primera divergencia de bus con ciclo absoluto, línea/ciclo,
dirección, byte, carácter, color y VC/VMLI; la de salida con línea/X y origen.

La RAM no capturada se envenena con $5a y el color con $d. Un acceso a información
no disponible imprime INCOMPLETE y hace fallar el replay; no se rellena con cero
ni se aprueba por coincidencia. La tabla de RAM reconstruye valores consumidos,
no un volcado completo; sólo es válida para esta ventana. El extractor rechaza
contradicciones de RAM/color, cambios de banco, discontinuidades y sprites activos.

Falta la captura inicial de D021–D023 en A y de D020 en ambos logs.
La salida anuncia INCOMPLETE en la captura de colores: A usa fondos controlados
0/0/0 y ambos usan borde controlado 0. B sí conserva fondos capturados 3/6/14
y las escrituras de D021. Un replay aprobado verifica estos estímulos declarados,
NO demuestra que se haya reproducido o solucionado el defecto visual de la demo.
No se deben tratar esos colores controlados como valores originales confirmados.

`acquire_demo_replay.py` permite revisar la adquisición, no regenera ni sustituye
automáticamente las expectativas C++ ni un golden de hardware:

```powershell
python tests/TestVICIIStop/acquire_demo_replay.py emulators/LETSSCROLLITA-S5.LOG 62135991 --json
python tests/TestVICIIStop/acquire_demo_replay.py emulators/LETSSCROLLITB-S5.LOG 31813199 --json
```

Resultado antes de la ampliación del resolver: dos ejecuciones Debug|Win32
idénticas, código 1. Los 819 ciclos coinciden con los logs y la referencia;
24/26 comprobaciones de salida pasan. Sólo B/150 falla en texto/composición,
con primera diferencia X=163 (actual 3, esperado 13), tras D021=13 en
31812897, línea 150/ciclo 34. Las ventanas de badline tardía A/53 y B/155–156
pasan. Esto no identifica el defecto visual de columnas de las demos.

## Resolución diferida de colores (referencia 6569)

El test mantiene ahora dos tokens por píxel: color del texto y color compuesto.
Los valores 0–15 son literales y $20–$23 identifican D020–D023. Los tokens
permanecen en el ring de ocho píxeles hasta la resolución final, separada del
fetch, del desplazador gráfico y de su máscara foreground.

La referencia sigue `draw_colors_6569`, `draw_colors8` y `update_cregs` de
`vice/src/viciisc/vicii-draw-cycle.c`: primero aplica la escritura de color
previamente latcheada, resuelve la entrada siguiente del ring y después emite
y reemplaza la entrada actual. El punto cero ya estaba resuelto al final del
recorrido anterior. Una escritura CPU, posterior al dibujo, se recoge en el
siguiente recorrido y alcanza los registros de resolución en el posterior.
La coordenada de presentación y el origen gráfico siguen viajando con el píxel.
No se introduce un desplazamiento nuevo del texto ni se cambia su foreground.

`testColorResolutionAnchors` verifica vectores literales para D021/D022/D023,
transiciones 3→13 y 13→3 y una escritura consecutiva a 1: primer drenaje
con el color anterior; segundo con punto cero anterior y los otros siete nuevos;
tercero con punto cero de la primera escritura y los otros siete a 1; cuarto
estabilizado a 1. Un punto literal 5 no cambia y otro conserva borde 14 en la
composición mientras cambia su color de texto. No se inspecciona foreground.
Estos anclajes validan el orden del resolver de VICE, NO una captura de hardware
ni por sí solos la correspondencia absoluta entre bus CPU y coordenada de vídeo.

`testReferenceD021ColorWrites` añade 64 escenarios: 38/40 columnas, XSCROLL 0–7,
escrituras en ciclos 10/25/34/54 de una línea sin badline. Un glifo MC de byte
cero y color RAM 8 selecciona exclusivamente D021. Se compara el bus, el color
del texto y, en otra ejecución, la composición real; no se compara foreground.
El fondo cambia de 3 a 13 y el borde controlado es 0. Así se separa la escritura
de fondo de los accesos inválidos y de los caracteres de las demos.

La nueva opción ejecuta sólo los anclajes y esos escenarios:

```powershell
& .\exe\x86\TestVICIIStopD.exe --color-reference
```

`--demo-line-replay` comprueba los anclajes antes de los replays. Los 64 escenarios
también se incluyen en `--graphic-reference` y la suite completa. La salida
conserva código 1 ante diferencias: no se adapta el color esperado para aprobar.
No se modela todavía el resolver 8565 ni su grey dot; no se generaliza a NTSC.
La diferencia B/150 puede permanecer: completar el resolver no presupone que el
fallo anterior fuera exclusivamente del test ni que arregle las columnas.

Resultado Debug|Win32 del 15-09-2026: los anclajes pasan. Los 64 escenarios
conservan 16 aprobados y 48 discrepancias de color (código 1); B/150 conserva
su discrepancia en X=163. No se rebajan esas expectativas para aprobar.
No se cambia el decodificador ni la clasificación foreground de VICII.

## Frontera de lectura $D012 en 10000Members

`test10000MembersRasterReadStabilizer`, opción
`--members-raster-stabilizer`, reproduce la secuencia temporal relevante sin
modificar VIC-II: un `CMP $D012` absoluto de cuatro ciclos iniciado en 50:59,
50:60 o 50:61 y la duración calculada del `BEQ $00` que le sigue. Esta prueba
no ejecuta instrucciones CPU ni reproduce la entrada de IRQ.

El oráculo corregido distingue el ciclo del opcode del avance posterior:
`FETCH_OPCODE` en `c64cpusc.c` lee el opcode antes del primer `CLK_INC`.
Para CMP absoluto, `CMP(LOAD(p2), 1, 3)` en `6510core.c` hace la lectura
final antes del último avance. Sin espera de bus, esa lectura se produce
tres ciclos después del opcode. `d01112_read` devuelve `raster_line`, que
`vicii-cycle.c` incrementa en el ciclo PAL 1.

Por tanto, los comienzos 50:59/60/61 leen en 50:62/63 y 51:1, respectivamente:
$32/$32/$33. BEQ consume 3/3/2 ciclos y la continuación es 51:3/4/4.
La pareja que se estabiliza es 60/61, no 59/60. Son ciclos físicos PAL,
sin sprites, badline ni espera de bus en la ventana de la comparación.

```powershell
& .\exe\x86\TestVICIIStopD.exe --members-raster-stabilizer
```

El aprobado del 16-09-2026 usaba el oráculo anterior, incorrecto; no descarta
el ajuste `+1` de la lectura raster. El aprobado histórico del replay capturado
tampoco valida el instante de la escritura CPU: reproduce los tiempos del log.

La prueba corregida se ejecutó el 24-09-2026 en Debug|Win32 con el `+1`
productivo conservado: 50:59 y 50:61 aprobaron; 50:60 devolvió $33 y continuó
en 51:3, frente a $32 y 51:4 esperados. Código de salida 1. En la misma
validación, `--members-line-replay` terminó con código 0.

La corrección productiva posterior elimina únicamente ese `+1` en
`VICIIRegisters::rasterLineAtInstructionEffect()` y corrige su comentario.
El desplazamiento recibido ya identifica el ciclo efectivo de acceso.
El cambio afecta a la lectura de $D012 y del bit alto del raster en $D011;
no modifica la predicción de escrituras, IRQ, badlines ni el dibujado.
Se conservan las expectativas del test. Esta corrección todavía no se ha
compilado ni ejecutado: se espera que aprueben los tres casos, no se declara
un resultado observado del binario modificado.

Queda pendiente cerrar la fase de entrada IRQ: el log contiene comienzos
59/60, distintos de la pareja física 60/61. Esta prueba aislada no demuestra
que quitar `+1` arregle la demo ni garantiza ausencia de regresiones.

## Replay capturado 10000Members (bitmap/VSP)

`test10000MembersLineReplay`, opción `--members-line-replay`, reproduce tres
ventanas del log 10000MEMBERS-S5.LOG: rasters 50–130, 5.103 ciclos por ventana.
Los ciclos absolutos son 51049788–51054890, 51069444–51074546 y
51089100–51094102; XSCROLL inicial 0, 5 y 3 respectivamente. Se incluyen
las escrituras originales D011/D016, el paso del modo inválido 7 al bitmap
multicolor 3, banco 1 y D018=$08. Ningún sprite está activo en esas ventanas.

El extractor reconstruye los cuarenta elementos de matriz/color conservados
del fotograma anterior antes del checkpoint. Hay 3.256 bytes RAM y 366 bytes
de color RAM conocidos, idénticos en las tres ventanas. Rechaza contradicciones
de valores, discontinuidades y cambios de banco o sprites activos. El include
base y su ampliación visible guardan exclusivamente estímulos de bus; los
otros dos includes guardan diferencias capturadas, no píxeles esperados.

La referencia opt-in incorpora la dirección bitmap/ECM de `vicii-fetch.c`,
los pares de colores bitmap y el negro del modo inválido de `draw_graphics`,
y los latches PAL 6569 de modo (subida en punto 4, bajada en punto 6) de
`draw_graphics8`. Los anclajes literales usan $1b, colores de pantalla $56,
color RAM $b y direcciones $0643/$0043. No se cambian los escenarios de
texto anteriores ni el foreground del emulador.

Se compara el bus contra log y referencia, los píxeles gráficos y la composición
real por separado, y VCBASE al finalizar cada raster. También se exige haber
comparado píxeles no negros en los últimos ocho puntos del contenido visible
y rechazar una referencia desplazada ocho puntos. No basta que el extremo
derecho esté vacío: la ampliación hasta 130 contiene datos no nulos allí.

```powershell
python tests/TestVICIIStop/acquire_demo_replay.py emulators/10000MEMBERS-S5.LOG 51049788 --members 130
python tests/TestVICIIStop/acquire_demo_replay.py emulators/10000MEMBERS-S5.LOG 51069444 --members 130
python tests/TestVICIIStop/acquire_demo_replay.py emulators/10000MEMBERS-S5.LOG 51089100 --members 130
& .\exe\x86\TestVICIIStopD.exe --members-line-replay
```

Resultado del replay ampliado Debug|Win32: 15.309 ciclos capturados y 486
comparaciones de línea aprobadas (81 rasters × 3 ventanas × gráfico/composición).
Bus, VCBASE y píxeles coinciden; RAM completa para los accesos realizados.
Se compararon 252, 286 y 350 píxeles no negros en el extremo derecho visible,
respectivamente; las tres ventanas rechazaron la referencia desplazada ocho
puntos. Código de salida 0.
No se ha reproducido una divergencia de dibujo de 10000Members con este estímulo.

Límites: D020–D023 iniciales no están capturados y se controlan a cero; la salida
conserva el aviso `capture INCOMPLETE`. No se ejecuta CPU/IRQ real ni se reproduce
la historia completa anterior al checkpoint o el fotograma completo.
Para el primer D016, MCM/CSEL se infieren de la escritura posterior y XSCROLL
del dibujo registrado; no hay snapshot inicial completo de ese registro.
El color de c-access inválido se condiciona al nibble del opcode suministrado
por el log, como en los replays anteriores; no verifica independientemente
la procedencia eléctrica de ese dato en el bus CPU/color RAM. La referencia
es una interpretación independiente y reducida del código VICE, NO una ejecución
comparada de VICE ni un golden de hardware. Aprobar no identifica ni soluciona la
columna visual denunciada, ni permite atribuirla sin más a CPU/IRQ.

## Identidad de la fuente gráfica en 10000Members

`test10000MembersGraphicSourcePhase`, opción `--members-graphic-phase`, evita
que bytes bitmap iguales oculten una fase incorrecta. Usa CSEL=0, VC=165, RC=7,
banco 1 y bitmap en $6000 para producir literalmente los accesos $652f, $6537
y $653f.
Los tres contienen patrones, códigos de pantalla y colores diferentes dentro de
la prueba. Se comparan el código de pantalla y el ciclo de procedencia presentado
en cada punto físico para XSCROLL 0..7.

La prueba no modifica ni instrumenta el VIC-II productivo. Obtiene la procedencia
del código único conservado por el desplazador y la contrasta con los `pipe0`,
`pipe1` y `sourceCycle` de la referencia independiente. Dos mutantes deben ser
rechazados: seleccionar la fuente una fase demasiado pronto y desplazar ocho
puntos las coordenadas junto con la fuente.

```powershell
& .\exe\x86\TestVICIIStopD.exe --members-graphic-phase
```

El resultado Debug|Win32 del 16-09-2026 y la primera repetición con `_RCA` quedan
invalidados: la captura ya usaba coordenadas visibles alineadas, pero la referencia
seguía publicando coordenadas PAL absolutas. Esa mezcla generaba exactamente ocho
puntos de diferencia. La referencia conserva la latencia normal del latch y debe
ejecutarse de nuevo antes de atribuir cualquier diferencia a otra etapa.

## Badline tardía y extremo derecho de 10000Members

`test10000MembersLateBadLineRightEdge`, opción `--members-late-edge`, reproduce
la transición $D011=$7c->$7b de la línea 51, restaura $D011=$3b en la línea 58
y conserva sin restauraciones artificiales el estado del VIC-II hasta la línea
98. Ejecuta CSEL=0 y la secuencia XSCROLL 5, 3, 1, 7. Los tres primeros casos
escriben D011 en el ciclo 48; el cuarto lo hace en el 47 para reproducir el
avance grueso de una posición observado en el log.

La RAM de pantalla, bitmap y color contiene marcadores distintos. En los últimos
40 puntos de la línea se comparan contra la referencia independiente el VCBASE,
la máscara del borde, la identidad ciclo/código de la fuente gráfica y el par
multicolor decodificado con su prioridad y color. Un mutante desplazado ocho
puntos debe resultar distinguible.

La captura del resultado usa `DrawContext::_RCA`, que es la coordenada visible
alineada donde escribe el renderer. No usa la columna absoluta del raster: ambos
sistemas tienen orígenes diferentes y mezclarlos puede ocultar exactamente un
desfase horizontal en la comparación. La referencia aplica la misma conversión
desde la tabla PAL antes de comparar.

Cada píxel conserva además, de forma independiente, el índice del g-access y el
índice de Video Matrix/Color RAM usados por la referencia. La comparación del
extremo derecho exige que ambos coincidan, además del ciclo, código, color y dato
bitmap; así no se puede ocultar el origen real mediante valores de memoria iguales.

```powershell
& .\exe\x86\TestVICIIStopD.exe --members-late-edge
```

La referencia obtiene `VCBASE` directamente del `VC` vivo acumulado por los
g-accesses realmente ejecutados. La comparación exige coincidencia del borde,
identidad de fuente y píxeles, RAM completa y el rechazo del mutante desplazado
ocho puntos.
