# TPI 6523: contratos y pruebas

Consultar [sources.md](sources.md). Las pruebas son propuestas para futuras revisiones; no han sido ejecutadas contra el emulador.

## Identidad y registro

El documento aportado especifica tres puertos de ocho bits y seis registros. El acrónimo de fabricante es TPI. `COMMODORE::TIA` es el nombre local; no describe la TIA de Atari. Tampoco identifica el hardware UART: eso corresponde a ACIA.

| RS2..0 | Registro | Lectura | Escritura |
|---|---|---|---|
| 000 | PRA | nivel de pines A | latch de salida A |
| 001 | PRB | nivel de pines B | latch de salida B |
| 010 | PRC | nivel de pines C | latch de salida C |
| 011 | DDRA | dirección A | 0 entrada; 1 salida |
| 100 | DDRB | dirección B | 0 entrada; 1 salida |
| 101 | DDRC | dirección C | 0 entrada; 1 salida |
| 110,111 | estados ilegales | sin contrato documentado de dato | no son registros de control/IRQ del 6525 |

Cada PR conserva el último dato escrito incluso cuando DDR selecciona entrada. Al habilitar salida se aplica ese latch. La lectura observa pines, también si están configurados como salida; no es una lectura automática del latch. En un modelo sin contención, resolver las salidas con el latch y las entradas con el periférico es suficiente. Si se modela contención, documentar resolución eléctrica: el fabricante advierte que una salida sobrecargada puede leerse diferente. No aplicar automáticamente la AND de pull-up pasivo del 6529 al TPI.

Reset activo bajo borra los registros internos y deja todos los DDR a cero, con puertos flotantes. Escribir datos antes de habilitar DDR evita pulsos indeseados. No añadir timers, IRQ, handshake automático o pines CA/CB de VIA. Selección CS activa baja y R/W distinguen acceso; el bus de datos queda liberado al deseleccionar. Las temporizaciones son de CS/R/W/RS; no inventar un reloj interno que avance un protocolo en cada `simulate()`.

Referencia: PDF 6523 pp. 1, 6-7; tiempos pp. 4-5. Para offsets 6/7 distinguir ausencia de especificación, política del emulador y resultado medido; no imponer $FF como hecho de silicio.

## Integración 1551

El esquema Commodore 251925 rev. 2 conserva un `6523A-28`, con PA0..7 para datos, PB0/PB1 para STATUS0/STATUS1 y PC6/PC7 para ACK/DAV. No confundir el encapsulado reducido con los 24 pines accesibles del 6523 de 40 pines. Conservar registros, bits conectados y pines no disponibles como conceptos distintos. El sufijo de velocidad A no explica por sí solo el encapsulado -28.

En ese dibujo la entrada RST del TPI aparece unida a +5 V; BRESET participa en otra parte de la interfaz. Verificar la revisión y la inicialización de firmware antes de exigir que un reset del ordenador borre los DDR del TPI externo. El reset genérico del chip solo describe lo que sucede al activar realmente su pin.

Las ventanas usuales del lado host son `$FEE0-$FEFF` para unidad 8 y `$FEC0-$FEDF` para unidad 9, con selección interna de ocho offsets y repeticiones dentro de la ventana de 32 bytes. Este mapeo depende del decodificador externo y selección de dispositivo: contrastarlo con firmware/PLA o una medición antes de fijarlo como oráculo. El esquema muestra el decodificador 8700-011 y la conexión A0..A2 al TPI; no contiene por sí solo su tabla lógica completa.

Probar interfaz ausente, una interfaz como 8, otra como 9 y coexistencia. Dos unidades requieren estados independientes. Un espejo entre ambas ventanas es una decisión local a revisar, no evidencia de cableado real. Un acceso al espacio de expansión no debe esconder RAM/ROM fuera de su selección real. Tampoco debe atribuir al TPI funciones de KERNAL, CBM DOS o geometría de disco.

Reconstruir con polaridad explícita el intercambio de datos, STATUS, DAV y ACK en ambos sentidos. Una transición solo se consume una vez, incluso si la función de resolución de pines se llama varias veces. El host debe poder esperar al dispositivo; no declarar una transferencia terminada al escribir PRA. Mantener datos estables durante la ventana de handshake. Documentar qué lado conduce cada línea y cuándo la libera.

En la placa de la unidad 1551 el esquema oficial identifica un 6525A; algunos dibujos reconstruidos lo rotulan 6523. El índice de Zimmers señala la discrepancia. No modelar todo el conjunto como dos 6523 idénticos ni trasladar los registros 6/7 del 6525 al TPI del cartucho.

## Matriz de validación

| ID | Secuencia | Criterio |
|---|---|---|
| TPI-1 | reset real, leer DDR, observar pines | tres DDR a 0; latches borrados; sin conducción de salida |
| TPI-2 | DDR=0; PR=$A5; DDR=$FF | dato retenido y aplicado al habilitar salida |
| TPI-3 | DDR=$55; variar entradas externas | bits de entrada y salida independientes; lectura de pines |
| TPI-4 | cambiar un puerto o DDR | ningún efecto en los otros puertos |
| TPI-5 | deseleccionar CS y alternar RS/RW | sin escrituras espurias ni conducción del bus de datos |
| TPI-6 | acceder offsets 6/7 y alias | estados ilegales no activan funciones de 6525; aliases dependen de A0..2 y CS |
| TPI-7 | configurar 8/9; escribir distintos patrones | ventanas y estados independientes si hay dos interfaces |
| TPI-8 | desconectar 1551; acceder ventanas | conducta del bus/expansión ausente, sin TPI ficticio |
| TPI-9 | retardar ACK/DAV y repetir lectura | sin bytes duplicados, perdidos ni anticipados |
| TPI-10 | reset host frente a reset de pin y power-on | conducta según conexión física y firmware, no reset genérico indiscriminado |

Guardar trazas de RS/CS/RW, DDR, PR, PA/PB/PC y línea remota con timestamp emulado. El correcto LOAD no basta para demostrar tiempos o lectura de pines; una implementación por traps puede tener compatibilidad funcional sin fidelidad del bus.
