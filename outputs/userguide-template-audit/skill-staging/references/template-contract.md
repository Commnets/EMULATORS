# Contrato de la plantilla de documentación

## Fuente y autoridad

Usar `../assets/emulators-documentation-template.docx` como fuente visual canónica. La plantilla deriva de `docs/UserGuide.docx`, pero elimina los resultados antiguos y rotos de la lista de ecuaciones e incorpora un bloque de contenido reutilizable. No editar ninguno de esos dos archivos durante la creación de un documento.

## Orden obligatorio

1. Portada.
2. Índice.
3. Prólogo.
4. Uno o más capítulos de contenido.
5. Anexos.
6. Ilustraciones.
7. Ecuaciones.

Permitir cualquier número de capítulos y subcapítulos dentro del bloque de contenido. Mantener Anexos aunque esté vacío; indicar brevemente que no hay anexos o dejar únicamente su título después de eliminar el marcador.

## Portada

- Mostrar el año en la esquina superior derecha.
- Mostrar título, subtítulo y autor en la zona inferior izquierda del bloque central.
- Conservar los colores, tamaños, posiciones y formas originales.
- No mostrar número de página en la portada.
- Tratar título, subtítulo, autor y año como valores variables de cada documento.

## Páginas interiores

- Mantener tamaño A4 vertical y márgenes aproximados de 3 cm a izquierda/derecha y 2,5 cm arriba/abajo.
- Mostrar el nombre del autor en mayúsculas como marca de agua diagonal gris y semitransparente.
- Mostrar en la esquina inferior derecha `Página X | Y` mediante campos `PAGE` y `NUMPAGES`.
- No sustituir campos por números escritos a mano.

## Jerarquía y navegación

- Aplicar `Title` a Prólogo, cada capítulo principal y Anexos.
- Aplicar `Heading 1`, `Heading 2` y `Heading 3` a niveles internos.
- Mantener el índice como campo Word y actualizarlo al final.
- No incluir texto de marcador en el documento entregado.

## Ilustraciones y ecuaciones

- Usar `Ilustración` como etiqueta de leyenda de figuras, diagramas, capturas y gráficos.
- Usar `Ecuación` como etiqueta de leyenda de ecuaciones que deban aparecer en su lista.
- Crear numeración con campos `SEQ`, no con números manuales.
- Usar referencias internas `REF` cuando el texto remita a una ilustración o ecuación.
- Mantener las dos listas finales como campos Word. Una lista vacía puede mostrar el mensaje normal de Word tras actualizar campos; nunca debe conservar entradas de otro documento ni errores de marcador.

## Contenido técnico

- Basar el texto en código, pruebas, documentación local y fuentes primarias que el usuario proporcione o autorice consultar.
- Explicar primero el propósito y después la arquitectura, flujo, interfaces, configuración, uso, límites y verificación que resulten pertinentes.
- Identificar versiones, modelos o plataformas cuando el comportamiento difiera.
- Separar hechos verificados de inferencias y de trabajo pendiente.
- Preferir diagramas y tablas solo cuando aclaren relaciones que serían difíciles de seguir en prosa.

## Criterios de aceptación

- Portada completa y legible.
- Orden de bloques correcto.
- Índice, listas, referencias y paginación actualizados.
- Marca de agua igual al autor de portada.
- Pie correcto en todas las páginas interiores y ausente en portada.
- Sin marcadores entre corchetes, errores de campo, texto recortado, solapamientos o saltos de página accidentales.
- Todas las páginas revisadas visualmente después de la última modificación.
