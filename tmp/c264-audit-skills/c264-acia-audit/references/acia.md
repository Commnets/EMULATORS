# ACIA: contrato de registros, UART y placa

Fuentes: [sources.md](sources.md). Pruebas propuestas, sin ejecución todavía. PDF aportado: diciembre de 1980, 8 páginas; pp. 6-8 contienen el contrato de programación. Las notas SY6551 amplían la operación; cualquier diferencia de variante debe conservarse explícita.

## Registros y alias

La placa Plus/4 decodifica `$FD00-$FD0F`; A0/A1 seleccionan cuatro offsets repetidos. No hay 16 registros independientes. Probar extremos de ventana y direcciones vecinas. En C16/C116 no hay ACIA de base: diferenciar no implementado de no aplicable; no inventar un puerto a partir de RAM subyacente o de una constante de lectura.

| Offset | Lectura CPU | Escritura CPU |
|---|---|---|
| 0 | Receiver Data Register (RDR) | Transmit Data Register (TDR) |
| 1 | Status, con reconocimiento IRQ | Programmed Reset; dato indiferente |
| 2 | Command | Command |
| 3 | Control | Control |

TDR/RDR son buffers distintos; ni siquiera comparten función aunque compartan dirección. Mantener además dos shift registers. Una escritura TDR no hace que el byte aparezca inmediatamente en RDR. La CPU lee la instantánea de Status antes de aplicar su reconocimiento; un peek no reconoce nada. No hacer read-modify-write sobre Status como si fuera RAM: su escritura resetea parcialmente la ACIA.

## Status

| Bit | Significado |
|---|---|
| 0 | Parity Error |
| 1 | Framing Error |
| 2 | Overrun |
| 3 | RDR Full |
| 4 | TDR Empty |
| 5 | nivel de /DCD en el chip: 0 indica portadora |
| 6 | nivel de /DSR en el chip: 0 indica preparado |
| 7 | IRQ pendiente, 1 indica solicitud, salida física activa baja |

Los tres errores describen la recepción y no generan por sí solos una interrupción separada. Leer RDR libera su condición de lleno y reconoce los errores asociados conforme a la variante; leer Status reconoce IRQ y no equivale a consumir el byte. La lectura de Status puede ser seguida de otra solicitud por una condición nueva; no modelar simplemente `IRQ = RDRF || TDRE` en todo momento.

TDRE señala que el buffer TDR puede recibir otro dato, no que terminó el último stop bit de la línea. Verificar por separado el vaciado hacia el transmisor, el comportamiento con CTS y el final de trama. No establecerlo permanentemente a uno ni importar el fallo conocido de transmisión de otra familia CMOS.

Los cambios de DCD/DSR pueden generar IRQ y tienen retención de estado/evento descrita en las notas de operación. Probar varias transiciones antes y después de leer Status. No borrar la interrupción TED al reconocer ACIA ni borrar ACIA al reconocer TED. La entrada IRQ común del 7501 conserva cualquier fuente aún activa.

## Command y Control

| Command | Función |
|---|---|
| bit 0 | 0 deshabilita RX/TX, /DTR alto; 1 habilita, /DTR bajo |
| bit 1 | inhibición IRQ de recepción: 0 habilita; 1 deshabilita |
| bits 3..2 | 00: TX IRQ deshabilitada, /RTS alto; 01: TX IRQ habilitada, /RTS bajo; 10: TX IRQ deshabilitada, /RTS bajo; 11: break, /RTS bajo |
| bit 4 | echo frente a recepción normal |
| bits 7..5 | bit 5=0 sin paridad; 001 impar, 011 par, 101 mark, 111 space |

Los códigos de paridad se muestran en orden bits 7,6,5. Mark/space transmiten paridad fija y deshabilitan su comprobación según el preliminar. No confundir bit 1 con una máscara global de todos los tipos de IRQ.

| Control | Función |
|---|---|
| bits 3..0 | divisor/selección del baud rate |
| bit 4 | 0 reloj RX externo; 1 generador interno |
| bits 6..5 | 00=8, 01=7, 10=6, 11=5 bits de datos |
| bit 7 | 0 un stop; 1 normalmente dos, con excepciones de trama |

Con bit 7=1, 8 bits más paridad usa un stop; 5 bits sin paridad usa 1,5 stops. No simplificar a una elección universal de uno/dos. Los datos se transmiten LSB primero, start bajo, stop/idle alto. RX elimina paridad y pone a cero los bits altos no usados; no hay FIFO ni nueve bits de datos de CPU por la mención a transmisión de nueve bits (8+paridad) del folleto.

## Reloj y tramas

El cristal ACIA es 1,8432 MHz y es independiente del reloj variable TED/CPU. Los códigos 1..15 seleccionan nominalmente 50,75,109,92,134,58,150,300,600,1200,1800,2400,3600,4800,7200,9600,19200 baud. Código 0 selecciona reloj externo x16, no desactiva la UART. Los divisores exactos y redondeos de las velocidades especiales se contrastan con la variante; no derivarlos de la etiqueta decimal redondeada.

En RX externo, un bit requiere 16 períodos del reloj RxC. Identificar dirección RxC al usar reloj interno. Mantener acumulación fraccional y timestamps al convertir entre dominios. El mismo número de ciclos CPU en modo lento y rápido no representa el mismo tiempo de transmisión. El avance de la línea no depende del polling CPU ni del siguiente tick del sistema operativo host.

Validar rechazo de falso start, punto de muestreo, paridad, stop inválido, break y recepción solapada con transmisión. Las notas SY6551 sitúan la notificación RX alrededor de 9/16 del stop; usar ese dato como referencia de esa variante y medirlo si se exige exactitud para otro silicio. En overrun, comprobar qué dato conserva RDR; no elegir sobrescribir o descartar para ajustarse al backend.

## Reset y efectos temporales

El PDF preliminar figura 7 da Command=$02 tras reset hardware y conserva bits 7..5 tras reset por software, fijando bits 4..0 a $02; Control se borra por hardware y se conserva por software. Status figura 8 indica TDRE=1 y errores/RDRF/IRQ=0 en reset hardware, con DCD/DSR dependientes de entradas; reset programado marca específicamente el borrado de Overrun.

Esto es evidencia de una edición preliminar, no prueba de cada 6551 de producción. Por ejemplo, el núcleo MAME consultado inicializa Command a cero. Mantener el desacuerdo documentado y comprobar el chip objetivo antes de cambiar el código. Un reset programado no puede implementarse como borrar toda la estructura de estado.

Las notas SY6551 describen casos en que una IRQ RX/TX pendiente sobrevive al reset programado hasta su servicio, mientras una de DCD/DSR se libera. Es una prueba de borde que necesita identificación de variante, no una razón para imponer un reset global de flags. Diferenciar encendido, pin /RES, escritura a offset 1 y programación del KERNAL.

## Conexión del Plus/4: no es un módem genérico directo

Fuente decisiva: esquema 310164 hoja 3, PDF de servicio p. 15, impresa 9.

- /CTS de ACIA pin 9 está a masa. CTS del conector K se lee por P1 del 6529B de user port. No conectar automáticamente K al pin /CTS de la ACIA ni bloquear TX por ese nivel externo.
- /DTR y /RTS pasan por inversores hacia E y D. DCD y DSR llegan desde H y L a través de inversores. Conservar nivel físico del conector y nivel activo bajo del chip por separado.
- TxD va a M, RxD a C y RxC a 8; la IRQ de U3 comparte la línea CPU con TED/expansión según placa.
- El user port usa niveles de placa TTL; la adaptación eléctrica RS-232 pertenece al periférico. No aplicar inversiones adicionales de un transceptor que no existe en el modelo.

Un backend de sockets/terminal debe declarar si reproduce bits y handshaking o solo transporta bytes. En ambos casos, no debe fabricar presencia de portadora, entrega instantánea o reset de registros para ocultar una desconexión. Si se usan valores de entrada por defecto, justificar con resistencias/inversores de la placa y presencia del periférico.

## Pruebas propuestas

| ID | Estímulo | Resultado a comprobar |
|---|---|---|
| ACIA-1 | acceder cuatro offsets y sus alias | separación RDR/TDR, Status/reset, Command y Control |
| ACIA-2 | hardware reset y program reset con registros no nulos | máscaras distintas y pines correctos para la variante |
| ACIA-3 | 8N1, 7E1, 5N1,5 y casos bit 7 | bits LSB, paridad y duración exacta de trama |
| ACIA-4 | dos bytes contiguos y TX/RX simultáneos | buffers/shift separados; TDRE antes del fin de stop |
| ACIA-5 | RX sin leer, error de paridad y stop bajo | RDRF/Overrun/errores y dato conservado, con reconocimiento correcto |
| ACIA-6 | IRQ pendiente; leer Status sin leer RDR | instantánea y reconocimiento IRQ sin consumir datos |
| ACIA-7 | DCD/DSR cambian dos veces; habilitar/deshabilitar | retención, nuevas IRQ y estado según variante |
| ACIA-8 | CTS en K cambia durante TX | cambia 6529 P1; /CTS interno sigue bajo en esta placa |
| ACIA-9 | cambiar velocidad CPU/DMA y baudrate | tiempo serie determinado por cristal y divisores |
| ACIA-10 | ACIA y TED solicitan a la vez | reconocer una conserva IRQ de la otra |
| ACIA-11 | break, echo, falso start, reloj externo detenido | secuencia de bits y ausencia de progreso sin reloj |
| ACIA-12 | C16/C116 sin expansión; Plus/4 sin módem | ausencia de chip frente a entradas de chip presente |

Guardar timeline TxD/RxD/RxC, Status, buffers y fuentes IRQ. Las pruebas de tramas requieren backend determinista; una prueba de eco del host valida menos que una captura de bits.
