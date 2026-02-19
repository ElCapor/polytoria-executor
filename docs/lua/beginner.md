# Lua Beginner Guide :material-play-speed:

Welcome to the **Polytoria Executor** scripting guide. This tutorial will take you from the basics of Lua through to your first in-game automation.

---

## :material-check-circle-outline: Prerequisites

Before drafting your first script, ensure you have:
1.  **Injected the Executor** into Polytoria.
2.  **Enabled the UI** by pressing :octicons-key-24: `DELETE`.
3.  **Basic Lua Knowledge** (Understanding variables and basic functions).

---

## :material-text-box-outline: Your First Script

The simplest way to verify your executor is working is through the `sendchat` function.

!!! tip "Exercise: Hello World"
    Type the following code in the executor tab and click **Run**:
    ```lua
    sendchat("Hello from the executor!")
    ```
    If successful, you will see the message appear in the game world's chat.

---

## :material-layers-outline: Understanding Game Objects

Accessing the game world is done via the `game` global, just like in the standard engine.

!!! info "Common Game Access"
    ```lua
    -- Accessing Core Services
    local Players = game["Players"]
    local Environment = game["Environment"]

    -- Getting the Local Player
    local player = Players.LocalPlayer
    ```

---

## :material-tools: Tool Management

Automating tools is a primary use case for scripting.

### Equipping and Activating
The executor provides direct shortcuts to manage player equipment.

```lua
local backpack = game["Players"].LocalPlayer:FindChild("Backpack")
local tool = backpack:FindChild("MyTool")

-- Equip the item automatically
equiptool(tool)

-- Activate the item (e.g., weapon swing)
wait(0.5)
activatetool(tool)
```

---

## :material-mouse-move: Mouse Simulation

The executor can simulate direct hardware inputs for complex interactions.

!!! warning "Screen Coordinates"
    Mouse functions use absolute screen space. If your resolution is 1920x1080:
    - `0, 0` is the top-left corner.
    - `960, 540` is the center.

```lua
-- Move cursor to center
mouse_move(960, 540)

-- Left click once
mouse_click(0)
```
local pos = mouse_get_position()
print("Cursor at: " .. pos)
```

### Click Simulation

```lua
-- Move and click
mouse_move(100, 100)
mouse_click(0)  -- Left click
```

### Drag Operation

```lua
-- Simulate drag from point A to B
mouse_move(100, 100)
mouse_down(0)   -- Press left button
mouse_move(500, 500)
mouse_up(0)    -- Release left button
```

## Drawing Overlay

### Creating a Simple HUD

```lua
-- Clear previous drawings
draw_clear()

-- Draw a background box
draw_rect(10, 10, 300, 100, 0, 0, 0, 150, 0, 1)

-- Draw text
draw_text(20, 20, 255, 255, 255, 255, "Polytoria Executor")
draw_text(20, 50, 0, 255, 0, 255, "Script Running...")
```

### ESP Box Example

```lua
-- Simple ESP-style box around a part
local Env = game["Environment"]

game.Rendered:Connect(function(dt)
    draw_clear()
    
    local target = Env:FindChild("TargetPart")
    if target then
        -- Get screen position (simplified)
        local pos = target.Position
        draw_rect(pos.x - 20, pos.y - 20, 40, 40, 255, 0, 0, 255, 2, 0)
    end
end)
```

## HTTP Requests

### Fetching Data

```lua
-- Simple GET request
local response = httpget("https://api.ipify.org?format=json")
print("Response: " .. response)
```

### Using with Services

```lua
-- Fetch and parse JSON (basic example)
local response = httpget("https://jsonplaceholder.typicode.com/users/1")

-- Response contains user data
print("User data: " .. response)
```

## Common Patterns

### Repeat Until Success

```lua
-- Keep trying until tool is found
local tool
repeat
    wait(0.5)
    local backpack = game["Players"].LocalPlayer:FindChild("Backpack")
    tool = backpack:FindChild("Sword")
until tool

equiptool(tool)
```

### Game Loop

```lua
-- Run code every frame
local count = 0
game.Rendered:Connect(function(dt)
    count = count + 1
    draw_clear()
    draw_text(10, 10, 255, 255, 255, 255, "Frame: " .. count)
end)
```

### Event Handling

```lua
-- Simple character added detection
local player = game["Players"].LocalPlayer

-- Modifying properties directly (Infinite Stamina example)
player.StaminaEnabled = false

-- Getting current position
local pos = player.Position
sendchat("Current position: " .. tostring(pos))
```

## Debugging Tips

### Print Everything

```lua
-- Debug output
print("Script started")
print("Player: " .. tostring(game["Players"].LocalPlayer))

-- Check if something exists
local part = game["Environment"]:FindChild("TestPart")
if part then
    print("Part found!")
else
    print("Part not found!")
end
```

### Error Handling

```lua
-- Wrap in pcall for safety
local success, err = pcall(function()
    local tool = game["Players"].LocalPlayer.Backpack:FindChild("Tool")
    equiptool(tool)
end)

if not success then
    print("Error: " .. tostring(err))
end
```

## Best Practices

1. **Check if children exist** - Use `FindChild` to verify objects.
2. **Add delays** - Don't spam functions like `activatetool` too quickly.
3. **Clear drawings** - Always `draw_clear()` before redrawing in a loop.
4. **Check returns** - Custom executor functions may return error strings.
5. **Use pcall** - Protect against crashes in mission-critical scripts.

## Next Steps

- Learn about [Advanced Techniques](advanced.md) for more complex scripts
- Explore the [Functions Reference](functions.md) for all available functions
- Understand the [Internals](../internals/index.md) if you want to extend the executor
