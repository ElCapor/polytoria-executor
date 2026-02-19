# Input Functions :material-mouse:

Functions for simulating mouse input and controlling the game cursor via global functions and the `Input` singleton.

---

### `Input` (Singleton)

The `Input` object provides access to hardware state and mouse position in world space.

```lua
local input = Input

-- Get mouse position in 3D world space
local worldPos = input:GetMouseWorldPosition()

-- Detect key state (key codes or names)
if input:GetKey("W") then
    print("Moving forward!")
end

-- Events
Input.KeyDown:Connect(function(key)
    if key == "P" then
        print("P key pressed!")
    end
end)
```

---

### `mouse_move` (Global)

Moves the cursor to absolute screen coordinates.

```lua
mouse_move(x, y)
```

**Parameters:**
- `x` (number): X coordinate
- `y` (number): Y coordinate

**Example:**
```lua
-- Move cursor to center of 1920x1080 screen
mouse_move(960, 540)
```

---

### `mouse_get_position` (Global)

Gets the current cursor position as a string.

```lua
local pos = mouse_get_position()
-- pos is "x,y" format string
local x, y = string.match(pos, "(%d+),(%d+)")
print("X: " .. x .. ", Y: " .. y)
```

**Returns:** String in format `"x,y"`

---

### `mouse_click` (Global)

Simulates a full mouse click (press + release).

```lua
mouse_click(button)
```

**Parameters:**
- `button` (number): Mouse button (0=left, 1=right, 2=middle)

---

### `mouse_scroll` (Global)

Simulates mouse scroll wheel movement.

```lua
mouse_scroll(amount)
```

**Parameters:**
- `amount` (number): Scroll amount (positive=up, negative=down)

**Example:**
```lua
-- Scroll up 3 lines
mouse_scroll(3)
```

---

### `mouse_lock` (Global)

Locks or unlocks the Unity cursor.

```lua
mouse_lock(locked)
```

**Parameters:**
- `locked` (number): 
  - `1` = Lock cursor
  - `0` = Unlock cursor

---

### `mouse_visible` (Global)

Shows or hides the Unity cursor.

```lua
mouse_visible(visible)
```

**Parameters:**
- `visible` (number):
  - `1` = Show cursor
  - `0` = Hide cursor
