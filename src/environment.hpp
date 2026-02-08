#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <string>
#include "http.hpp"

namespace Environment {

    // Global storage for HTTP response (used to pass data between C++ and Lua)
    inline std::string g_LastHttpResponse;
    inline std::string g_LastHttpUrl;
    
    // Perform HTTP GET and store result for Lua access
    inline void DoHttpGet(const std::string& url) {
        g_LastHttpUrl = url;
        g_LastHttpResponse = Http::HttpGet(url);
    }
    
    inline void DoHttpPost(const std::string& url, const std::string& data) {
        g_LastHttpUrl = url;
        g_LastHttpResponse = Http::HttpPost(url, data);
    }

    // Lua environment prefix that sets up custom globals
    // This script is prepended to every user script to add custom functions
    inline std::string GetEnvironmentScript() {
        return R"LUA(
-- PolyExecutor Environment Setup v1.0
-- Custom internal environment for enhanced scripting

if not _POLY_ENVIRONMENT then

    -- Create executor info table as a global
    _EXECUTOR_INFO = {
        Name = "Miisploit",
        Version = "v1.0",
        Author = "Mii"
    }

    -- Environment table stored as global
    _POLY_ENVIRONMENT = {
        ExecutorName = _EXECUTOR_INFO.Name,
        ExecutorVersion = _EXECUTOR_INFO.Version,
        IsExecutor = true,
        Platform = "Windows"
    }

    -- HTTP cache
    local _httpCache = {}

    -- Hook game metatable to add custom properties and methods
    if game then
        local success, err = pcall(function()
            local gameMeta = getmetatable(game)
            if gameMeta then
                local oldIndex = gameMeta.__index
                
                -- Create new __index that handles our custom properties
                gameMeta.__index = function(self, key)
                    -- Custom Environment property
                    if key == "Environment" then
                        return _POLY_ENVIRONMENT
                    -- HttpGet method
                    elseif key == "HttpGet" then
                        return function(_, url)
                            -- Return cached or nil for now
                            if _httpCache[url] then
                                return _httpCache[url]
                            end
                            return nil
                        end
                    -- HttpPost method
                    elseif key == "HttpPost" then
                        return function(_, url, data)
                            return nil
                        end
                    -- HttpGetAsync method
                    elseif key == "HttpGetAsync" then
                        return function(_, url)
                            return coroutine.wrap(function()
                                return game:HttpGet(url)
                            end)
                        end
                    end
                    
                    -- Fall back to original __index
                local result = nil
                if oldIndex then
                    if type(oldIndex) == "function" then
                        result = oldIndex(self, key)
                    elseif type(oldIndex) == "table" then
                        result = oldIndex[key]
                    end
                end
                
                if result ~= nil then
                    return result
                end

                -- Fallback: Look for child with this name (Roblox-style)
                -- Try FindFirstChild if available
                if self.FindFirstChild then
                    -- We must be careful not to infinite loop if FindFirstChild triggers __index
                    -- But oldIndex check above should handle methods
                    local child = self:FindFirstChild(key)
                    if child then return child end
                end

                -- Try iterating GetChildren (slower but safer fallback)
                if self.GetChildren then
                     local children = self:GetChildren()
                     if children and type(children) == "table" then
                         for _, child in pairs(children) do
                             if child.Name == key then
                                 return child
                             end
                         end
                     end
                end

                return nil
            end
            end
        end)
        
        if not success then
            print("[Miisploit] Warning: Could not hook game metatable: " .. tostring(err))
        end
    end

    -- identifyexecutor implementation
    if not identifyexecutor then
        identifyexecutor = function()
            return "Miisploit", "v1.0"
        end
    end
        
    -- debug.getinfo compatibility
        
    -- getfenv/setfenv compatibility (MoonSharp may have these)
    if not getfenv then
        local _envRegistry = setmetatable({}, {__mode = "k"})
        
        getfenv = function(f)
            if type(f) == "number" or type(f) == "nil" then
                -- f=0 or nil means global env
                return _G
            end
            if type(f) == "function" then
                return _envRegistry[f] or _G
            end
            return _G
        end
        
        setfenv = function(f, env)
            if type(f) == "number" then
                -- We can't really set stack env in this shim
                return
            end
            if type(f) == "function" then
                _envRegistry[f] = env
            end
            return f
        end
    end

    -- getrawmetatable - get metatable without __metatable protection
    if not getrawmetatable then
        getrawmetatable = function(obj)
            local mt = debug.getmetatable(obj)
            if not mt then
                 -- Try direct getmetatable if debug one fails or is missing
                 return getmetatable(obj)
            end
            return mt
        end
    end

    -- setreadonly - make a table readonly
    if not setreadonly then
        local _readonlyTables = setmetatable({}, {__mode = "k"})
        
        setreadonly = function(tbl, readonly)
            if type(tbl) ~= "table" then return end
            
            -- If making readonly, we need to proxy it or use __newindex
            if readonly then
                if _readonlyTables[tbl] then return end -- Already readonly
                _readonlyTables[tbl] = true
                
                local mt = getmetatable(tbl) or {}
                local oldNewIndex = mt.__newindex
                
                mt.__newindex = function(t, k, v)
                    if _readonlyTables[t] then
                        error("Attempt to modify a readonly table", 2)
                    end
                    if oldNewIndex then
                       if type(oldNewIndex) == "function" then
                           return oldNewIndex(t, k, v)
                       else
                           oldNewIndex[k] = v
                       end
                    else
                       rawset(t, k, v)
                    end
                end
                setmetatable(tbl, mt)
            else
                _readonlyTables[tbl] = false
            end
        end
        
        isreadonly = function(tbl)
            return _readonlyTables[tbl] == true
        end
    end

    -- Make http global table
    if not http then
        http = {
            get = function(url)
                return game and game:HttpGet(url)
            end,
            post = function(url, data)
                return game and game:HttpPost(url, data)
            end,
            request = function(options)
                if type(options) ~= "table" then return nil end
                local method = options.Method or "GET"
                local url = options.Url
                
                if method == "GET" then
                    return {
                        Success = true,
                        StatusCode = 200,
                        Body = game and game:HttpGet(url) or ""
                    }
                elseif method == "POST" then
                    return {
                        Success = true,
                        StatusCode = 200,
                        Body = game and game:HttpPost(url, options.Body or "") or ""
                    }
                end
                return nil
            end
        }
    end

    -- syn compatibility table
    if not syn then
        syn = {
            request = http.request,
            protect_gui = function() end,
            unprotect_gui = function() end,
            is_cached = function() return false end,
            cache_replace = function() end,
            cache_invalidate = function() end
        }
    end

    -- request function (UNC standard)
    if not request then
        request = http.request
    end

    -- Clipboard & Console API
    function setclipboard(text)
        print("__MII_CMD:CLIPBOARD " .. tostring(text))
    end
    toclipboard = setclipboard

    function rconsoleprint(text)
        print("__MII_CMD:RCON_LOG INFO " .. tostring(text))
    end
    function rconsoleinfo(text)
        print("__MII_CMD:RCON_LOG INFO " .. tostring(text))
    end
    function rconsolewarn(text)
        print("__MII_CMD:RCON_LOG WARN " .. tostring(text))
    end
    function rconsoleerr(text)
        print("__MII_CMD:RCON_LOG ERROR " .. tostring(text))
    end
    function rconsoleclear()
        print("__MII_CMD:RCON_CLEAR")
    end
    function rconsolename(name)
        print("__MII_CMD:RCON_TITLE " .. tostring(name))
    end
    function rconsolecreate()
        print("__MII_CMD:RCON_CREATE")
    end
    function rconsoledestroy()
        print("__MII_CMD:RCON_DESTROY")
    end

    -- loadstring compatibility (returns a function, does not execute)
    local _load = load or loadstring
    loadstring = function(src)
        local loader = _load
        if not loader then
            return nil, "loadstring not available"
        end
        return loader(src)
    end

    -- Decompile stub (not actually implemented - would need bytecode access)
    if not decompile then
        decompile = function(f)
            return "-- Decompilation not available"
        end
    end

    -- Script global (reference to current script instance)
    if not script then
        script = nil  -- Will be set by the game
    end

    -- GetChildren / GetDescendants helpers
    if not GetChildren then
        GetChildren = function(obj)
            if obj and obj.GetChildren then
                return obj:GetChildren()
            end
            return {}
        end
    end

    if not GetDescendants then
        GetDescendants = function(obj)
            if obj and obj.GetDescendants then
                return obj:GetDescendants()
            end
            return {}
        end
    end

    -- newproxy if not available
    if not newproxy then
        newproxy = function(addMetatable)
            local proxy = {}
            if addMetatable then
                setmetatable(proxy, {})
            end
            return proxy
        end
    end

    -- collectgarbage wrapper
    if not collectgarbage then
        collectgarbage = function(opt)
            -- Limited implementation
            return 0
        end
    end

    -- inspect helper for console
    if not inspect then
        inspect = function(obj)
            print("Inspecting: " .. tostring(obj))
            if type(obj) == "table" or type(obj) == "userdata" then
                -- Try iterating using pairs if possible
                local success, err = pcall(function()
                    for k, v in pairs(obj) do
                        print("  [" .. tostring(k) .. "] = " .. tostring(v))
                    end
                end)
                if not success then
                    -- If pairs fails (some userdata), try getmetatable
                    print("  Cannot iterate directly (userdata?). Trying methods...")
                     -- Try to list methods via DumpGlobals logic or just print meta
                    local mt = getmetatable(obj)
                    if mt then
                        print("  Metatable found.")
                        if mt.__index and type(mt.__index) == "table" then
                             for k,v in pairs(mt.__index) do
                                 if type(v) == "function" then
                                     print("  (Method) " .. tostring(k))
                                 else
                                     print("  (Prop) " .. tostring(k) .. " = " .. tostring(v))
                                 end
                             end
                        end
                    else
                        print("  No metatable.")
                    end
                    
                    -- Try GetChildren if available
                    if obj.GetChildren then
                         local children = obj:GetChildren()
                         print("  Children: " .. #children)
                         for _, c in pairs(children) do
                             print("    - " .. tostring(c))
                         end
                    end
                end
            else
                print("  Value: " .. tostring(obj))
            end
        end
    end

    -- hierarchy helper (dumps tree)
    if not hierarchy then
        hierarchy = function(root)
            root = root or game
            print("Dumping Hierarchy for: " .. tostring(root))
            
            local function dump(obj, indent)
                if not obj then return end
                print(indent .. "- " .. tostring(obj))
                
                if obj.GetChildren then
                    local children = obj:GetChildren()
                    if children then
                        for _, child in pairs(children) do
                            dump(child, indent .. "  ")
                        end
                    end
                end
            end
            
            dump(root, "")
        end
    end
    
    -- get helper (get object by path string)
    if not get then
        get = function(path)
            local current = game
            for part in string.gmatch(path, "[^%.]+") do
                if current.FindFirstChild then
                     local found = current:FindFirstChild(part)
                     if found then
                         current = found
                     else
                         print("Not found: " .. part)
                         return nil
                     end
                else
                     return nil
                end
            end
            return current
        end
    end

end -- End of _POLY_ENVIRONMENT check

-- End of environment setup
-- User script begins below:
)LUA";
    }
    
    // Create full script with environment prefix
    inline std::string PrepareScript(const std::string& userScript) {
        return GetEnvironmentScript() + "\n" + userScript;
    }
}

#endif // ENVIRONMENT_HPP
