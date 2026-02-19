# Drawing Functions :material-format-paint:

Functions for rendering 2D overlays and graphics on top of the game world.

!!! summary
    All drawing functions render to an overlay on the game screen. Coordinates are screen-space (0,0 is top-left). Drawing commands are queued and rendered on the next frame. Use `draw_clear()` to reset.

---

### `draw_line`

Draws a line between two points.

```lua
draw_line(x1, y1, x2, y2, r, g, b, a, thickness)
```

**Parameters:**
- `x1`, `y1` (number): Start position
- `x2`, `y2` (number): End position
- `r`, `g`, `b`, `a` (number): RGBA color (0-255)
- `thickness` (number): Line thickness

**Example:**
```lua
-- Draw red diagonal line
draw_line(100, 100, 500, 500, 255, 0, 0, 255, 2)
```

---

### `draw_rect`

Draws a rectangle (outline or filled).

```lua
draw_rect(x, y, w, h, r, g, b, a, thickness, filled)
```

**Parameters:**
- `x`, `y` (number): Top-left corner
- `w`, `h` (number): Width and height
- `r`, `g`, `b`, `a` (number): RGBA color (0-255)
- `thickness` (number): Line thickness (ignored if filled)
- `filled` (number): 1=filled, 0=outline

---

### `draw_circle`

Draws a circle (outline or filled).

```lua
draw_circle(x, y, radius, r, g, b, a, thickness, segments, filled)
```

**Parameters:**
- `x`, `y` (number): Center position
- `radius` (number): Circle radius
- `r`, `g`, `b`, `a` (number): RGBA color (0-255)
- `thickness` (number): Line thickness (ignored if filled)
- `segments` (number): Number of segments (higher = smoother)
- `filled` (number): 1=filled, 0=outline

---

### `draw_text`

Draws text on screen.

```lua
draw_text(x, y, r, g, b, a, text)
```

**Parameters:**
- `x`, `y` (number): Text position
- `r`, `g`, `b`, `a` (number): RGBA color (0-255)
- `text` (string): Text to display

---

### `draw_clear`

Clears all pending draw commands.

```lua
draw_clear()
```

**Example:**
```lua
-- Redraw loop using game.Rendered
game.Rendered:Connect(function(dt)
    draw_clear()
    draw_line(0, 0, 1920, 1080, 255, 0, 0, 255, 1)
end)
```
