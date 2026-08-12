from pathlib import Path
import sys

from PIL import Image, ImageDraw


source = Path(sys.argv[1])
destination = Path(sys.argv[2])
destination.mkdir(parents=True, exist_ok=True)
pages = sorted(source.glob("page-*.png"), key=lambda p: int(p.stem.split("-")[-1]))
thumb_width = 720
gap = 24

for offset in range(0, len(pages), 4):
    batch = pages[offset:offset + 4]
    thumbs = []
    for page in batch:
        image = Image.open(page).convert("RGB")
        height = round(image.height * thumb_width / image.width)
        image = image.resize((thumb_width, height), Image.Resampling.LANCZOS)
        draw = ImageDraw.Draw(image)
        draw.rectangle((0, 0, 115, 25), fill="white")
        draw.text((5, 5), f"page {int(page.stem.split('-')[-1])}", fill="red")
        thumbs.append(image)
    cell_height = max(image.height for image in thumbs)
    sheet = Image.new("RGB", (thumb_width * 2 + gap, cell_height * 2 + gap), "#d9d9d9")
    for index, image in enumerate(thumbs):
        x = (index % 2) * (thumb_width + gap)
        y = (index // 2) * (cell_height + gap)
        sheet.paste(image, (x, y))
    first = offset + 1
    last = offset + len(batch)
    sheet.save(destination / f"contact-{first:02d}-{last:02d}.png")
