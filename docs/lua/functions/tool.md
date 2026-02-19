# Tool Functions :material-tools:

Functions related to managing and interacting with tools within the game world.

---

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
local player = game["Players"].LocalPlayer
local backpack = player:FindChild("Backpack")
local tool = backpack:FindChild("MyTool")
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
local player = game["Players"].LocalPlayer
local backpack = player:FindChild("Backpack")
local tool = backpack:FindChild("MyTool")
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
local player = game["Players"].LocalPlayer
local backpack = player:FindChild("Backpack")
local tool = backpack:FindChild("Sword")
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
local Players = game["Players"]
local player = Players:FindChild("PlayerName")
local tool = game["Environment"]:FindChild("Tool")

serverequiptool(player, tool)
```
