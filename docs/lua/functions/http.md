# HTTP Functions :material-web:

Functions for making external web requests from within the executor.

---

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
