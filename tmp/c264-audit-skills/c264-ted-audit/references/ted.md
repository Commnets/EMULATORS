# TED: contratos y pruebas de fidelidad

Fuentes y páginas: [sources.md](sources.md). Direcciones en hexadecimal. Los números de página PDF son de visor, empezando en 1. Las pruebas siguientes son propuestas de auditoría, no resultados ejecutados.

## 1. Alcance y relojes

TED integra vídeo, sonido, tres contadores de 16 bits, IRQ, refresco/temporización DRAM, selección ROM y captura de entrada. No tiene sprites de hardware ni SID. Separar estado del chip, decodificación de placa, puerto CPU 7501 y presentación SDL.

El manual preliminar describe 312 líneas PAL y 262 NTSC; el cristal nominal es 17,734475 MHz PAL o 14,31818 MHz NTSC. El reloj lento resulta de dividir por 20 o 16, respectivamente: aproximadamente 886,724 kHz y 894,886 kHz. El modo rápido duplica esa frecuencia, sujeto a accesos y arbitraje. No usar el reloj promedio del CPU como reloj de los temporizadores o del audio. No importar los 63/65 ciclos del VIC-II. Construir la tabla horizontal TED desde los diagramas de operación y DMA, indicando si una unidad es dot, ciclo lento, rápido o fase.

Mantener separados el bit de estándar `$FF07.6`, el cristal de placa y la configuración del modelo. Cambiar un bit no cambia el oscilador físico. Antes de exigir un total NTSC exacto en producción, contrastar revisión y medición: las tablas preliminares y coordenadas internas no bastan para resolver todas las diferencias de fin de cuadro. Los valores de contador, ventana visible, sincronismo y recorte de pantalla son magnitudes distintas.

## 2. Mapa funcional

La ventana `$FF00-$FF3F` no equivale a 64 registros RAM. Auditar selecciones, huecos y máscaras por dirección; no aplicar `% 32` a toda ella, especialmente a `$FF3E/$FF3F`.

| Dirección | Contrato que revisar |
|---|---|
| `$00/$01`, `$02/$03`, `$04/$05` | T1, T2 y T3: bajo/alto; lecturas del contador actual; bajo detiene, alto inicia |
| `$06` | TEST bit 7; ECM 6; bitmap 5; display enable 4; 24/25 filas 3; scroll Y 2..0 |
| `$07` | desactivar inverso 7; NTSC/PAL 6; freeze 5; multicolor 4; 38/40 columnas 3; scroll X 2..0 |
| `$08` | escritura captura entradas K; lectura devuelve captura, no escaneo continuo |
| `$09` | flags de IRQ: raster 1, T1 3, T2 4, T3 6; bit 7 refleja solicitud; escribir uno reconoce la fuente |
| `$0A/$0B` | habilitaciones y comparador raster: bit 0 de `$0A` es raster alto, no una habilitación |
| `$0C/$0D` | posición de cursor, alto de 2 bits y bajo de 8 |
| `$0E/$0F` | frecuencia baja de voces 1/2 |
| `$10` | frecuencia alta de voz 2, bits 1..0 |
| `$11` | reload/DC 7; ruido 6; tono 2 en 5; tono 1 en 4; volumen 3..0 |
| `$12` | base bitmap 5..3; ROM/RAM de fetch gráfico 2; frecuencia alta de voz 1 en 1..0 |
| `$13` | base caracteres 7..2; forzar reloj lento 1; estado ROM CPU de solo lectura 0 |
| `$14` | base matriz/atributos en bits 7..3 |
| `$15-$18`, `$19` | fondos 0..3 y borde: luminancia 6..4, crominancia 3..0 |
| `$1A/$1B` | posición de recarga de caracteres, alto de 2 bits y bajo de 8 |
| `$1C/$1D` | posición raster vertical actual, alto de 1 bit y bajo de 8 |
| `$1E` | posición horizontal; verificar codificación/fase y bit no implementado |
| `$1F` | contador FLASH y línea dentro del carácter |
| `$3E/$3F` | escrituras de selección ROM/RAM, independientes del dato escrito |

Los offsets de la tabla son relativos a `$FF00`. Determinar máscaras de lectura/escritura a partir del mapa de producción y las páginas de registro; un bit sin uso no equivale a almacenamiento ordinario. No asumir que los valores descritos como normales en BASIC sean reset del silicio. No crear una entrada de lápiz óptico funcional basándose en bits reservados para expansión futura.

Una escritura a `$FF12` debe actualizar únicamente los campos que le corresponden sin destruir el estado del otro subsistema. El estado ROM de `$FF13.0` no es el selector de fuente de caracteres: probar todas las combinaciones de ambos.

## 3. Temporizadores e IRQ

Referencia directa: preliminar aportado PDF p. 15 y p. 16. Los tres timers cuentan con reloj lento, incluso con CPU rápido o detenido por DMA. Al escribir bajo se inhibe el conteo hasta escribir alto. T1 guarda recarga periódica; T2/T3 continúan desde `$FFFF` tras llegar a cero y no se detienen permanentemente tras el primer evento. Leer devuelve contador, no recarga. No añadir un latch de lectura de byte alto sin evidencia.

No reducir el contrato a `--counter == 0`: representar orden de escritura, tick, comparación, recarga y señal IRQ. Para N=0,1,2,255,256,65535 medir primera expiración y período estable. Conservar una incertidumbre explícita si la evidencia no resuelve la fase o el caso cero. La recomendación de escribir ambos bytes con menos de 125 microsegundos entre ellos no autoriza a bloquear escrituras legales; la fuga de carga de registros dinámicos detenidos es un fenómeno analógico, no un timeout fijo inventado.

Los eventos ponen flags aunque estén enmascarados. La máscara habilita la señal, no la captura. `$FF09` reconoce los bits escritos a uno; una lectura no equivale al read-clear de la CIA. `$FF0A` reemplaza su máscara y contiene el noveno bit raster; no usa el protocolo set/clear de CIA ICR. Reconocer una fuente no borra otra. Verificar habilitar un flag pendiente, deshabilitar una fuente activa, dos eventos simultáneos y escrituras RMW reales del 7501.

TED y ACIA comparten la entrada IRQ de la CPU en Plus/4: la composición conserva la solicitud de cualquier fuente activa. El registro TED no reconoce las interrupciones de ACIA. Registrar el instante del comparador raster respecto a la línea, no simplemente lanzar IRQ al pintar una fila del framebuffer.

## 4. Fetch y vídeo

Referencias: preliminar PDF pp. 3-8, 11-12, 18-20; hardware pp. 18-20. La base común es `B = (FF14 & $F8) << 8`. Atributos en `B + índice`; matriz en `B + $400 + índice`. Son bytes de RAM ordinaria, con luminancia y FLASH, no la color RAM de 4 bits del C64. El bitmap se alinea a 8 KiB mediante `$FF12.5..3`. Verificar alineación del generador de caracteres por modo, en particular uso de A10 al habilitar 256 caracteres.

Separar la vista CPU de la vista TED. Probar RAM bajo ROM, selección de ROM de caracteres, bancos internos/externos y alias de RAM física de 16 KiB. Un fetch gráfico no debe activar accidentalmente los efectos laterales de un registro CPU. Las escrituras CPU bajo overlays se resuelven según la placa, no usando la lectura de memoria como regla de escritura.

| Modo | Pruebas de color y datos |
|---|---|
| Texto estándar | 40x25 celdas 8x8; fondo 0, atributo por celda; inverso frente a 256 caracteres; cursor y FLASH |
| Texto multicolor | atributo bit 3 decide celda multicolor o hires; pares de bits y luminancia correctos; mezclar ambas clases en una fila |
| ECM | dos bits altos de código eligen fondo y quedan 64 glifos; verificar precedencia de inverso/cursor |
| Bitmap hires | dos colores por celda derivados de matriz/atributo; probar cada plano de crominancia y luminancia con valores distintos |
| Bitmap multicolor | cuatro pares 00,01,10,11; dos colores por celda y fondos globales según tabla del manual |
| Combinaciones no estándar | ECM con bitmap/multicolor: verificar por silicio o experimento; no normalizar al modo soportado más próximo |

Usar patrones que hagan distinguibles cada fuente de color: bytes `$00,$55,$AA,$FF`, un píxel por borde de celda, índices 0/39/40/999 y atributos con FLASH activado/desactivado. Los 128 códigos crominancia/luminancia producen 121 colores nominales al compartir negro; esto no especifica valores RGB exactos de una salida PAL/NTSC.

TED necesita datos de matriz y atributos en fases/líneas distintas; no agrupar todas las lecturas al principio del cuadro. Seguir contadores de fila, posición de recarga, buffers internos y fetch de glifo/bitmap. Para cada DMA registrar dirección, dato, BA, AEC y estado de bus CPU. El preliminar describe preaviso BA de tres ciclos lentos antes de apropiarse del bus; validarlo con las fases del 7501 y sus escrituras, sin copiar ciegamente la máquina de badlines del VIC-II.

Probar DEN, RSEL/CSEL y todos los scrolls cerca de los bordes; cambios de color, base o modo antes y después de cada fetch; escrituras a contadores `$1A-$1F`; freeze y reanudación. Distinguir captura de atributos, registro de desplazamiento y salida del borde. Un renderer que consulta registros vivos al final de la instrucción puede desplazar efectos varios píxeles aunque la imagen estática sea correcta.

## 5. Sonido

Referencias: preliminar PDF pp. 8-10 y 17-18; contraste de implementación VICE en sources. Dos generadores con frecuencia de 10 bits; solo voz 2 proporciona ruido. No añadir ADSR, filtro SID, anillo o sincronización SID.

- Voz 1: `$FF0E | (($FF12 & 3) << 8)`; voz 2: `$FF0F | (($FF10 & 3) << 8)`. Probar escrituras parciales preservando los campos gráficos de `$FF12`.
- `$FF11` selecciona fuentes y volumen compartido. Probar las ocho combinaciones de bits 4..6, especialmente tono 2 con ruido habilitado simultáneamente, sin asumir suma de tres voces independientes.
- Auditar volumen 0..15, saturación a partir de 8 y nivel DC. Separar valor programado, amplitud efectiva y filtro analógico de placa.
- El bit 7 es una función de reload/test aprovechable para salida digital: probar escritura repetida, mantenimiento a uno, flanco de bajada, fase inicial y semilla de ruido. Un simple reinicio por flanco puede omitir el efecto de mantenerlo activo.
- Modelar secuencia y reloj del generador de ruido de forma determinista; no usar azar del host como sustituto. Exigir evidencia de polinomio, dirección y semilla antes de declarar equivalencia bit a bit.
- El divisor de frecuencia exige contraste. La fórmula preliminar con `1024-N` discrepa de documentación y modelos posteriores; verificar período real para valores normales y casos `$3FE/$3FF`. No sustituir el contador por una fórmula que divide por cero ni fijar un caso especial sin medición o modelo independiente identificado.
- Aplicar cambios en su timestamp emulado, no en el siguiente bloque SDL. Medir estabilidad de afinación con CPU lento/rápido, DEN y DMA; conservar fase y fracciones de reloj entre bloques de audio. Probar también con audio del host silenciado.

Validar primero períodos/transiciones digitales; luego PCM a sample rate fijo y, por separado, respuesta analógica. Una grabación con otra cadena de filtrado no permite exigir igualdad de muestras.

## 6. Entrada y selección de memoria

El 6529 de teclado selecciona filas activas bajas; TED captura columnas al escribir `$FF08`. Mantener fila seleccionada, niveles de la matriz y captura como estados distintos. Cambiar tecla o fila después del strobe no debe cambiar la lectura hasta otro strobe. Consultar la guía 6529B para combinaciones y joystick; leer `$FF08` no debe devolver simplemente el último dato escrito.

Verificar que `$FF3E/$FF3F` son órdenes por dirección y que la ventana TED sigue accesible según decodificación aun al cambiar bancos. Comprobar `$FC00-$FCFF` y selectores `$FDD0-$FDDF` en los esquemas/ROM correspondientes; no extender una regla plana de overlay a toda la zona alta.

## 7. Matriz mínima de auditoría

| ID | Estímulo | Evidencia esperada |
|---|---|---|
| TED-R | barrido de registros con 00/FF/55/AA | máscaras y efectos por registro; huecos no convertidos en RAM |
| TED-T | cargar cada timer lento/rápido y durante DMA | misma base temporal; bajo detiene, alto inicia; T1 recarga, T2/T3 continúan |
| TED-I | dos fuentes, máscara y reconocimiento selectivo | flags conservados e IRQ común compuesta correctamente |
| TED-V | patrones de los cinco modos, PAL/NTSC y RAM 16/64K | dirección de cada fetch y fuente de color identificables |
| TED-D | escrituras alrededor de fetch/DMA/borde | timeline de bus y salida; CPU no avanza en fases robadas |
| TED-S | frecuencias normales/límite, ruido, DC, volumen 0..15 | períodos, fase y secuencia; frecuencia independiente de CPU/host |
| TED-K | fila/tecla, strobe, cambio de tecla sin strobe | lectura mantiene captura y luego se actualiza |
| TED-B | ROM CPU activa/inactiva y fuente gráfica independiente | vistas CPU/TED correctas y RAM subyacente preservada |

Por prueba guardar modelo/ROM/revisión, programa mínimo, origen del tiempo, traza y criterio de aceptación. Separar pruebas digitales exactas de límites analógicos y puntos todavía no resueltos.
