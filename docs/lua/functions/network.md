# Networking Functions :material-lan:

Functions interacting with the game's networking layer and objects.

---

### `fireclickdetector`

Fires a ClickDetector on any instance.

```lua
fireclickdetector(instance)
```

**Parameters:**
- `instance` (Instance): Instance with ClickDetector

**Example:**
```lua
local part = game["Environment"]:FindChild("ClickPart")
if part and part:FindChild("ClickDetector") then
    fireclickdetector(part)
end
```

---

### `NetMessage` (Singleton)

Used for creating networking packets to send to the server.

```lua
local msg = NetMessage.New()
msg:AddString('type', 'Status')
msg:AddString('Status', "Hello from PolyHack")
msg:AddInt('MaxCustomers', 9999999)

-- Invoke on a NetworkEvent
game["Environment"]["Tycoon 1"]["NetworkEvent"]:InvokeServer(msg)
```

