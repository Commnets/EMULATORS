# DEBUG file documentation

This directory contains the structured sources and tools used to generate the
DOCX guides for the EMULATORS deep-debug format.

Files with a `.yaml` extension use JSON syntax, which is a subset of YAML 1.2.
This keeps them readable and versionable without adding a third-party generator
dependency.

## Maintained artefacts

| Emulator | Machine-specific source | Generated DOCX |
|---|---|---|
| Commodore 64 | `sources/c64.yaml` | `docs/C64Data/C64_DEBUG_File_Format.docx` |
| VIC-20 | `sources/vic20.yaml` | `docs/VIC20Data/VIC20_DEBUG_File_Format.docx` |
| C264 | `sources/c264.yaml` | `docs/C264Data/C264_DEBUG_File_Format.docx` |
| ZX80/ZX81 | `sources/zx80-zx81.yaml` | `docs/ZX81Data/ZX80_ZX81_DEBUG_File_Format.docx` |
| ZX Spectrum | `sources/zxspectrum.yaml` | `docs/ZXSpectrumData/ZXSpectrum_DEBUG_File_Format.docx` |
| MSX | `sources/msx.yaml` | `docs/MSXData/MSX_DEBUG_File_Format.docx` |

Each guide combines its machine-specific source with `sources/debug-common.yaml`. Every
DOCX is self-contained, while the shared material is maintained only once.

## Maintenance rule

Every change that adds, removes, renames, reorders, or modifies a DEBUG header,
field, event, unit, timing rule, or emission condition must update the relevant
common or machine-specific source and regenerate the affected DOCX files in the
same change.

This includes changes to `debug...` methods, calls to `writeCompleteLine`,
`writeLineData`, or `writeSimpleLine`, direct writes through `_deepDebugFile`,
propagation through `setDeepDebugFile`, and selection through
`activateDeepDebug`.

## Generation and validation

Use the Python executable provided by the Codex runtime:

```powershell
& $PYTHON_BIN .codex/documentation/debug-files/scripts/generate_debug_docs.py
& .codex/documentation/debug-files/scripts/update_debug_doc_fields.ps1
& $PYTHON_BIN .codex/documentation/debug-files/scripts/generate_debug_docs.py --finalize
& $PYTHON_BIN .codex/documentation/debug-files/scripts/check_debug_docs.py
```

The generator updates `state/generated-state.json`. Validation fails when a structured
source, a watched C++ file, or a generated DOCX changes. This check is
deliberately conservative: a non-documentation change inside a watched producer
also requires review and regeneration.

After generation, update fields in Microsoft Word and run the structural and
visual validation described by the `emulators-documentation-authoring` skill.
