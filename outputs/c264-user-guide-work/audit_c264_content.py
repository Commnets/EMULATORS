from __future__ import annotations

import json
import sys
from pathlib import Path

from docx import Document
from docx.oxml.ns import qn


path = Path(sys.argv[1])
document = Document(path)
paragraphs = document.paragraphs
all_text = "\n".join(p.text for p in paragraphs)
headings = [
    {"index": i, "style": p.style.name, "text": p.text}
    for i, p in enumerate(paragraphs)
    if p.style.name in {"Title", "Heading 1", "Heading 2", "Heading 3"}
]
update = document.settings.element.find(qn("w:updateFields"))
result = {
    "paragraphs": len(paragraphs),
    "heading2_count": sum(item["style"] == "Heading 2" for item in headings),
    "machine_parameter_count": sum(item["style"] == "Heading 1" and item["text"] == "/m" for item in headings),
    "specific_commands": [
        item["text"] for item in headings
        if item["style"] == "Heading 2" and item["text"] in {"SCREENDUMP", "ATTRIBUTEDUMP", "BITMAPDUMP", "CHARSDRAW"}
    ],
    "contains_foreword": any(item["style"] == "Title" and item["text"] == "Foreword" for item in headings),
    "contains_vic20": "VIC20" in all_text or "VIC-20" in all_text,
    "contains_stale_languages": any(value in all_text for value in ("JAP: Japanese", "DKA: Danish")),
    "contains_c264_models": all(value in all_text for value in ("C16", "C116", "CP4")),
    "update_fields": None if update is None else update.get(qn("w:val")),
    "keep_failures": [
        item for item in headings
        if (paragraphs[item["index"]].paragraph_format.keep_with_next
            if paragraphs[item["index"]].paragraph_format.keep_with_next is not None
            else paragraphs[item["index"]].style.paragraph_format.keep_with_next) is not True
    ],
    "heading2_spacing_failures": [
        item for item in headings
        if item["style"] == "Heading 2" and (
            (paragraphs[item["index"]].paragraph_format.space_before
             if paragraphs[item["index"]].paragraph_format.space_before is not None
             else paragraphs[item["index"]].style.paragraph_format.space_before) is None
            or (paragraphs[item["index"]].paragraph_format.space_before
                if paragraphs[item["index"]].paragraph_format.space_before is not None
                else paragraphs[item["index"]].style.paragraph_format.space_before).pt < 14
        )
    ],
}
print(json.dumps(result, indent=2, ensure_ascii=False))
