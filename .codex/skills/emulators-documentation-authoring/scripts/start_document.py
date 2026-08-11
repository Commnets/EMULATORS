#!/usr/bin/env python3
"""Create a parametrized working DOCX from the canonical EMULATORS template."""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
from pathlib import Path
import re
import tempfile
import zipfile

from lxml import etree


W = "http://schemas.openxmlformats.org/wordprocessingml/2006/main"
V = "urn:schemas-microsoft-com:vml"
DC = "http://purl.org/dc/elements/1.1/"
CP = "http://schemas.openxmlformats.org/package/2006/metadata/core-properties"
DCTERMS = "http://purl.org/dc/terms/"
XSI = "http://www.w3.org/2001/XMLSchema-instance"
COVER = "http://schemas.microsoft.com/office/2006/coverPageProps"
NS = {"w": W, "v": V, "dc": DC, "cp": CP, "dcterms": DCTERMS}


def set_sdt_text(root: etree._Element, alias: str, value: str) -> None:
    matches = root.xpath(f'.//w:sdt[w:sdtPr/w:alias[@w:val="{alias}"]]', namespaces=NS)
    if not matches:
        raise RuntimeError(f"Missing cover content control: {alias}")
    for control in matches:
        texts = control.xpath(".//w:sdtContent//w:t", namespaces=NS)
        if not texts:
            raise RuntimeError(f"Content control has no text node: {alias}")
        texts[0].text = value
        for extra in texts[1:]:
            extra.text = ""


def patch_document(data: bytes, title: str, subtitle: str, author: str, year: int) -> bytes:
    root = etree.fromstring(data)
    set_sdt_text(root, "Título", title)
    set_sdt_text(root, "Subtítulo", subtitle)
    set_sdt_text(root, "Autor", author)
    set_sdt_text(root, "Año", str(year))
    for date in root.xpath('.//w:sdt[w:sdtPr/w:alias[@w:val="Año"]]/w:sdtPr/w:date', namespaces=NS):
        date.set(f"{{{W}}}fullDate", f"{year}-01-01T00:00:00Z")
    return etree.tostring(root, xml_declaration=True, encoding="UTF-8", standalone=True)


def patch_header(data: bytes, author: str) -> bytes:
    root = etree.fromstring(data)
    paths = root.xpath(".//v:textpath", namespaces=NS)
    if not paths:
        return data
    for path in paths:
        path.set("string", author.upper())
    return etree.tostring(root, xml_declaration=True, encoding="UTF-8", standalone=True)


def patch_core(data: bytes, title: str, subtitle: str, author: str) -> bytes:
    root = etree.fromstring(data)
    values = {
        f"{{{DC}}}title": title,
        f"{{{DC}}}subject": subtitle,
        f"{{{DC}}}creator": author,
        f"{{{CP}}}lastModifiedBy": author,
    }
    for tag, value in values.items():
        node = root.find(tag)
        if node is None:
            node = etree.SubElement(root, tag)
        node.text = value
    modified = root.find(f"{{{DCTERMS}}}modified")
    if modified is not None:
        modified.text = datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")
        modified.set(f"{{{XSI}}}type", "dcterms:W3CDTF")
    return etree.tostring(root, xml_declaration=True, encoding="UTF-8", standalone=True)


def patch_cover_properties(data: bytes, year: int) -> bytes:
    root = etree.fromstring(data)
    publish = root.find(f"{{{COVER}}}PublishDate")
    if publish is None:
        publish = etree.SubElement(root, f"{{{COVER}}}PublishDate")
    publish.text = f"{year}-01-01T00:00:00"
    return etree.tostring(root, xml_declaration=True, encoding="UTF-8", standalone=True)


def patch_settings(data: bytes) -> bytes:
    root = etree.fromstring(data)
    update = root.find(f"{{{W}}}updateFields")
    if update is None:
        update = etree.SubElement(root, f"{{{W}}}updateFields")
    update.set(f"{{{W}}}val", "true")
    return etree.tostring(root, xml_declaration=True, encoding="UTF-8", standalone=True)


def build(template: Path, output: Path, title: str, subtitle: str, author: str, year: int, force: bool) -> None:
    if not template.is_file():
        raise FileNotFoundError(template)
    if template.resolve() == output.resolve():
        raise ValueError("Output must differ from the canonical template")
    if output.exists() and not force:
        raise FileExistsError(f"Output already exists: {output}; pass --force to replace it")
    if not re.fullmatch(r"\d{4}", str(year)):
        raise ValueError("Year must contain four digits")

    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(suffix=".docx", delete=False, dir=output.parent) as handle:
        temporary = Path(handle.name)
    try:
        with zipfile.ZipFile(template, "r") as source, zipfile.ZipFile(temporary, "w") as target:
            for info in source.infolist():
                data = source.read(info.filename)
                if info.filename == "word/document.xml":
                    data = patch_document(data, title, subtitle, author, year)
                elif re.fullmatch(r"word/header\d+\.xml", info.filename):
                    data = patch_header(data, author)
                elif info.filename == "docProps/core.xml":
                    data = patch_core(data, title, subtitle, author)
                elif info.filename == "customXml/item1.xml":
                    data = patch_cover_properties(data, year)
                elif info.filename == "word/settings.xml":
                    data = patch_settings(data)
                target.writestr(info, data)
        temporary.replace(output)
    finally:
        if temporary.exists():
            temporary.unlink()


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    default_template = Path(__file__).resolve().parent.parent / "assets" / "emulators-documentation-template.docx"
    parser.add_argument("--template", type=Path, default=default_template)
    parser.add_argument("--title", required=True)
    parser.add_argument("--subtitle", required=True)
    parser.add_argument("--author", default="Ignacio Cea Forniés")
    parser.add_argument("--year", required=True, type=int)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--force", action="store_true")
    args = parser.parse_args()
    build(args.template, args.out, args.title, args.subtitle, args.author, args.year, args.force)
    print(args.out.resolve())


if __name__ == "__main__":
    main()
