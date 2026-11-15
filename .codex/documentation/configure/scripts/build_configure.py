"""Build the English onboarding guide using the canonical EMULATORS template."""

from pathlib import Path
import copy
import hashlib
import json
import re
import shutil
import subprocess
import sys
import zipfile
from lxml import etree
from docx import Document
from docx.shared import Pt, Cm, RGBColor
from docx.oxml import OxmlElement
from docx.oxml.ns import qn

ROOT = Path(__file__).resolve().parents[4]
WORK = ROOT / '.codex/tmp/configure-guide'
SOURCE = ROOT / '.codex/documentation/configure/sources/guide.md'
TEMPLATE = ROOT / '.codex/skills/emulators-documentation-authoring/assets/emulators-documentation-template.docx'
OUTPUT = ROOT / 'Configure.docx'


def add_text(paragraph, text):
    for index, part in enumerate(text.split('`')):
        run = paragraph.add_run(part)
        if index % 2:
            run.font.name = 'Consolas'
            run.font.size = Pt(9)


def add_table(doc, lines):
    rows = [[cell.strip() for cell in line.strip().strip('|').split('|')] for line in lines]
    rows = [row for row in rows if not all(re.fullmatch(r'[:\- ]+', c) for c in row)]
    table = doc.add_table(rows=0, cols=len(rows[0]))
    table.autofit = False
    widths = [5.0, 10.0] if len(rows[0]) == 2 else [3.3, 5.85, 5.85]
    for col, width in zip(table.columns, widths):
        col.width = Cm(width)
    borders = OxmlElement('w:tblBorders')
    for edge in ['top', 'left', 'bottom', 'right', 'insideH', 'insideV']:
        item = OxmlElement('w:' + edge)
        for k, v in [('val', 'single'), ('sz', '4'), ('color', 'D9D9D9')]:
            item.set(qn('w:' + k), v)
        borders.append(item)
    table._tbl.tblPr.append(borders)
    for number, cells in enumerate(rows):
        row = table.add_row()
        props = row._tr.get_or_add_trPr()
        props.append(OxmlElement('w:cantSplit'))
        if number == 0:
            props.append(OxmlElement('w:tblHeader'))
        for cell, value, width in zip(row.cells, cells, widths):
            cell.width = Cm(width)
            p = cell.paragraphs[0]
            p.paragraph_format.space_after = Pt(4)
            p.paragraph_format.space_before = Pt(4)
            p.paragraph_format.line_spacing = 1.0
            add_text(p, value)
            for run in p.runs:
                run.font.size = Pt(9)
                run.bold = number == 0
            shading = OxmlElement('w:shd')
            shading.set(qn('w:fill'), 'E8EDF1' if number == 0 else ('F6F7F8' if number % 2 == 0 else 'FFFFFF'))
            cell._tc.get_or_add_tcPr().append(shading)
            cell.vertical_alignment = 1


def add_markdown(doc, source):
    lines = source.splitlines()
    i = 0
    in_code = False
    while i < len(lines):
        line = lines[i]
        if line.startswith('```'):
            in_code = not in_code
            i += 1
            continue
        if in_code:
            p = doc.add_paragraph(style='Configuration Code')
            p.paragraph_format.keep_with_next = (
                i + 1 < len(lines) and not lines[i + 1].startswith('```'))
            # Explicit line continuations are used in the guide to fit the template width.
            p.add_run(line)
        elif line.startswith('|'):
            group = []
            while i < len(lines) and lines[i].startswith('|'):
                group.append(lines[i])
                i += 1
            add_table(doc, group)
            continue
        elif line.startswith('# '):
            doc.add_paragraph(line[2:], style='Title')
        elif line.startswith('## '):
            doc.add_paragraph(line[3:], style='Heading 1')
        elif line.startswith('### '):
            doc.add_paragraph(line[4:], style='Heading 2')
        elif line.strip():
            previous = doc.element.body[-2]
            previous_style = previous.find('w:pPr/w:pStyle', namespaces=previous.nsmap)
            if previous.tag == qn('w:tbl') or (
                    previous_style is not None and
                    previous_style.get(qn('w:val')) == doc.styles['Configuration Code'].style_id):
                doc.add_paragraph(style='Normal')
            p = doc.add_paragraph()
            if re.match(r'^\d+\. ', line):
                p.paragraph_format.left_indent = Cm(0.5)
                p.paragraph_format.first_line_indent = Cm(-0.5)
            elif line.startswith('- '):
                p.paragraph_format.left_indent = Cm(0.45)
                p.paragraph_format.first_line_indent = Cm(-0.3)
                line = '\u2022 ' + line[2:]
            add_text(p, line)
        i += 1


def field(paragraph, instruction, result):
    run = paragraph.add_run()
    begin = OxmlElement('w:fldChar')
    begin.set(qn('w:fldCharType'), 'begin')
    code = OxmlElement('w:instrText')
    code.set(qn('xml:space'), 'preserve')
    code.text = instruction
    separate = OxmlElement('w:fldChar')
    separate.set(qn('w:fldCharType'), 'separate')
    text = OxmlElement('w:t')
    text.text = result
    end = OxmlElement('w:fldChar')
    end.set(qn('w:fldCharType'), 'end')
    for node in [begin, code, separate, text, end]:
        run._r.append(node)


def main():
    WORK.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(TEMPLATE, WORK / 'reference.docx')
    contract = ROOT / '.codex/documentation/configure/state/template-contract.md'
    if contract.exists():
        shutil.copyfile(contract, WORK / 'artifact.md')
    script = ROOT / '.codex/skills/emulators-documentation-authoring/scripts/start_document.py'
    base = WORK / 'base.docx'
    subprocess.run([sys.executable, str(script), '--title', 'EMULATORS', '--subtitle', 'Installation Configuration and First Run', '--author', 'Ignacio Cea Forniés', '--year', '2026', '--out', str(base), '--force'], check=True)
    doc = Document(base)
    # Preserve the template cover content control and the section/header/footer system.
    body = doc._element.body
    for node in list(body):
        if node.tag != qn('w:sectPr') and node.tag != qn('w:sdt'):
            body.remove(node)
    for name in ['Title', 'Heading 1', 'Heading 2', 'Heading 3']:
        doc.styles[name].paragraph_format.keep_with_next = True
    # Heading 1 is used for the template's standalone lists. Internal headings
    # use the same typography but must not force a new page for every subsection.
    doc.styles['Heading 1'].paragraph_format.page_break_before = False
    doc.styles['Heading 2'].paragraph_format.space_before = Pt(14)
    code = doc.styles.add_style('Configuration Code', 1)
    code.base_style = doc.styles['Normal']
    code.font.name = 'Consolas'
    code.font.size = Pt(9)
    code.paragraph_format.space_before = Pt(0)
    code.paragraph_format.space_after = Pt(0)
    code.paragraph_format.line_spacing = 1.0
    code.paragraph_format.keep_together = True
    contents_style = doc.styles.add_style('Configuration Contents', 1)
    contents_style.base_style = doc.styles['Heading 1']
    p = doc.add_paragraph('Contents', style=contents_style)
    p.paragraph_format.page_break_before = True
    p = doc.add_paragraph()
    field(p, ' TOC \\o "1-3" \\h \\z \\u ', 'Update the contents in Word.')
    add_markdown(doc, SOURCE.read_text(encoding='utf-8'))
    p = doc.add_paragraph('Illustrations', style='Heading 1')
    p.paragraph_format.page_break_before = True
    field(doc.add_paragraph(style='table of figures'), ' TOC \\h \\z \\c "Illustration" ', 'No illustrations are used in this guide.')
    p = doc.add_paragraph('Equations', style='Heading 1')
    # Both empty lists share the final page instead of creating a mostly empty page.
    field(doc.add_paragraph(style='table of figures'), ' TOC \\h \\z \\c "Equation" ', 'No equations are used in this guide.')
    for p in doc.paragraphs:
        level = {'Title': '0', 'Heading 1': '1', 'Heading 2': '2'}.get(p.style.name)
        if p.style.name == 'Configuration Contents':
            level = '9'
        if level is not None:
            props = p._p.get_or_add_pPr()
            outline = props.find(qn('w:outlineLvl'))
            if outline is None:
                outline = OxmlElement('w:outlineLvl')
                props.append(outline)
            outline.set(qn('w:val'), level)
    for section in doc.sections:
        for footer in [section.footer, section.first_page_footer, section.even_page_footer]:
            for p in footer.paragraphs:
                for run in p.runs:
                    if 'Página' in run.text:
                        run.text = run.text.replace('Página', 'Page')
    for s in doc.styles:
        if s.type == 1 and s.name not in ['Configuration Code']:
            s.element.get_or_add_pPr().append(OxmlElement('w:widowControl'))
    doc.save(OUTPUT)
    with zipfile.ZipFile(TEMPLATE) as source, zipfile.ZipFile(OUTPUT) as final:
        media = [n for n in source.namelist() if n.startswith('word/media/')]
        if not all(source.read(n) == final.read(n) for n in media):
            raise RuntimeError('Template media changed')
    print(OUTPUT)


if __name__ == '__main__':
    main()
