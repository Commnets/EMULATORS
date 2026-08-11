from __future__ import annotations

import argparse
from pathlib import Path

from docx import Document
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.shared import Pt


HEADING_STYLES = ("Title", "Heading 1", "Heading 2", "Heading 3")


def set_update_fields(document: Document) -> None:
    settings = document.settings.element
    update = settings.find(qn("w:updateFields"))
    if update is None:
        update = OxmlElement("w:updateFields")
        settings.append(update)
    update.set(qn("w:val"), "true")


def update_document(source: Path, destination: Path) -> dict[str, int]:
    document = Document(source)

    for style_name in HEADING_STYLES:
        style = document.styles[style_name]
        style.paragraph_format.keep_with_next = True

    # Heading 2 is the entry heading used by the C64 command and parameter
    # catalogues. Fourteen points gives each entry a clear visual start.
    document.styles["Heading 2"].paragraph_format.space_before = Pt(14)

    heading_counts = {name: 0 for name in HEADING_STYLES}
    for paragraph in document.paragraphs:
        for run in paragraph.runs:
            if "C64Emulator_UserGuider.docx" in run.text:
                run.text = run.text.replace(
                    "C64Emulator_UserGuider.docx",
                    "C64Emulator_UserGuide.docx",
                )
        style_name = paragraph.style.name if paragraph.style is not None else ""
        if style_name in heading_counts:
            paragraph.paragraph_format.keep_with_next = True
            heading_counts[style_name] += 1
            if style_name == "Heading 2":
                paragraph.paragraph_format.space_before = Pt(14)

    set_update_fields(document)
    destination.parent.mkdir(parents=True, exist_ok=True)
    document.save(destination)
    return heading_counts


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=Path)
    parser.add_argument("destination", type=Path)
    args = parser.parse_args()
    counts = update_document(args.source, args.destination)
    print("; ".join(f"{name}={count}" for name, count in counts.items()))


if __name__ == "__main__":
    main()
