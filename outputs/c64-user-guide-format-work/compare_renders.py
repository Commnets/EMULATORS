from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageChops


def page_number(path: Path) -> int:
    return int(path.stem.split("-")[-1])


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("before", type=Path)
    parser.add_argument("after", type=Path)
    args = parser.parse_args()

    before = {page_number(path): path for path in args.before.glob("page-*.png")}
    after = {page_number(path): path for path in args.after.glob("page-*.png")}
    if before.keys() != after.keys():
        raise SystemExit(f"Page sets differ: {sorted(before)} vs {sorted(after)}")

    changed = []
    for number in sorted(before):
        image_before = Image.open(before[number]).convert("RGB")
        image_after = Image.open(after[number]).convert("RGB")
        diff = ImageChops.difference(image_before, image_after)
        bbox = diff.getbbox()
        if bbox is not None:
            changed.append((number, bbox))
    print(f"pages={len(before)}")
    print(f"changed={changed}")


if __name__ == "__main__":
    main()
