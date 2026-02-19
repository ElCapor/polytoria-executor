# Interaction Functions :material-chat:

Functions related to in-game communication and interaction.

---

### `sendchat`

Sends a chat message to the game chat.

```lua
sendchat(message)
```

**Parameters:**
- `message` (string): Message to send

**Example:**
```lua
-- Normal message
sendchat("Hello from PolyHack!")

-- Using newlines to fake broadcast messages
sendchat("\n\n\n\n[System]: Server shutdown in 5 minutes.")
```
