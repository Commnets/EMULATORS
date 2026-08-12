from __future__ import annotations

import json
import sys
from pathlib import Path

from docx import Document
from docx.oxml.ns import qn


path = Path(sys.argv[1])
document = Document(path)
paragraphs = document.paragraphs
text = "\n".join(p.text for p in paragraphs)
required = ("SYSVARS", "SYSVAR", "ULA", "ULAEVENTS", "SCREENDUMP", "COLORDUMP", "CHARSDRAW")
headings2 = [p for p in paragraphs if p.style.name == "Heading 2"]


def effective_keep(paragraph) -> bool:
    direct = paragraph.paragraph_format.keep_with_next
    return bool(direct if direct is not None else paragraph.style.paragraph_format.keep_with_next)


def effective_space_before(paragraph):
    direct = paragraph.paragraph_format.space_before
    return direct if direct is not None else paragraph.style.paragraph_format.space_before


keep_failures = [p.text for p in paragraphs if p.style.name in {"Title", "Heading 1", "Heading 2", "Heading 3"} and not effective_keep(p)]
spacing_failures = [p.text for p in headings2 if effective_space_before(p) is None or effective_space_before(p).pt < 14]
settings = document.settings.element.find(qn("w:updateFields"))
report = {
    "paragraphs": len(paragraphs),
    "heading2_count": len(headings2),
    "required_commands": {name: any(p.text.strip() == name for p in headings2) for name in required},
    "contains_foreword": "Foreword" in text,
    "contains_zxstd": "ZXSTD" in text,
    "contains_16k_48k": "16 KiB" in text and "48 KiB" in text,
    "stale_terms": [term for term in ("ZX80", "ZX81", "DFDUMP", "Commodore", "game.p") if term in text],
    "update_fields": None if settings is None else settings.get(qn("w:val")),
    "keep_failures": keep_failures,
    "heading2_spacing_failures": spacing_failures,
    "fake_bullet_count": sum(1 for p in paragraphs if p.text.startswith(("• ", "ï¿½ ", "â€¢ "))),
    "real_bullet_count": sum(1 for p in paragraphs if p._p.pPr is not None and p._p.pPr.numPr is not None),
}
print(json.dumps(report, indent=2, ensure_ascii=False))

if (
    len(headings2) != 74
    or not all(report["required_commands"].values())
    or report["stale_terms"]
    or report["update_fields"] != "true"
    or keep_failures
    or spacing_failures
    or report["fake_bullet_count"]
):
    raise SystemExit(1)
