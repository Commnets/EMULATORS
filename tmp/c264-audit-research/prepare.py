from pathlib import Path
import shutil,hashlib,json,re
base=Path('tmp/c264-audit-skills')
specs={
'c264-ted-audit':('C264 TED Audit','Auditar vídeo, sonido, timers y bus del TED'),
'c264-tpi-audit':('C264 TPI 6523 Audit','Auditar TPI/TIA y la interfaz paralela 1551'),
'c264-acia-audit':('C264 ACIA 6551 Audit','Auditar UART, IRQ y cableado ACIA del Plus4'),
'c264-6529b-audit':('C264 6529B Audit','Auditar puertos 6529B, teclado y user port')}
for skill,(display,short) in specs.items():
    agents=base/skill/'agents'; agents.mkdir(exist_ok=True)
    values={'display_name':display,'short_description':short,'default_prompt':f'Usa ${skill} para revisar la fidelidad del chip y su conexión en EMULATORS, aportando fuentes y pruebas reproducibles.'}
    (agents/'openai.yaml').write_text('interface:\n'+''.join(f'  {k}: {json.dumps(v,ensure_ascii=False)}\n' for k,v in values.items()),encoding='utf-8')
files=[
('C:/Temp/(C264) TED System Hardware Manual.pdf','c264-ted-audit','TED-System-Hardware-Manual.pdf'),
('C:/Temp/TED 7360R0 Preliminary Data Sheet.pdf','c264-ted-audit','TED-7360R0-Preliminary.pdf'),
('tmp/c264-audit-research/Plus4-Service-1984.pdf','c264-ted-audit','Plus4-Service-1984.pdf'),
('tmp/c264-audit-research/C16-Service-1984.pdf','c264-ted-audit','C16-Service-1984.pdf'),
('C:/Temp/6523-Commodore.pdf','c264-tpi-audit','6523-Commodore.pdf'),
('tmp/c264-audit-research/1551-paddle-251925.png','c264-tpi-audit','1551-paddle-251925.png'),
('C:/Temp/6551_acia.pdf','c264-acia-audit','6551_acia.pdf'),
('C:/Temp/6529B.pdf','c264-6529b-audit','6529B.pdf')]
rows={s:[] for s in specs}
for original,skill,name in files:
    dest=base/skill/'references'/name
    shutil.copyfile(original,dest)
    digest=hashlib.sha256(dest.read_bytes()).hexdigest()
    assert digest==hashlib.sha256(Path(original).read_bytes()).hexdigest()
    rows[skill].append(f'| `{name}` | {dest.stat().st_size} | `{digest}` |')
for skill,items in rows.items():
    p=base/skill/'references/sources.md'
    with p.open('a',encoding='utf-8') as f:
        f.write('\n## Identidad de las copias conservadas\n\n| Archivo | Bytes | SHA-256 |\n|---|---:|---|\n'+'\n'.join(items)+'\n')
for p in base.rglob('*.md'):
    data=p.read_text(encoding='utf-8-sig')
    p.write_text(data,encoding='utf-8')
print('Prepared',len(list(base.rglob('*.*'))),'files')
for skill in specs:
    print(skill,'destination exists:',(Path('.codex/skills')/skill).exists())
