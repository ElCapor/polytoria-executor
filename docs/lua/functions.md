# Lua Functions Reference

Complete reference for all custom Lua functions available in the Polytoria Executor.

## Core Functions

### `loadstring`

Dynamically executes Lua code string at runtime.

```lua
local result = loadstring("print('Hello World')")
```

**Parameters:**
- `code` (string): Lua code to execute

**Returns:** The result of the executed code

**Example:**
```lua
-- Execute dynamic code
loadstring("game:GetService('Workspace'):ClearAllChildren()")

-- With return value
local func = loadstring("return 1 + 2")
local result = func()  -- result = 3
```

---

### `identifyexecutor`

Returns the executor identifier (elcapor compatible).

```lua
local executor = identifyexecutor()
print(executor)  -- Output: "elcapor"
```

**Returns:** String identifier of the executor

---

## Tool Functions

### `equiptool`

Equips a tool to the local player.

```lua
equiptool(toolInstance)
```

**Parameters:**
- `toolInstance` (Instance): Tool instance to equip

**Example:**
```lua
-- Equip a tool from Backpack
local backpack = game:GetService("Players").LocalPlayer:WaitForChild("Backpack")
local tool = backpack:WaitForChild("MyTool")
equiptool(tool)
```

---

### `unequiptool`

Unequips the currently equipped tool.

```lua
unequiptool(toolInstance)
```

**Parameters:**
- `toolInstance` (Instance): Tool instance to unequip

**Example:**
```lua
local backpack = game:GetService("Players").LocalPlayer:WaitForChild("Backpack")
local tool = backpack:WaitForChild("MyTool")
unequiptool(tool)
```

---

### `activatetool`

Activates a tool (triggers its activated event).

```lua
activatetool(toolInstance)
```

**Parameters:**
- `toolInstance` (Instance): Tool instance to activate

**Example:**
```lua
local backpack = game:GetService("Players").LocalPlayer:WaitForChild("Backpack")
local tool = backpack:WaitForChild("Sword")
equiptool(tool)
wait(0.1)
activatetool(tool)
```

---

### `serverequiptool`

Equips a tool to any player (server-side).

```lua
serverequiptool(player, toolInstance)
```

**Parameters:**
- `player` (Player): Target player
- `toolInstance` (Instance): Tool instance to equip

**Example:**
```lua
local Players = game:GetService("Players")
local player = Players:FindFirstChild("PlayerName")
local tool = game:GetService("ReplicatedStorage"):WaitForChild("Tool")

serverequiptool(player, tool)
```

---

## Chat Functions

### `sendchat`

Sends a chat message to the game chat.

```lua
sendchat(message)
```

**Parameters:**
- `message` (string): Message to send

**Example:**
```lua
sendchat("Hello from the executor!")
sendchat("Playing on Polytoria!")
```

---

## Mouse Functions

### `mouse_move`

Moves the cursor to absolute screen coordinates.

```lua
mouse_move(x, y)
```

**Parameters:**
- `x` (number): X coordinate
- `y` (number): Y coordinate

**Example:**
```lua
-- Move cursor to top-left corner
mouse_move(0, 0)

-- Move cursor to center of 1920x1080 screen
mouse_move(960, 540)
```

---

### `mouse_get_position`

Gets the current cursor position.

```lua
local pos = mouse_get_position()
-- pos is "x,y" format string
local x, y = string.match(pos, "(%d+),(%d+)")
print("X: " .. x .. ", Y: " .. y)
```

**Returns:** String in format `"x,y"`

---

### `mouse_click`

Simulates a full mouse click (press + release).

```lua
mouse_click(button)
```

**Parameters:**
- `button` (number): Mouse button
  - `0` = Left click
  - `1` = Right click
  - `2` = Middle click

**Example:**
```lua
-- Left click
mouse_click(0)

-- Right click
mouse_click(1)
```

---

### `mouse_down`

Simulates mouse button press (without release).

```lua
mouse_down(button)
```

**Parameters:**
- `button` (number): Mouse button (0=left, 1=right, 2=middle)

---

### `mouse_up`

Simulates mouse button release.

```lua
mouse_up(button)
```

**Parameters:**
- `button` (number): Mouse button (0=left, 1=right, 2=middle)

---

### `mouse_scroll`

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

-- Scroll down 2 lines
mouse_scroll(-2)
```

---

### `mouse_lock`

Locks or unlocks the Unity cursor.

```lua
mouse_lock(locked)
```

**Parameters:**
- `locked` (number): 
  - `1` = Lock cursor
  - `0` = Unlock cursor

**Example:**
```lua
-- Lock cursor for FPS-style control
mouse_lock(1)

-- Unlock cursor
mouse_lock(0)
```

---

### `mouse_visible`

Shows or hides the Unity cursor.

```lua
mouse_visible(visible)
```

**Parameters:**
- `visible` (number):
  - `1` = Show cursor
  - `0` = Hide cursor

---

## Drawing Functions

All drawing functions render to an overlay on the game screen. Coordinates are screen-space (0,0 is top-left).

!!! note
    Drawing commands are queued and rendered on the next frame. Use `draw_clear()` to reset.

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

**Example:**
```lua
-- Filled green rectangle
draw_rect(100, 100, 200, 150, 0, 255, 0, 200, 1, 1)

-- Outline blue rectangle
draw_rect(400, 100, 200, 150, 0, 0, 255, 255, 2, 0)
```

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

**Example:**
```lua
-- Filled yellow circle
draw_circle(500, 300, 50, 255, 255, 0, 255, 1, 32, 1)

-- Outline purple circle
draw_circle(500, 300, 50, 128, 0, 128, 255, 2, 32, 0)
```

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

**Example:**
```lua
draw_text(50, 50, 255, 255, 255, 255, "Polytoria Executor")
draw_text(50, 80, 255, 0, 0, 255, "Hello World!")
```

---

### `draw_clear`

Clears all pending draw commands.

```lua
draw_clear()
```

**Example:**
```lua
-- Clear all drawings
draw_clear()

-- Redraw loop
while true do
    draw_clear()
    draw_line(0, 0, 1920, 1080, 255, 0, 0, 255, 1)
    wait(1/60)
end
```

---

## Network Functions

### `fireclickdetector`

Fires a ClickDetector on any instance.

```lua
fireclickdetector(instance)
```

**Parameters:**
- `instance` (Instance): Instance with ClickDetector

**Example:**
```lua
local part = game:GetService("Workspace"):FindFirstChild("ClickPart")
if part and part:FindFirstChild("ClickDetector") then
    fireclickdetector(part)
end
```

---

## HTTP Functions

### `httpget`

Performs an HTTP GET request.

```lua
local response = httpget(url)
```

**Parameters:**
- `url` (string): URL to request

**Returns:** Response body as string

**Example:**
```lua
-- Simple GET request
local response = httpget("https://api.example.com/data")
print(response)

-- Using with JSON
local response = httpget("https://jsonplaceholder.typicode.com/todos/1")
print(response)
```

---

## Complete Example Script

```lua
-- Complete example demonstrating multiple functions

-- Send a chat message
sendchat("Executor loaded!")

-- Draw an overlay
draw_clear()
draw_rect(50, 50, 200, 100, 0, 128, 255, 200, 2, 1)
draw_text(60, 70, 255, 255, 255, 255, "Polytoria Executor")

-- Equip a tool
local player = game:GetService("Players").LocalPlayer
local backpack = player:WaitForChild("Backpack")
local tool = backpack:FindFirstChild("Sword")

if tool then
    equiptool(tool)
    wait(0.5)
    activatetool(tool)
end

-- HTTP request example
local response = httpget("https://api.ipify.org?format=json")
print("Your IP: " .. response)
```

## Error Handling

All functions return error messages as strings when invalid arguments are provided:

```lua
-- Invalid arguments
local result = equiptool("not a tool")
print(result)  -- "Invalid argument, expected Tool"
```

Always check the return value when debugging:
```lua
local result = sendchat("Hello")
if type(result) == "string" and result ~= "" then
    print("Error: " .. result)
end
```
