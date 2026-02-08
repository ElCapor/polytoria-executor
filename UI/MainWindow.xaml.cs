using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Navigation;
using Microsoft.Win32;
using System.IO;
using System.Diagnostics;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text.Json;
using Microsoft.Web.WebView2.Core;

namespace PolyHack
{
    public class MonacoGlobal
    {
        public string Label { get; set; } = string.Empty;
        public string Kind { get; set; } = "Function";
        public string InsertText { get; set; } = string.Empty;
        public string Detail { get; set; } = string.Empty;
        public string Documentation { get; set; } = string.Empty;
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private string monacoPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "monaco");
        private const string ScriptsFolderName = "Scripts";
        private readonly string scriptsFolderPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, ScriptsFolderName);
        private readonly string monacoIndexPath;
        private TaskCompletionSource<bool> monacoReadyTcs = new TaskCompletionSource<bool>();

        public MainWindow()
        {
            InitializeComponent();
            monacoIndexPath = System.IO.Path.Combine(monacoPath, "index.html");
            EnsureScriptsFolder();
            LoadScriptsTree();
            Loaded += MainWindow_Loaded;
            Log("Miisploit v1.0 initialized.");
        }

        private async void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            await InitializeMonacoAsync();
        }

        private void Log(string message)
        {
            Application.Current.Dispatcher.Invoke(() =>
            {
                OutputLog.Items.Add($"[{DateTime.Now:HH:mm:ss}] {message}");
                OutputLog.ScrollIntoView(OutputLog.Items[OutputLog.Items.Count - 1]);
            });
        }

        private async void Clear_Click(object sender, RoutedEventArgs e)
        {
            await SetEditorTextAsync(string.Empty);
            Log("Script area cleared.");
        }

        private async void Load_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "Lua files (*.lua)|*.lua|Text files (*.txt)|*.txt|All files (*.*)|*.*";
            if (openFileDialog.ShowDialog() == true)
            {
                await OpenScriptFileAsync(openFileDialog.FileName);
                Log($"Loaded script: {openFileDialog.SafeFileName}");
            }
        }

        private string? currentScriptPath;

        private async void Save_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrWhiteSpace(currentScriptPath))
            {
                SaveFileDialog saveFileDialog = new SaveFileDialog();
                saveFileDialog.Filter = "Lua files (*.lua)|*.lua|Text files (*.txt)|*.txt|All files (*.*)|*.*";
                if (saveFileDialog.ShowDialog() == true)
                {
                    string content = await GetEditorTextAsync();
                    File.WriteAllText(saveFileDialog.FileName, content);
                    currentScriptPath = saveFileDialog.FileName;
                    Log($"Saved script: {saveFileDialog.SafeFileName}");
                }
            }
            else
            {
                string content = await GetEditorTextAsync();
                File.WriteAllText(currentScriptPath, content);
                Log($"Saved script: {System.IO.Path.GetFileName(currentScriptPath)}");
            }
        }

        private void Inject_Click(object sender, RoutedEventArgs e)
        {
            string dllPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "poly.dll");
            Log("Injecting poly.dll...");
            if (DllInjector.Inject("Polytoria Client", dllPath))
            {
                Log("Injection successful!");
                MessageBox.Show("Inject Success!", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
            }
            else
            {
                Log("Injection failed.");
                MessageBox.Show("Failed to inject!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private async void Execute_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string script = await GetEditorTextAsync();
                Log("Executing Lua script...");
                NamedPipes.LuaPipe(script);
            }
            catch (Exception ex)
            {
                Log("Execution Error: " + ex.Message);
            }
        }

        // --- SCRIPTS LOGIC ---

        private void EnsureScriptsFolder()
        {
            if (!Directory.Exists(scriptsFolderPath))
            {
                Directory.CreateDirectory(scriptsFolderPath);
            }
        }

        private void LoadScriptsTree()
        {
            EnsureScriptsFolder();
            ScriptsTree.ItemsSource = BuildScriptNodes(scriptsFolderPath);
        }

        private ObservableCollection<ScriptNode> BuildScriptNodes(string rootPath)
        {
            var nodes = new ObservableCollection<ScriptNode>();
            var dirInfo = new DirectoryInfo(rootPath);

            foreach (var dir in dirInfo.GetDirectories().OrderBy(d => d.Name))
            {
                var node = new ScriptNode(dir.Name, dir.FullName, true)
                {
                    Children = BuildScriptNodes(dir.FullName)
                };
                nodes.Add(node);
            }

            foreach (var file in dirInfo.GetFiles().OrderBy(f => f.Name))
            {
                if (!IsScriptFile(file.FullName)) continue;
                nodes.Add(new ScriptNode(file.Name, file.FullName, false));
            }

            return nodes;
        }

        private bool IsScriptFile(string path)
        {
            string ext = System.IO.Path.GetExtension(path).ToLowerInvariant();
            return ext == ".lua" || ext == ".txt";
        }

        private void RefreshScripts_Click(object sender, RoutedEventArgs e)
        {
            LoadScriptsTree();
            Log("Scripts refreshed.");
        }

        private void OpenScriptsFolder_Click(object sender, RoutedEventArgs e)
        {
            EnsureScriptsFolder();
            Process.Start(new ProcessStartInfo
            {
                FileName = scriptsFolderPath,
                UseShellExecute = true
            });
        }

        private async void ScriptsTree_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (e.NewValue is ScriptNode node && !node.IsDirectory)
            {
                await OpenScriptFileAsync(node.FullPath);
            }
        }

        // --- EDITOR (MONACO) ---

        private async Task InitializeMonacoAsync()
        {
            if (!File.Exists(monacoIndexPath))
            {
                Log("Monaco editor files not found.");
                if (!monacoReadyTcs.Task.IsCompleted)
                {
                    monacoReadyTcs.TrySetResult(false);
                }
                return;
            }

            if (MonacoView.CoreWebView2 == null)
            {
                await MonacoView.EnsureCoreWebView2Async();
                var core = MonacoView.CoreWebView2;
                if (core != null)
                {
                    core.Settings.AreDefaultContextMenusEnabled = false;
                    core.Settings.AreDevToolsEnabled = false;
                    core.Settings.IsStatusBarEnabled = false;
                }
                MonacoView.NavigationCompleted += MonacoView_NavigationCompleted;
            }

            MonacoView.Source = new Uri(monacoIndexPath, UriKind.Absolute);
        }

        private void MonacoView_NavigationCompleted(object? sender, CoreWebView2NavigationCompletedEventArgs e)
        {
            if (!monacoReadyTcs.Task.IsCompleted)
            {
                monacoReadyTcs.TrySetResult(e.IsSuccess);
            }
        }

        private async Task<bool> EnsureMonacoReadyAsync()
        {
            bool ok = await monacoReadyTcs.Task;
            return ok && MonacoView.CoreWebView2 != null;
        }

        private async Task<string> GetEditorTextAsync()
        {
            if (!await EnsureMonacoReadyAsync())
            {
                return string.Empty;
            }
            string json = await MonacoView.ExecuteScriptAsync("GetText()");
            return JsonSerializer.Deserialize<string>(json) ?? string.Empty;
        }

        private async Task SetEditorTextAsync(string text)
        {
            if (!await EnsureMonacoReadyAsync())
            {
                return;
            }
            string payload = JsonSerializer.Serialize(text);
            await MonacoView.ExecuteScriptAsync($"SetText({payload});");
        }

        private async Task OpenScriptFileAsync(string path)
        {
            string content = File.ReadAllText(path);
            await SetEditorTextAsync(content);
            currentScriptPath = path;
        }

        private void Nav_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button btn)
            {
                string header = btn.Tag?.ToString() ?? btn.Content?.ToString() ?? string.Empty;
                foreach (TabItem item in MainTabControl.Items)
                {
                    if (item.Header.ToString() == header)
                    {
                        MainTabControl.SelectedItem = item;
                        break;
                    }
                }
            }
        }

        // --- CMD LOGIC ---

        private void CmdInput_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter) {
                string input = CmdInput.Text.Trim();
                if (string.IsNullOrWhiteSpace(input)) return;

                CmdHistory.Items.Add($"> {input}");
                Log($"CMD: {input}");

                string[] parts = input.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                string cmd = parts[0].ToLower();
                string lua = input;

                switch (cmd)
                {
                    case "cmds":
                        CmdHistory.Items.Add("Available Commands:");
                        CmdHistory.Items.Add("- cmds (list commands)");
                        CmdHistory.Items.Add("- health (amount)");
                        CmdHistory.Items.Add("- speed (amount)");
                        CmdHistory.Items.Add("- size (x) (y) (z)");
                        CmdHistory.Items.Add("- invisible (true/false)");
                        CmdHistory.Items.Add("- esp (true/false)");
                        CmdHistory.Items.Add("- noclip (true/false)");
                        CmdHistory.Items.Add("- walkfling (true/false)");
                        CmdHistory.Items.Add("- crash");
                        lua = "";
                        break;
                    case "health":
                        if (parts.Length > 1)
                            lua = $"game['Players'].LocalPlayer.MaxHealth = {parts[1]}; game['Players'].LocalPlayer.Health = {parts[1]}";
                        break;
                    case "speed":
                        if (parts.Length > 1)
                            lua = $"game['Players'].LocalPlayer.WalkSpeed = {parts[1]}";
                        break;
                    case "size":
                        if (parts.Length > 3)
                            lua = $"game['Players'].LocalPlayer.Size = Vector3.new({parts[1]}, {parts[2]}, {parts[3]})";
                        break;
                    case "invisible":
                        if (parts.Length > 1) {
                            bool inv = parts[1].ToLower() == "true" || parts[1] == "1";
                            lua = inv ? "game['Players'].LocalPlayer.Size = Vector3.new(0, 1, 0)" : "game['Players'].LocalPlayer.Size = Vector3.new(1, 1, 1)";
                        }
                        break;
                    case "noclip":
                        bool nc = parts.Length > 1 && (parts[1].ToLower() == "true" || parts[1] == "1");
                        lua = nc ? "local p = game['Players'].LocalPlayer; if p and p.Character then for _,v in pairs(p.Character:GetChildren()) do if v.CanCollide ~= nil then v.CanCollide = false end end end" : "local p = game['Players'].LocalPlayer; if p and p.Character then for _,v in pairs(p.Character:GetChildren()) do if v.CanCollide ~= nil then v.CanCollide = true end end end";
                        break;
                    case "esp":
                        bool esp = parts.Length > 1 && (parts[1].ToLower() == "true" || parts[1] == "1");
                        if (esp) {
                            lua = @"
_esp_enabled = true
if _esp_render then pcall(function() _esp_render:Disconnect() end) _esp_render = nil end
if _esp_added then pcall(function() _esp_added:Disconnect() end) _esp_added = nil end
if _esp_removed then pcall(function() _esp_removed:Disconnect() end) _esp_removed = nil end

local function getPlayerGUI()
    local lp = game['Players'].LocalPlayer
    if not lp then return nil end
    local gui = nil
    pcall(function() gui = lp:FindFirstChild('PlayerGUI') end)
    if not gui then pcall(function() gui = lp.PlayerGUI end) end
    if not gui then
        pcall(function()
            gui = Instance.New('PlayerGUI')
            gui.Parent = lp
        end)
    end
    return gui
end

local gui = getPlayerGUI()
if not gui then return end

_esp_labels = _esp_labels or {}

local function setPos(ui, x, y)
    local v = Vector2.New(x, y)
    if not pcall(function() ui.PositionOffset = v end) then
        pcall(function() ui.Position = v end)
    end
end

local function setVisible(ui, visible)
    if not pcall(function() ui.Visible = visible end) then
        pcall(function() ui.Opacity = visible and 1 or 0 end)
    end
end

local function vec3(v)
    if not v then return nil, nil, nil end
    return v.x or v.X, v.y or v.Y, v.z or v.Z
end

local function worldToScreen(pos)
    local ok, v = pcall(function() return Input:WorldToScreenPoint(pos) end)
    if ok and v then
        local sx, sy, sz = vec3(v)
        return sx, sy, sz
    end

    ok, v = pcall(function() return Input:WorldToViewportPoint(pos) end)
    if ok and v then
        local vx, vy, vz = vec3(v)
        if vx and vy then
            local screen = nil
            pcall(function() screen = Input:ViewportToScreenPoint(Vector2.New(vx, vy)) end)
            if screen then
                local sx, sy, sz = vec3(screen)
                return sx, sy, vz or sz
            end
            local w = nil
            local h = nil
            pcall(function() w = Input.ScreenWidth end)
            pcall(function() h = Input.ScreenHeight end)
            if w and h then
                return vx * w, vy * h, vz
            end
        end
    end
    return nil, nil, nil
end

local function fixY(y)
    local h = nil
    pcall(function() h = Input.ScreenHeight end)
    if h and y and (y < 0 or y > h) then
        local flipped = h - y
        if flipped >= 0 and flipped <= h then return flipped end
    end
    return y
end

local function getPos(p)
    if p and p.Position then return p.Position end
    local char = nil
    pcall(function() char = p.Character end)
    if char then
        local head = nil
        pcall(function() head = char:FindFirstChild('Head') end)
        if head and head.Position then return head.Position end
        local torso = nil
        pcall(function() torso = char:FindFirstChild('Torso') end)
        if torso and torso.Position then return torso.Position end
        local root = nil
        pcall(function() root = char:FindFirstChild('Root') end)
        if root and root.Position then return root.Position end
    end
    local head = nil
    pcall(function() head = p:FindFirstChild('Head') end)
    if head and head.Position then return head.Position end
    local torso = nil
    pcall(function() torso = p:FindFirstChild('Torso') end)
    if torso and torso.Position then return torso.Position end
    return nil
end

local function ensureLabel(p)
    local key = tostring(p.UserID or p.Name or p)
    local label = _esp_labels[key]
    if not label then
        label = Instance.New('UILabel')
        label.Parent = gui
        pcall(function() label.AutoSize = true end)
        pcall(function() label.TextColor = Color.New(0, 1, 1, 1) end)
        pcall(function() label.FontSize = 14 end)
        pcall(function() label.Text = p.Name or 'Player' end)
        pcall(function() label:ApplyAnchorPreset(AnchorPreset.TopLeft) end)
        _esp_labels[key] = label
    end
    return label, key
end

local function removeLabel(p)
    local key = tostring(p.UserID or p.Name or p)
    local label = _esp_labels[key]
    if label then pcall(function() label:Destroy() end) end
    _esp_labels[key] = nil
end

for _, p in ipairs(game['Players']:GetPlayers()) do
    if p ~= game['Players'].LocalPlayer then
        pcall(function() ensureLabel(p) end)
    end
end

_esp_added = game['Players'].PlayerAdded:Connect(function(p)
    if _esp_enabled then pcall(function() ensureLabel(p) end) end
end)

_esp_removed = game['Players'].PlayerRemoved:Connect(function(p)
    pcall(function() removeLabel(p) end)
end)

_esp_render = game.Rendered:Connect(function()
    if not _esp_enabled then return end
    for _, p in ipairs(game['Players']:GetPlayers()) do
        if p ~= game['Players'].LocalPlayer then
            local pos = getPos(p)
            if pos then
                local sx, sy, sz = worldToScreen(pos)
                sy = fixY(sy)
                local label = ensureLabel(p)
                if sz and sz > 0 then
                    setVisible(label, true)
                    setPos(label, sx, (sy or 0) - 16)
                    pcall(function() label.Text = p.Name or 'Player' end)
                else
                    setVisible(label, false)
                end
            end
        end
    end
end)
";
                        } else {
                            lua = @"
_esp_enabled = false
if _esp_render then pcall(function() _esp_render:Disconnect() end) _esp_render = nil end
if _esp_added then pcall(function() _esp_added:Disconnect() end) _esp_added = nil end
if _esp_removed then pcall(function() _esp_removed:Disconnect() end) _esp_removed = nil end
if _esp_labels then
    for _, label in pairs(_esp_labels) do
        pcall(function() label:Destroy() end)
    end
end
_esp_labels = {}
";
                        }
                        break;
                    case "walkfling":
                        bool wf = parts.Length > 1 && (parts[1].ToLower() == "true" || parts[1] == "1");
                        if (wf) {
                            lua = @"
_walkflinging = true
_update = function(dt)
    if not _walkflinging then _update = nil return end
    local p = game['Players'].LocalPlayer
    if p and p.Character then
        local root = p.Character:FindFirstChild('Torso') or p.Character:FindFirstChild('Root')
        if root then
            -- Noclip part
            for _, v in pairs(p.Character:GetChildren()) do
                if v.CanCollide ~= nil then v.CanCollide = false end
            end
            
            -- Fling part
            local oldVel = root.Velocity
            root.Velocity = oldVel * 1000 + Vector3.new(0, 500, 0)
            -- Polytoria doesn't have RenderStepped wait exactly, so we just toggle
            -- In next update it will be back to normal or boosted again
        end
    end
end";
                        } else {
                            lua = "_walkflinging = false";
                        }
                        break;
                    case "unwalkfling":
                        lua = "_walkflinging = false";
                        break;
                    case "crash":
                        lua = "function _update(dt) game['Players'].LocalPlayer.Health = 1891289812981298912981289 end";
                        break;
                }

                if (!string.IsNullOrEmpty(lua)) {
                    NamedPipes.LuaPipe(lua);
                }
                CmdInput.Clear();
            }
        }

        private void Window_KeyDown(object sender, KeyEventArgs e) { }
        private void About_Click(object sender, RoutedEventArgs e) { }
        private void BackToEditor_Click(object sender, RoutedEventArgs e) { }
        private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e) { }
    }

    public class ScriptNode
    {
        public ScriptNode(string name, string fullPath, bool isDirectory)
        {
            Name = name;
            FullPath = fullPath;
            IsDirectory = isDirectory;
            Children = new ObservableCollection<ScriptNode>();
        }

        public string Name { get; }
        public string FullPath { get; }
        public bool IsDirectory { get; }
        public ObservableCollection<ScriptNode> Children { get; set; }
    }

}
