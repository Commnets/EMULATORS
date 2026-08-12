from pathlib import Path
import json
import sys

from docx import Document
from docx.oxml.ns import qn


path = Path(sys.argv[1])
document = Document(path)
paragraphs = document.paragraphs
heading2 = [p.text.strip() for p in paragraphs if p.style.name == "Heading 2"]
required = ["SYSVARS", "SYSVAR", "ULA", "ULAEVENTS", "DFDUMP", "CHARSDRAW"]
stale_terms = ("C264", "Commodore 264", "C16", "C116", "Plus/4", "VIC20", "VIC-20", "COMMODORE::")

keep_failures = []
spacing_failures = []
for index, paragraph in enumerate(paragraphs):
    if paragraph.style.name in {"Title", "Heading 1", "Heading 2", "Heading 3"}:
        ppr = paragraph._p.pPr
        direct = ppr is not None and ppr.find(qn("w:keepNext")) is not None
        inherited = bool(document.styles[paragraph.style.name].paragraph_format.keep_with_next)
        if not direct and not inherited:
            keep_failures.append((index, paragraph.style.name, paragraph.text))
    if paragraph.style.name == "Heading 2":
        value = paragraph.paragraph_format.space_before
        inherited = document.styles["Heading 2"].paragraph_format.space_before
        points = (value or inherited).pt if (value or inherited) is not None else 0
        if points < 14:
            spacing_failures.append((index, paragraph.text, points))

settings = document.settings.element
update = settings.find(qn("w:updateFields"))
texts = "\n".join(p.text for p in paragraphs)
fake_bullets = [p.text[:30] for p in paragraphs if p.text.startswith(("• ", "â€¢ "))]
real_bullets = []
for p in paragraphs:
    ppr = p._p.pPr
    if ppr is not None and ppr.find(qn("w:numPr")) is not None:
        real_bullets.append(p.text[:30])

print(json.dumps({
    "paragraphs": len(paragraphs),
    "heading2_count": len(heading2),
    "required_commands": {name: name in heading2 for name in required},
    "machine_parameter_count": sum(1 for p in paragraphs if p.text.strip() == "/m" and p.style.name == "Heading 1"),
    "contains_foreword": "Foreword" in texts,
    "contains_zx_models": all(name in texts for name in ("ZX80", "ZX811", "ZX812", "ZX813")),
    "stale_terms": [term for term in stale_terms if term in texts],
    "update_fields": None if update is None else update.get(qn("w:val")),
    "keep_failures": keep_failures,
    "heading2_spacing_failures": spacing_failures,
    "fake_bullet_count": len(fake_bullets),
    "fake_bullet_samples": fake_bullets[:5],
    "real_bullet_count": len(real_bullets),
}, indent=2, ensure_ascii=False))
