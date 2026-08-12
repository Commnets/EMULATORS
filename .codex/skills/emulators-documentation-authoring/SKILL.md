---
name: emulators-documentation-authoring
description: Crear, ampliar o actualizar documentación técnica de EMULATORS en formato DOCX usando la plantilla oficial del repositorio, con portada parametrizada, índice, prólogo, capítulos de contenido, anexos, listas de ilustraciones y ecuaciones, marca de agua del autor y pie Página X | Y. Usar cuando Codex deba documentar subsistemas, arquitecturas, dispositivos, modelos, herramientas, compilación, uso, pruebas o cualquier otro aspecto del emulador y el resultado deba conservar la identidad visual de docs/UserGuide.docx.
---

# Documentación de EMULATORS

Crear documentos DOCX técnicamente rigurosos a partir de la plantilla incluida. Mantener `assets/emulators-documentation-template.docx` como autoridad visual y no modificarla directamente.

## Preparar el trabajo

1. Leer [references/template-contract.md](references/template-contract.md) por completo.
2. Cargar el runtime de dependencias del workspace y usar su Python para todo el trabajo DOCX.
3. Reunir título, subtítulo, autor, año, audiencia, alcance y fuentes. Inferir título y subtítulo cuando la petición los haga evidentes. Usar el año natural actual y `Ignacio Cea Forniés` como autor predeterminado salvo indicación contraria.
4. Preguntar únicamente cuando una decisión ausente cambie de forma material el alcance, la autoría, el nivel técnico o la organización del documento.
5. Inspeccionar el código, pruebas y documentación del repositorio que sostengan cada afirmación. Activar además el skill de auditoría específico del sistema o dispositivo cuando exista.

## Crear el documento base

Ejecutar `scripts/start_document.py` sobre el asset canónico:

```powershell
& $PYTHON_BIN scripts/start_document.py `
  --title "Título" `
  --subtitle "Subtítulo" `
  --author "Nombre del autor" `
  --year 2026 `
  --out "C:\ruta\salida.docx"
```

No escribir sobre el asset ni sobre `docs/UserGuide.docx`. Trabajar siempre en la copia generada.

## Redactar y maquetar

1. Sustituir los marcadores del prólogo, del primer bloque de contenido y de los anexos. Eliminar todo marcador no utilizado.
2. Crear tantos bloques de contenido como requiera el tema, entre Prólogo y Anexos.
3. Aplicar estilos Word reales: `Title` para capítulos principales y `Heading 1` a `Heading 3` para su jerarquía interna. No simular títulos con formato directo.
4. Mantener A4, márgenes, tipografía, colores, portada, marca de agua y pie del asset. No reconstruir la portada ni los encabezados.
5. Añadir ilustraciones y ecuaciones cerca de su primera explicación. Usar leyendas Word con las etiquetas exactas `Ilustración` y `Ecuación`, campos `SEQ` y referencias `REF` cuando proceda.
6. Mantener Anexos, Ilustraciones y Ecuaciones al final y en ese orden. Conservar sus campos de lista aunque una lista quede vacía.
7. Evitar afirmaciones no verificadas. Distinguir claramente comportamiento implementado, comportamiento previsto, limitaciones y aproximaciones de emulación.
8. Localizar los rótulos fijos de estructura, leyendas y pie al idioma solicitado para el documento, conservando los campos, estilos, posiciones y orden de la plantilla. Por ejemplo, en inglés usar `Contents`, `Foreword`, `Appendices`, `Illustrations`, `Equations` y `Page X | Y`.
9. Separar visualmente las entradas repetitivas de referencia. En guías de comandos, parámetros u otros catálogos, aplicar al encabezado de cada entrada al menos 14 pt de espacio anterior, sin insertar párrafos vacíos.
10. Impedir títulos huérfanos. Aplicar `keep with next` (`w:keepNext`) a `Title`, `Heading 1`, `Heading 2`, `Heading 3` y a cualquier otro estilo de título utilizado, de modo que ningún título termine una página sin al menos el primer párrafo que le sigue. Si hay varios títulos consecutivos, mantener unida toda la cadena hasta el primer párrafo de contenido.

## Mantener guías ligadas al código

- Tratar `docs/C64Emulator_UserGuide.docx`, `docs/VIC20Data/VIC20Emulator_UserGuide.docx`, `docs/C264Data/C264Emulator_UserGuide.docx`, `docs/ZX81Data/ZX81Emulator_UserGuide.docx`, `docs/ZXSpectrumData/ZXSpectrumEmulator_UserGuide.docx` y `docs/MSXData/MSXEmulator_UserGuide.docx` como artefactos mantenidos. Todo cambio que añada, elimine, renombre o altere un parámetro de arranque, un comando, su sintaxis, parámetros, consecuencias o salida visible del sistema correspondiente debe revisar y, cuando corresponda, actualizar su guía en el mismo cambio.
- Para parámetros de arranque, recorrer el punto de entrada del ejecutable y toda la jerarquía de la clase `Emulator`; no confiar únicamente en el texto breve de ayuda de `main`.
- Para comandos, auditar `LocalConsole` y toda la cadena de responsabilidad específica. En Commodore: `C64::CommandBuilder`, `VIC20::CommandBuilder` o `C264::CommandBuilder` -> `COMMODORE::CommandBuilder` -> `MCHEmul::StandardCommandBuilder`. En ZX80/ZX81: `ZX81::CommandBuilder` -> `SINCLAIR::CommandBuilder` -> `MCHEmul::StandardCommandBuilder`. En ZX Spectrum: `ZXSPECTRUM::CommandBuilder` -> `SINCLAIR::CommandBuilder` -> `MCHEmul::StandardCommandBuilder`. En MSX, auditar la cadena efectiva construida por los puntos de entrada: `MSX::CommandBuilder` -> `MCHEmul::StandardCommandBuilder` -> `MCHEmul::StandardCommandBuilder`; la duplicación actual no añade órdenes distintas, pero forma parte del código que debe revisarse. Indicar expresamente qué órdenes son exclusivas de la consola local y cuáles admite el canal remoto de la versión sin consola.
- Si cambia el contrato `InfoStructure` o la salida de un comando, mantener también los `.fmt` canónicos bajo `projects` mediante `emulators-fmt-audit`.

## Actualizar campos y verificar

1. Actualizar todos los campos en Microsoft Word para regenerar índice, listas, referencias, `PAGE` y `NUMPAGES`. Si Word no está disponible, establecer `w:updateFields=true` y declarar la actualización diferida.
2. Ejecutar:

```powershell
& $PYTHON_BIN scripts/validate_document.py "C:\ruta\salida.docx"
```

3. Usar el skill `documents` para renderizar el DOCX y revisar todas las páginas al 100 %. Si LibreOffice no está disponible y Word sí, exportar a PDF con Word y rasterizar el PDF para la revisión.
4. Corregir y repetir la actualización de campos, validación estructural y revisión visual hasta que no haya marcadores, errores de campo, recortes, solapamientos ni páginas en blanco accidentales.
5. Durante la revisión visual, comprobar expresamente que todos los títulos conservan debajo al menos su primer párrafo y que los encabezados de entradas repetitivas mantienen la separación anterior establecida.
6. Entregar solo el DOCX final, salvo que el usuario pida también PDF u otros artefactos.

## Recursos

- `assets/emulators-documentation-template.docx`: plantilla canónica derivada de `docs/UserGuide.docx`.
- `scripts/start_document.py`: crea una copia y parametriza portada, metadatos, año y marca de agua.
- `scripts/validate_document.py`: comprueba estructura, campos, pie, marca de agua y marcadores pendientes.
- `references/template-contract.md`: contrato detallado de estructura, estilos y criterios de aceptación.
