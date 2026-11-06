from pathlib import Path
import hashlib,subprocess,sys
src=Path('tmp/c264-audit-skills')
dst=Path('.codex/skills')
count=0
for p in src.rglob('*'):
    if p.is_file():
        q=dst/p.relative_to(src)
        assert q.is_file(),q
        assert hashlib.sha256(p.read_bytes()).digest()==hashlib.sha256(q.read_bytes()).digest(),q
        count+=1
for skill in sorted(src.iterdir()):
    r=subprocess.run([sys.executable,'C:/Users/ignac/.codex/skills/.system/skill-creator/scripts/quick_validate.py',str(dst/skill.name)],capture_output=True,text=True)
    print(skill.name,r.stdout.strip())
    assert r.returncode==0,r.stderr
print('Installed files verified byte-for-byte:',count)
