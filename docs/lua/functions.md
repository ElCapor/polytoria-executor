# Lua Functions Reference :material-api:

Complete reference for all custom Lua functions available in the Polytoria Executor.

The custom functions are categorized into specific modules to help you find what you need.

---

## :material-cog: [Core Functions](functions/core.md)
Functions for script execution, environment management, and identification.

## :material-tools: [Tool Functions](functions/tool.md)
Everything related to tool management, equipping, and activation.

## :material-chat: [Interaction Functions](functions/chat.md)
Chat and other game-world interaction tools.

## :material-mouse: [Input Functions](functions/mouse.md)
Direct mouse simulation and cursor control, including the `Input` singleton.

## :material-format-paint: [Drawing Functions](functions/drawing.md)
A powerful overlay drawing library for HUDs and visual aids.

## :material-lan: [Networking Functions](functions/network.md)
Functions for interacting with networked game objects and `NetMessage`.

## :material-web: [HTTP Functions](functions/http.md)
Tools for external communication via web requests.

---

## Complete Example Script

```lua
-- Complete example demonstrating multiple functions

-- Send a chat message
sendchat("Executor loaded!")

-- Draw an overlay using Rendered event
game.Rendered:Connect(function(dt)
    draw_clear()
    draw_rect(50, 50, 200, 100, 0, 128, 255, 200, 2, 1)
    draw_text(60, 70, 255, 255, 255, 255, "Polytoria Executor")
end)

-- Equip a tool
local player = game["Players"].LocalPlayer
local backpack = player:FindChild("Backpack")
local tool = backpack:FindChild("Sword")

if tool then
    equiptool(tool)
    wait(0.5)
    activatetool(tool)
end

-- HTTP request example
local response = httpget("https://api.ipify.org?format=json")
print("Response: " .. response)
```

## Error Handling

!!! bug "Error Handling"
    All functions return error messages as strings when invalid arguments are provided. Always check return types during development.

```lua
-- Invalid arguments
local result = equiptool("not a tool")
if type(result) == "string" then
    print("Error: " .. result)
end
```
