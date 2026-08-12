# ARGOS visual identity

Professional vector reconstruction of the approved design stored in
`argos-logo-reference.png`. The artwork preserves its characteristic sail,
negative-space `A`, hull, three stepped circuit traces, terminal pads and custom
ARGOS letterforms.

Unlike a bitmap autotrace, the production masters use deliberate Bézier curves,
controlled corners and consistent radii. The wordmark is made from paths and has
no font dependency.

## Horizontal lockup

- `argos-logo-horizontal.svg`: full-color master on a transparent background.
- `argos-logo-horizontal-monochrome.svg`: single-ink master for light media;
  circuit traces are paper knockouts.
- `argos-logo-horizontal-dark.svg`: reversed master on ARGOS Navy.
- `argos-logo-horizontal.png`: preview rendered from the color master.

## Stacked lockup — wordmark below the symbol

- `argos-logo-stacked.svg`: full-color master on a transparent background.
- `argos-logo-stacked-monochrome.svg`: single-ink master for light media;
  circuit traces are paper knockouts.
- `argos-logo-stacked-dark.svg`: reversed master on ARGOS Navy.
- `argos-logo-stacked.png`: preview rendered from the color master.

## Standalone symbol

- `argos-mark.svg`: full-color symbol on a transparent background.
- `argos-mark-monochrome.svg`: single-ink symbol for light media.
- `argos-mark-dark.svg`: reversed symbol on ARGOS Navy.
- `argos-mark.png`: preview rendered from the color master.

## Source and maintenance

- `argos-logo-reference.png`: approved visual reference supplied by the user.
- `build-assets.js`: authoritative geometry and deterministic asset generator.

Run `node build-assets.js` from this directory after editing the authoritative
geometry. It rebuilds all nine SVG masters from the same symbol and wordmark, so
horizontal and stacked versions cannot drift apart.

## Palette

| Color | Hex | Purpose |
| --- | --- | --- |
| ARGOS Navy | `#062649` | Core identity color |
| Navy highlight | `#07315D` | Subtle upper gradient stop |
| Navy shade | `#031E3B` | Subtle lower gradient stop |
| Circuit Orange | `#F27511` | Core circuit color |
| White | `#FFFFFF` | Reversed artwork |

Do not alter the geometry, proportions, typography or circuit paths. Do not add
shadows, outlines, extra colors or substitute fonts.
