#!/usr/bin/env python3
"""Validate the required EMULATORS documentation structure in a DOCX package."""

from __future__ import annotations

import argparse
from pathlib import Path
import re
import zipfile

from lxml import etree


W = "http://schemas.openxmlformats.org/wordprocessingml/2006/main"
V = "urn:schemas-microsoft-com:vml"
NS = {"w": W, "v": V}


def xml(zip_file: zipfile.ZipFile, name: str) -> etree._Element:
    return etree.fromstring(zip_file.read(name))


def joined_text(root: etree._Element) -> str:
    return " ".join(text for text in root.xpath(".//w:t/text()", namespaces=NS) if text)


def validate(path: Path, allow_placeholders: bool) -> list[str]:
    errors: list[str] = []
    with zipfile.ZipFile(path) as package:
        names = set(package.namelist())
        required_parts = {"word/document.xml", "word/footer1.xml", "word/settings.xml"}
        for missing in sorted(required_parts - names):
            errors.append(f"missing package part: {missing}")

        document = xml(package, "word/document.xml")
        text = joined_text(document)
        positions = []
        paragraphs = document.xpath(".//w:body/w:p", namespaces=NS)
        required_blocks = (
            ("Índice", "Contents"),
            ("Prólogo", "Foreword"),
            ("Anexos", "Appendices"),
            ("Ilustraciones", "Illustrations"),
            ("Ecuaciones", "Equations"),
        )
        for alternatives in required_blocks:
            position = next((index for index, paragraph in enumerate(paragraphs)
                             if joined_text(paragraph).strip() in alternatives), -1)
            if position < 0:
                errors.append(f"missing required block: {' or '.join(alternatives)}")
            positions.append(position)
        if all(position >= 0 for position in positions) and positions != sorted(positions):
            errors.append("required blocks are not in the expected order")

        instructions = " ".join(document.xpath(".//w:instrText/text()", namespaces=NS))
        if not re.search(r"\bTOC\b", instructions):
            errors.append("missing table-of-contents field")
        if '\\c "Ilustración"' not in instructions and '\\c "Illustration"' not in instructions:
            errors.append("missing illustration-list field")
        if '\\c "Ecuación"' not in instructions and '\\c "Equation"' not in instructions:
            errors.append("missing equation-list field")

        placeholder_markers = (
            "[Escriba aquí", "[Título del bloque", "[Desarrolle aquí",
            "[Añada aquí", "[Write the", "[Content block title",
            "[Develop the", "[Add appendices",
        )
        if not allow_placeholders and any(marker in text for marker in placeholder_markers):
            errors.append("unresolved template placeholder remains")

        footer_text = ""
        footer_fields = ""
        for name in sorted(n for n in names if re.fullmatch(r"word/footer\d+\.xml", n)):
            footer = xml(package, name)
            footer_text += " " + joined_text(footer)
            footer_fields += " " + " ".join(footer.xpath(".//w:instrText/text()", namespaces=NS))
        if not any(label in footer_text for label in ("Página", "Page")) or not re.search(r"\bPAGE\b", footer_fields) or not re.search(r"\bNUMPAGES\b", footer_fields):
            errors.append("footer does not contain a localized page label plus PAGE/NUMPAGES fields")

        watermark_count = 0
        for name in sorted(n for n in names if re.fullmatch(r"word/header\d+\.xml", n)):
            header = xml(package, name)
            watermark_count += len(header.xpath(".//v:textpath[@string]", namespaces=NS))
        if watermark_count == 0:
            errors.append("author watermark was not found in headers")

        settings = xml(package, "word/settings.xml")
        if not settings.xpath('.//w:updateFields[@w:val="true" or @w:val="1"]', namespaces=NS):
            errors.append("w:updateFields is not enabled")
    return errors


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("document", type=Path)
    parser.add_argument("--allow-placeholders", action="store_true")
    args = parser.parse_args()
    if not args.document.is_file():
        raise SystemExit(f"Document not found: {args.document}")
    errors = validate(args.document, args.allow_placeholders)
    if errors:
        for error in errors:
            print(f"ERROR: {error}")
        raise SystemExit(1)
    print(f"OK: {args.document.resolve()}")


if __name__ == "__main__":
    main()
