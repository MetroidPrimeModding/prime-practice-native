# Game text markup (CTextParser)

Summary of the markup the game's `CTextParser` accepts (see `src/Kyoto/Text/CTextParser.cpp` in the prime decomp). It
applies to any text rendered through the game's text system (`CGuiTextSupport`, `CTextExecuteBuffer`).

## Syntax

- A tag is `&name;`. Everything between `&` and the next `;` is the tag body.
- `&&` produces a literal `&`.
- Unknown tags go to `HandleUserTag`, which is a no-op, so they're silently dropped.
- Nothing is validated: invalid hex digits parse as 0, and `ParseInt` doesn't check for digits.

## Tags

| Tag | Effect |
|---|---|
| `&font=XXXXXXXX;` | Switch font; value is the FONT asset ID as 8 hex digits. |
| `&image=...;` | Inline texture (see [Images](#images)). |
| `&fg-color=#RRGGBB[AA];` | Foreground color: sets the main color and refreshes the main + geometry palette entries. |
| `&main-color=#RRGGBB[AA];` | Main (fill) color, palette entry 0. |
| `&outline-color=#RRGGBB[AA];` | Outline color, palette entry 1. Only has an effect on fonts in `OneLayerOutline` mode. |
| `&geometry-color=...;` | **Broken in retail**: skips 11 characters instead of 15, so it parses `lor=#RR...` and yields roughly `(0, 0, RR, 255)`. Avoid. |
| `&line-spacing=N;` | Line spacing as a percentage (`100` = 1.0). Negative values are allowed. |
| `&line-extra-space=N;` | Extra space between lines as an integer. Can be negative. |
| `&just=left\|center\|right\|full\|nleft\|ncenter\|nright;` | Horizontal justification. |
| `&vjust=top\|center\|bottom\|full\|ntop\|ncenter\|nbottom;` | Vertical justification. |
| `&push;` / `&pop;` | Save/restore the whole render state (colors, font, justification, ...). |
| `&colorNN...;` | Palette color override (see [Color overrides](#color-overrides)). |

The `*Mono` justification enum values exist, but no tag can set them.

## Colors

- Format is `#RRGGBB` or `#RRGGBBAA`, case-insensitive hex.
- The first character is skipped without being checked, so it doesn't have to be `#`.
- Alpha is only read when the value is exactly 9 characters; otherwise it's 255.

## Color overrides

The parser uses fixed offsets, and no example strings were found in the decomp. This layout is inferred from the
parser, not confirmed against retail strings:

- `color`, then 1 ignored character (index 5).
- 1–2 decimal digits: the palette index. The parser accepts 0–99, but the render state only has 16 override slots, so
  keep it under 16.
- 10 more ignored characters.
- Then either `no` (removes the override) or a color.

An override pins that palette entry, so later `main-color`/`outline-color` tags stop changing it until the override is
removed.

## Images

Asset IDs are 8 hex digits. If a texture remap table is passed to the parser, the ID is looked up there first. The
`A`/`SA`/`SI` prefixes are case-insensitive.

- `&image=XXXXXXXX;` — single texture, scale 1×1.
- `&image=SI,sx,sy,XXXXXXXX;` — single texture with a scale/crop (floats). Needs exactly 4 tokens.
- `&image=A,fps,ID1,ID2,...;` — animated sequence at `fps`, scale 1×1.
- `&image=SA,fps,sx,sy,ID1,ID2,...;` — animated sequence with a scale. Needs at least 5 tokens.

`CPauseScreen` and `CAutoMapper` build `&image=` strings at runtime, which is a useful reference.

## Example

```
&push;&main-color=#FF0000;warning&pop; normal text
```
