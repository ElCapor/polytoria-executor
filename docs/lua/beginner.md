# Lua Beginner Guide

This guide will help you get started with scripting in the Polytoria Executor. It covers the basics of the Lua environment and how to use the custom functions effectively.

## Prerequisites

Before starting, ensure you have:
1. The executor injected into the game
2. Access to the Executor tab in the UI (press `DELETE`)
3. Basic understanding of Lua/Luau syntax

## Your First Script

Let's start with a simple script that sends a chat message:

```lua
sendchat("Hello, Polytoria!")
```

Type this in the Executor tab and press Run. You should see the message appear in chat.

## Understanding the Basics

### Game Objects

The executor provides access to the game through standard Polytoria's API:

```lua
-- Get services
local Players = game:GetService("Players")
local Workspace = game:GetService("Workspace")

-- Get local player
local player = Players.LocalPlayer

-- Find instances
local part = Workspace:FindFirstChild("PartName")
```

### Waiting for Instances

Many scripts require waiting for instances to load:

```lua
local player = game:GetService("Players").LocalPlayer
local backpack = player:WaitForChild("Backpack")
local tool = backpack:WaitForChild("MyTool")

-- Now the tool is guaranteed to exist
equiptool(tool)
```

## Tool Management

### Equipping Tools

The most common use case is equipping and activating tools:

```lua
-- Wait for backpack and tool to load
local player = game:GetService("Players").LocalPlayer
local backpack = player:WaitForChild("Backpack")
local tool = backpack:WaitForChild("Sword")

-- Equip the tool
equiptool(tool)

-- Activate it (like clicking)
wait(0.5)
activatetool(tool)
```

### Complete Tool Script

```lua
local Players = game:GetService("Players")
local player = Players.LocalPlayer
local backpack = player:WaitForChild("Backpack")

-- Find all tools in backpack
for _, item in pairs(backpack:GetChildren()) do
    if item:IsA("Tool") then
        print("Found tool: " .. item.Name)
        equiptool(item)
        wait(0.2)
        activatetool(item)
        wait(0.2)
    end
end
```

## Mouse Control

### Basic Mouse Movement

```lua
-- Move cursor to position
mouse_move(500, 500)

-- Get current position
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
local Workspace = game:GetService("Workspace")

while true do
    draw_clear()
    
    local target = Workspace:FindFirstChild("TargetPart")
    if target then
        -- Get screen position (simplified - actual implementation needs WorldToScreen)
        local pos = target.Position
        draw_rect(pos.X - 20, pos.Y - 20, 40, 40, 255, 0, 0, 255, 2, 0)
    end
    
    wait(1/30)  -- 30 FPS
end
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
    local backpack = game:GetService("Players").LocalPlayer:WaitForChild("Backpack")
    tool = backpack:FindFirstChild("Sword")
until tool

equiptool(tool)
```

### Game Loop

```lua
-- Run code every frame
local count = 0
while true do
    count = count + 1
    draw_clear()
    draw_text(10, 10, 255, 255, 255, 255, "Frame: " .. count)
    wait(1/60)
end
```

### Event Handling

```lua
-- Simple character added detection
local player = game:GetService("Players").LocalPlayer
local character = player.Character or player.CharacterAdded:Wait()

sendchat("Character loaded: " .. character.Name)
```

## Debugging Tips

### Print Everything

```lua
-- Debug output
print("Script started")
print("Player: " .. tostring(game:GetService("Players").LocalPlayer))

-- Check if something exists
local part = game:GetService("Workspace"):FindFirstChild("TestPart")
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
    local tool = game:GetService("Players").LocalPlayer.Backpack:FindFirstChild("Tool")
    equiptool(tool)
end)

if not success then
    print("Error: " .. tostring(err))
end
```

## Best Practices

1. **Always use WaitForChild** - Don't assume instances exist
2. **Add delays** - Don't spam functions too quickly
3. **Clear drawings** - Always clear before redrawing
4. **Check returns** - Functions may return error strings
5. **Use pcall** - Protect against crashes

## Next Steps

- Learn about [Advanced Techniques](advanced.md) for more complex scripts
- Explore the [Functions Reference](functions.md) for all available functions
- Understand the [Internals](../internals/index.md) if you want to extend the executor
