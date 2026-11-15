"""Render Word-exported PDFs and record text and layout evidence."""
from pathlib import Path
import json
import shutil
import subprocess
import sys
from PIL import Image, ImageDraw
from pypdf import PdfReader

pdf = Path(sys.argv[1]).resolve()
out = pdf.parent / (pdf.stem + '-render')
out.mkdir(exist_ok=True)
renderer = shutil.which('pdftoppm')
if not renderer:
    raise RuntimeError('Poppler pdftoppm is required')
for pattern in ['page-*.png', 'overview-*.png']:
    for previous in out.glob(pattern):
        previous.unlink()
subprocess.run([renderer, '-png', '-scale-to', '1600', str(pdf), str(out / 'page')], check=True)
reader = PdfReader(pdf)
texts = [p.extract_text() for p in reader.pages]
(out / 'pages.txt').write_text('\n\f\n'.join(texts), encoding='utf-8')
images = sorted(out.glob('page-*.png'), key=lambda p: int(p.stem.split('-')[-1]))
thumb_w, thumb_h = 283, 400
for start in range(0, len(images), 12):
    group = images[start:start + 12]
    sheet = Image.new('RGB', (4 * (thumb_w + 12), 3 * (thumb_h + 30)), '#d8dde2')
    draw = ImageDraw.Draw(sheet)
    for i, path in enumerate(group):
        thumb = Image.open(path).convert('RGB')
        thumb.thumbnail((thumb_w, thumb_h))
        x, y = (i % 4) * (thumb_w + 12), (i // 4) * (thumb_h + 30)
        sheet.paste(thumb, (x, y + 20))
        draw.text((x + 5, y + 3), f'Page {start + i + 1}', fill='black')
    sheet.save(out / f'overview-{start // 12 + 1}.png')
report = {'pages': len(reader.pages), 'images': len(images),
          'errors': [i + 1 for i, t in enumerate(texts) if 'Error!' in t or 'Error de marcador' in t],
          'short_pages': [i + 1 for i, t in enumerate(texts) if len(t.strip()) < 120]}
(out / 'report.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
print(json.dumps(report))
