# Core Functions :material-cog:

Core Lua functions providing basic execution and identification tools.

---

### `loadstring`

!!! warning "Arbitrary Code Execution"
    `loadstring` allows execution of any dynamic Lua string. Never execute code from untrusted sources or unencrypted network sources!

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
loadstring("game['Environment']:ClearAllChildren()")

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
