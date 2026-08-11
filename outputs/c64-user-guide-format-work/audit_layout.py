from __future__ import annotations

import argparse
import json
from pathlib import Path

import pdfplumber
from docx import Document
from PIL import Image, ImageDraw


HEADING_STYLES = {"Title", "Heading 1", "Heading 2", "Heading 3"}


def build_contact_sheets(render_dir: Path, output_dir: Path) -> list[Path]:
    pages = sorted(
        render_dir.glob("page-*.png"),
        key=lambda path: int(path.stem.split("-")[-1]),
    )
    output_dir.mkdir(parents=True, exist_ok=True)
    sheets: list[Path] = []
    for offset in range(0, len(pages), 4):
        batch = pages[offset : offset + 4]
        opened = [Image.open(path).convert("RGB") for path in batch]
        width = max(image.width for image in opened)
        height = max(image.height for image in opened)
        sheet = Image.new("RGB", (width * 2 + 30, height * 2 + 60), "#d7d7d7")
        draw = ImageDraw.Draw(sheet)
        for index, (path, image) in enumerate(zip(batch, opened)):
            x = (index % 2) * (width + 30)
            y = (index // 2) * (height + 30)
            sheet.paste(image, (x, y))
            draw.text((x + 8, y + 8), path.stem, fill="red", stroke_width=1, stroke_fill="white")
        out = output_dir / f"contact-{offset + 1:02d}-{offset + len(batch):02d}.png"
        sheet.save(out)
        sheets.append(out)
    return sheets


def audit_docx(path: Path) -> dict:
    document = Document(path)
    headings = []
    for paragraph in document.paragraphs:
        style_name = paragraph.style.name if paragraph.style is not None else ""
        if style_name not in HEADING_STYLES:
            continue
        direct_keep = paragraph.paragraph_format.keep_with_next
        effective_keep = (
            direct_keep
            if direct_keep is not None
            else paragraph.style.paragraph_format.keep_with_next
        )
        direct_space = paragraph.paragraph_format.space_before
        effective_space = (
            direct_space
            if direct_space is not None
            else paragraph.style.paragraph_format.space_before
        )
        headings.append(
            {
                "style": style_name,
                "text": paragraph.text,
                "keep_with_next": effective_keep,
                "space_before_pt": (
                    effective_space.pt
                    if effective_space is not None
                    else None
                ),
            }
        )
    heading2 = [item for item in headings if item["style"] == "Heading 2"]
    return {
        "heading_count": len(headings),
        "keep_with_next_failures": [item for item in headings if item["keep_with_next"] is not True],
        "heading2_count": len(heading2),
        "heading2_spacing_failures": [
            item for item in heading2 if item["space_before_pt"] is None or item["space_before_pt"] < 14
        ],
    }


def audit_pdf(path: Path) -> dict:
    possible_orphans = []
    page_count = 0
    with pdfplumber.open(path) as pdf:
        page_count = len(pdf.pages)
        for page_number, page in enumerate(pdf.pages, 1):
            words = page.extract_words(extra_attrs=["size", "fontname"], use_text_flow=True)
            # Ignore the footer and the diagonal watermark. A heading that is the
            # last body line is suspicious and is inspected in the contact sheet.
            body = [
                word for word in words
                if 60 <= float(word["x0"]) <= 540 and 70 <= float(word["top"]) <= 745
                and float(word.get("size", 0)) < 28
            ]
            if not body:
                continue
            bottom = max(float(word["top"]) for word in body)
            last_line = [word for word in body if abs(float(word["top"]) - bottom) < 2.0]
            if last_line and max(float(word.get("size", 0)) for word in last_line) >= 13:
                possible_orphans.append(
                    {
                        "page": page_number,
                        "top": round(bottom, 1),
                        "text": " ".join(word["text"] for word in last_line),
                    }
                )
    return {"page_count": page_count, "possible_orphans": possible_orphans}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--docx", type=Path, required=True)
    parser.add_argument("--pdf", type=Path, required=True)
    parser.add_argument("--render-dir", type=Path, required=True)
    parser.add_argument("--contact-dir", type=Path, required=True)
    args = parser.parse_args()
    result = {
        "docx": audit_docx(args.docx),
        "pdf": audit_pdf(args.pdf),
        "contact_sheets": [str(path) for path in build_contact_sheets(args.render_dir, args.contact_dir)],
    }
    print(json.dumps(result, indent=2, ensure_ascii=False))


if __name__ == "__main__":
    main()
