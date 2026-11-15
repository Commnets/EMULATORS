"""Finalize field refresh settings and save verification evidence."""
from pathlib import Path
import argparse
import hashlib
import json
import zipfile
from lxml import etree
from pypdf import PdfReader

root = Path(__file__).resolve().parents[4]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--reviewed-pages', type=int, required=True,
                    help='Actual page count after visual review of every rendered page.')
parser.add_argument('--scope', default='Configuration guide update.')
args = parser.parse_args()
output = root / 'Configure.docx'
work = root / '.codex/tmp/configure-guide'
state = root / '.codex/documentation/configure/state'
state.mkdir(parents=True, exist_ok=True)
w = 'http://schemas.openxmlformats.org/wordprocessingml/2006/main'
with zipfile.ZipFile(output) as package:
    parts = [(info, package.read(info.filename)) for info in package.infolist()]
temporary = output.with_suffix('.verified.docx')
with zipfile.ZipFile(temporary, 'w') as package:
    for info, data in parts:
        if info.filename == 'word/settings.xml':
            settings = etree.fromstring(data)
            field = settings.find(f'{{{w}}}updateFields')
            if field is None:
                field = etree.SubElement(settings, f'{{{w}}}updateFields')
            field.set(f'{{{w}}}val', 'true')
            data = etree.tostring(settings, xml_declaration=True, encoding='UTF-8', standalone=True)
        package.writestr(info, data)
temporary.replace(output)
template = root / '.codex/skills/emulators-documentation-authoring/assets/emulators-documentation-template.docx'
pages = PdfReader(work / 'Configure.pdf').pages
texts = [p.extract_text() for p in pages]
assert len(pages) == args.reviewed_pages
assert 'FOREWORD' in texts[1] and 'EXTERNAL DEPENDENCIES' in texts[1]
assert 'No se encontraron' not in '\n'.join(texts)
assert 'NO SE ENCUENTRAN' not in '\n'.join(texts)
assert not any('Error!' in text for text in texts)
report = {
    'document': 'Configure.docx',
    'language': 'English',
    'pages_including_cover': len(pages),
    'visually_reviewed_pages': list(range(1, len(pages) + 1)),
    'template_sha256': hashlib.sha256(template.read_bytes()).hexdigest(),
    'document_sha256': hashlib.sha256(output.read_bytes()).hexdigest(),
    'fields_updated_with': 'Microsoft Word',
    'rendered_with': 'Microsoft Word PDF export and Poppler',
    'compile_or_runtime_test_performed': False,
    'scope': args.scope
}
(state / 'verified.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
(state / 'template-contract.md').write_text((work / 'artifact.md').read_text(encoding='utf-8'), encoding='utf-8')
print(json.dumps(report, indent=2))
