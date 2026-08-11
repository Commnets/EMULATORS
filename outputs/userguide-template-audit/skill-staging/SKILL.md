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

## Actualizar campos y verificar

1. Actualizar todos los campos en Microsoft Word para regenerar índice, listas, referencias, `PAGE` y `NUMPAGES`. Si Word no está disponible, establecer `w:updateFields=true` y declarar la actualización diferida.
2. Ejecutar:

```powershell
& $PYTHON_BIN scripts/validate_document.py "C:\ruta\salida.docx"
```

3. Usar el skill `documents` para renderizar el DOCX y revisar todas las páginas al 100 %. Si LibreOffice no está disponible y Word sí, exportar a PDF con Word y rasterizar el PDF para la revisión.
4. Corregir y repetir la actualización de campos, validación estructural y revisión visual hasta que no haya marcadores, errores de campo, recortes, solapamientos ni páginas en blanco accidentales.
5. Entregar solo el DOCX final, salvo que el usuario pida también PDF u otros artefactos.

## Recursos

- `assets/emulators-documentation-template.docx`: plantilla canónica derivada de `docs/UserGuide.docx`.
- `scripts/start_document.py`: crea una copia y parametriza portada, metadatos, año y marca de agua.
- `scripts/validate_document.py`: comprueba estructura, campos, pie, marca de agua y marcadores pendientes.
- `references/template-contract.md`: contrato detallado de estructura, estilos y criterios de aceptación.
