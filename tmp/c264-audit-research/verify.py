from pathlib import Path
import re,yaml
base=Path('tmp/c264-audit-skills').resolve()
errors=[]
for p in base.rglob('*.md'):
    for target in re.findall(r'\]\(([^)]+)\)',p.read_text(encoding='utf-8')):
        if '://' in target or target.startswith('#'): continue
        q=(p.parent/target.split('#')[0]).resolve()
        if not q.exists(): errors.append(f'{p}: missing {target}')
for p in base.glob('*/agents/openai.yaml'):
    d=yaml.safe_load(p.read_text(encoding='utf-8'))['interface']
    assert 25<=len(d['short_description'])<=64
    assert '$'+p.parent.parent.name in d['default_prompt']
for p in base.glob('*/SKILL.md'):
    assert p.parent.name==yaml.safe_load(p.read_text(encoding='utf-8').split('---')[1])['name']
assert not errors,errors
print('All relative reference links and four metadata files verified')
print('Markdown words:',sum(len(p.read_text(encoding='utf-8').split()) for p in base.rglob('*.md')))
print('Final staged files:')
for p in sorted(base.rglob('*')):
    if p.is_file(): print(p.relative_to(base),p.stat().st_size)
