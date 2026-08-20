# Fuentes y evidencias de Nintendo Game Boy DMG

## Política de evidencia

La prioridad de los oráculos será:

1. Documentación original de Nintendo, Sharp o del titular del hardware.
2. Patentes originales que describan el circuito o la secuencia observada.
3. Trazas o ROMs de prueba ejecutadas sobre una revisión física identificada.
4. Cálculos directos y reproducibles a partir de los puntos anteriores.
5. Comparación diferencial con implementaciones independientes, solo como detector de discrepancias.

Una contradicción se conserva con revisión, fuente y experimento pendiente; no se resuelve por popularidad.

## Fuentes primarias

### NIN-GBPM-1999

- **Título:** Game Boy Programming Manual.
- **Documento:** `DMG-06-4216-001-A`, versión 1.0, 1999.
- **Autoridad:** Nintendo of America Inc.
- **URL:** https://files.nekoblog.org/uploads/pdf/39999184-GameBoy-Programming-Manual.pdf
- **Uso:** arquitectura, memoria, registros, CPU, interrupciones, LCD, sonido, Game Pak y comunicación.
- **Límite:** combina DMG y CGB y documenta principalmente la interfaz de programación. Un silencio no prueba comportamiento.

### PAT-US5134391

- **Título:** System for preventing the use of an unauthorized external memory.
- **Titular original:** Nintendo Co., Ltd.
- **URL:** https://patents.google.com/patent/US5134391A/en
- **Uso:** ROM interna `0x0000-0x00FF`, selección interna/externa, logo, checksum y entrega de control al Game Pak.
- **Límite:** cada detalle debe contrastarse con `DMG-CPU B`.

### PAT-US5095798

- **Título:** Electronic gaming device with pseudo-stereophonic sound generating capabilities.
- **Titular:** Nintendo Co., Ltd.
- **Inventores:** Satoru Okada e Hirokazu Tanaka.
- **URL:** https://patents.justia.com/patent/5095798
- **Uso:** fuentes de sonido, direccionamiento izquierda/derecha y selección estéreo/mono externa.
- **Límite:** no sustituye el ledger digital del APU.

### NIN-DMG-SERVICE

- **Título provisional:** Nintendo Game Boy Technical/Service Manual.
- **Autoridad declarada:** Nintendo World Class Service.
- **Estado:** candidato; antes de citar esquemas se verificará procedencia, edición y hash del escaneo.
- **Uso previsto:** placa, RAM, amplificador, LCD, conectores y señales del SoC.

## Evidencia experimental y diagnósticos

### MOONEYE

- **Repositorio:** https://github.com/Gekkio/mooneye-test-suite
- **Tipo:** ROMs diagnósticas con resultados contrastados en hardware identificado.
- **Relevancia:** incluye unidades DMG de revisiones 0, A, B y C.
- **Uso:** CPU, temporizadores, interrupciones, DMA, arranque y diferencias de revisión.
- **Estado:** fijar commit y licencia antes de automatizar.

### BLARGG

- **Repositorio de conservación:** https://github.com/retrio/gb-test-roms
- **Tipo:** ROMs diagnósticas con fuentes y salidas conocidas.
- **Uso:** instrucciones, temporización, memoria, interrupciones y sonido.
- **Estado:** registrar procedencia exacta de cada binario.

### DMG-ACID2

- **Repositorio:** https://github.com/mattcurrie/dmg-acid2
- **Tipo:** diagnóstico visual con fuente y referencia.
- **Uso:** tiles, ventana, sprites, prioridades, flips, tamaños y límite por línea.
- **Límite:** no es una prueba exhaustiva de timing del modo 3.

### Diagnósticos PPU avanzados

- **Estado:** candidato para Mealybug/SameSuite u otra suite con captura física.
- **Uso:** escrituras durante modos LCD, fetcher/FIFO y bordes STAT.
- **Condición:** fijar repositorio, commit, licencia, revisión y oráculo antes de volverlos obligatorios.

## Fuentes secundarias

Pan Docs, análisis de silicio, bases de placas y emuladores precisos pueden descubrir preguntas o experimentos. No reemplazan una fuente primaria ni una captura física. SameBoy, BGB, Gambatte u otros solo se usarán de forma diferencial.

## Política de ROM y medios

- La Boot ROM de Nintendo no se almacenará en el repositorio.
- Será aportada por el usuario y validada por tamaño y hash configurado.
- Un boot libre o sintético será una opción diferenciada.
- No se añadirán ROMs comerciales, partidas o volcados de usuario.
- Las ROMs diagnósticas requieren revisión de licencia y procedencia.
- Todo golden registrará productor, modelo, SoC, ROM, hash, entrada, captura y tolerancia.

## Metadatos mínimos de una captura física

- Modelo, placa y marcado del SoC.
- Boot ROM y ROM diagnóstica identificadas por hash.
- Estado inicial, reset y secuencia de entradas.
- Instrumento o método de captura.
- Dominio temporal y punto de muestreo.
- Datos en bruto, transformación y tolerancia.

## Estado al cerrar Hito 0

Las tres fuentes primarias y tres familias diagnósticas iniciales están identificadas. La descarga de binarios, commits y hashes pertenece al hito que los ejecute y no bloquea el ledger del SM83.

