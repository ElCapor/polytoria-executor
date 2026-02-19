# Lua Advanced Guide

This guide covers advanced techniques for writing complex scripts and understanding how to extend the Lua environment.

## Advanced Drawing Techniques

### Dynamic HUD Positioning

```lua
-- Calculate screen-relative positions
local screenSize = Vector2.new(1920, 1080)  -- Game resolution
local scale = function(x) return x * (screenSize.X / 1920) end

-- Scaled drawing
draw_rect(scale(100), scale(50), scale(200), scale(100), 0, 128, 255, 200, 2, 1)
```

### Smooth Animations

```lua
local frame = 0

-- Animation loop with easing
while true do
    frame = frame + 0.05
    local x = 500 + math.sin(frame) * 200
    local y = 300 + math.cos(frame) * 100
    
    draw_clear()
    draw_circle(x, y, 30, 255, 0, 0, 255, 2, 32, 1)
    
    wait(1/60)
end
```

### Multiple Layer Rendering

```lua
-- Layer 1: Background
draw_rect(50, 50, 400, 300, 0, 0, 0, 100, 0, 1)

-- Layer 2: Border
draw_rect(50, 50, 400, 300, 255, 255, 255, 255, 3, 0)

-- Layer 3: Text
draw_text(60, 60, 255, 255, 255, 255, "Layered HUD")
```

## Complex Tool Scripts

### Tool Combo System

```lua
local combo = {}
local lastActivation = 0

local function activateTool(tool)
    equiptool(tool)
    wait(0.1)
    activatetool(tool)
end

-- Auto-combo script
local backpack = game:GetService("Players").LocalPlayer:WaitForChild("Backpack")
local tools = {}

for _, item in pairs(backpack:GetChildren()) do
    if item:IsA("Tool") then
        table.insert(tools, item)
    end
end

local current = 1
while true do
    if #tools > 0 then
        activateTool(tools[current])
        current = current % #tools + 1
    end
    wait(0.5)
end
```

### Weapon Spam Script

```lua
local player = game:GetService("Players").LocalPlayer
local mouse = player:GetMouse()

local function spamActivate()
    local backpack = player:WaitForChild("Backpack")
    for _, tool in pairs(backpack:GetChildren()) do
        if tool:IsA("Tool") then
            equiptool(tool)
            for i = 1, 5 do
                activatetool(tool)
                wait(0.1)
            end
        end
    end
end
```

## Mouse Automation

### Auto Clicker

```lua
local clicking = false
local cps = 10  -- Clicks per second

local function clickLoop()
    while clicking do
        mouse_click(0)
        wait(1/cps)
    end
end

clicking = true
clickLoop()
```

### AFK Detection Bypass

```lua
-- Periodically move mouse slightly to avoid AFK
while true do
    local pos = mouse_get_position()
    local x, y = string.match(pos, "(%d+),(%d+)")
    x, y = tonumber(x), tonumber(y)
    
    mouse_move(x + math.random(-1, 1), y + math.random(-1, 1))
    wait(30)  -- Every 30 seconds
end
```

## HTTP & API Integration

### JSON Parsing (Basic)

```lua
-- Simple JSON-like parsing
local response = httpget("https://jsonplaceholder.typicode.com/users/1")

-- Extract name (very basic parsing)
local name = string.match(response, '"name":"([^"]+)"')
print("User: " .. tostring(name))
```

### API Rate Limiting

```lua
local lastRequest = 0
local minInterval = 1  -- seconds between requests

local function throttledGet(url)
    local now = tick()
    if now - lastRequest < minInterval then
        wait(minInterval - (now - lastRequest))
    end
    lastRequest = tick()
    return httpget(url)
end

-- Use throttled function
for i = 1, 5 do
    print(throttledGet("https://api.example.com/data"))
    wait(2)
end
```

### Webhook Integration

```lua
-- Discord webhook example (basic)
local function sendWebhook(content)
    local json = string.format('{"content": "%s"}', content)
    -- Note: Full webhook support requires POST
    httpget("https://httpbin.org/post?data=" .. http://URLEncode(json))
end
```

## Performance Optimization

### Drawing Batching

```lua
-- Bad: Clear every frame
while true do
    draw_clear()  -- Too expensive
    draw_rect(100, 100, 50, 50, 255, 0, 0, 255, 1, 1)
    wait(1/60)
end

-- Good: Batch updates
local x, y = 100, 100
while true do
    x = x + 1
    if x > 500 then x = 100 end
    
    draw_clear()
    draw_rect(x, y, 50, 50, 255, 0, 0, 255, 1, 1)
    wait(1/60)
end
```

### Conditional Updates

```lua
-- Only redraw when needed
local lastState = nil

while true do
    local player = game:GetService("Players").LocalPlayer
    local state = player and player.Character and player.Character:FindFirstChild("Humanoid")
    
    if state ~= lastState then
        draw_clear()
        if state then
            draw_text(10, 10, 0, 255, 0, 255, "Character: " .. state.Parent.Name)
        else
            draw_text(10, 10, 255, 0, 0, 255, "No Character")
        end
        lastState = state
    end
    
    wait(0.5)
end
```

### Memory Management

```lua
-- Avoid creating new strings in loops
local buffer = ""

-- Good: Build once
for i = 1, 100 do
    buffer = buffer .. tostring(i) .. ","
end

-- Better: Use table.concat
local parts = {}
for i = 1, 100 do
    table.insert(parts, tostring(i))
end
local result = table.concat(parts, ",")
```

## Error Recovery

### Robust Script Template

```lua
local function safeCall(fn, ...)
    local args = {...}
    local success, result = pcall(fn, unpack(args))
    if not success then
        print("Error: " .. tostring(result))
        return nil
    end
    return result
end

-- Usage
safeCall(function()
    local tool = game:GetService("Players").LocalPlayer.Backpack:FindFirstChild("Tool")
    equiptool(tool)
end)
```

### Graceful Degradation

```lua
local function tryEquip()
    local success, err = pcall(function()
        local player = game:GetService("Players").LocalPlayer
        local backpack = player:WaitForChild("Backpack", 5)
        if not backpack then return end
        
        local tool = backpack:FindFirstChild("Sword")
        if tool then
            equiptool(tool)
        end
    end)
    
    if not success then
        print("Equip failed: " .. tostring(err))
    end
end
```

### State Machines

```lua
local State = {
    IDLE = 1,
    EQUIPPING = 2,
    ACTIVATING = 3,
    DONE = 4
}

local currentState = State.IDLE

while currentState ~= State.DONE do
    if currentState == State.IDLE then
        local backpack = game:GetService("Players").LocalPlayer:WaitForChild("Backpack")
        local tool = backpack:FindFirstChild("Sword")
        if tool then
            equiptool(tool)
            currentState = State.EQUIPPING
        end
    elseif currentState == State.EQUIPPING then
        wait(0.1)
        activatetool(tool)
        currentState = State.ACTIVATING
    elseif currentState == State.ACTIVATING then
        currentState = State.DONE
    end
end
```

## Security Considerations

### Anti-Detection

```lua
-- Randomize timing to avoid pattern detection
local function randomDelay(min, max)
    wait(min + math.random() * (max - min))
end

-- Use instead of fixed delays
for i = 1, 10 do
    activatetool(tool)
    randomDelay(0.05, 0.15)  -- Random delay between 50-150ms
end
```

### Safe String Handling

```lua
-- Sanitize user input
local function sanitize(str)
    return string.gsub(str, "[^%w%s]", "")
end

local userInput = "'; drop table users; --"
local safe = sanitize(userInput)
print(safe)  -- Only safe characters remain
```

## Debugging Advanced Scripts

### Stack Traces

```lua
local function deepTrace(level)
    level = level or 2
    local info = debug.getinfo(level)
    if info then
        print(string.format("%s:%d in %s", info.short_src, info.currentline, info.name or "unnamed"))
        if info.next then
            deepTrace(level + 1)
        end
    end
end
```

### Performance Profiling

```lua
local function profile(fn)
    local start = tick()
    fn()
    local elapsed = tick() - start
    print("Execution time: " .. elapsed .. "ms")
end

profile(function()
    -- Your code here
    for i = 1, 1000 do
        draw_rect(i, i, 10, 10, 255, 0, 0, 255, 1, 1)
    end
end)
```

## Next Steps

- Review the [Functions Reference](functions.md) for complete function documentation
- Explore [Internals Documentation](../internals/index.md) to understand how the executor works
- Study the source code in `ptoria/scriptservice.cpp` to see how functions are implemented
