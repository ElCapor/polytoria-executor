using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Win32;
using System.IO;
using System.Text.Json;
using System.Collections.Generic;
using System.Diagnostics;

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
        private bool isDLL = true; // old placeholder for melon loader executor

        public MainWindow()
        {
            InitializeComponent();
            InitWV2();
        }

        private async void InitWV2()
        {
            await wv2.EnsureCoreWebView2Async(null);

            string sitePath = System.IO.Path.Combine(monacoPath, "index.html");
            wv2.Source = new Uri(sitePath);

            wv2.NavigationCompleted += async (s, e) =>
            {
                if (e.IsSuccess)
                {
                    await SetupIntellisense();
                }
            };
        }

        private async Task SetupIntellisense()
        {
            await AddIntellisenseFromFile("globalf.txt", "Function");
            await AddIntellisenseFromFile("globalv.txt", "Variable");
            await AddIntellisenseFromFile("globalns.txt", "Class");
            await AddIntellisenseFromFile("classfunc.txt", "Method");
            await AddIntellisenseFromFile("base.txt", "Keyword");

            // Default examples
            await AddIntellisense("print", "Function", "Standard Lua print", "print($1)");
            await AddIntellisense("task.wait", "Method", "Yields execution", "task.wait($1)");
        }

        private async Task AddIntellisenseFromFile(string fileName, string kind)
        {
            string path = System.IO.Path.Combine(monacoPath, fileName);
            if (File.Exists(path))
            {
                foreach (string text in File.ReadLines(path))
                {
                    if (!string.IsNullOrWhiteSpace(text))
                        await AddIntellisense(text.Trim(), kind, "", text.Trim());
                }
            }
        }

        public async Task AddIntellisense(string label, string kind, string detail, string insertText)
        {
            string script = $"AddIntellisense({JsonSerializer.Serialize(label)}, {JsonSerializer.Serialize(kind)}, {JsonSerializer.Serialize(detail)}, {JsonSerializer.Serialize(insertText)})";
            await wv2.ExecuteScriptAsync(script);
        }

        private async void Load_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "Lua files (*.lua)|*.lua|Text files (*.txt)|*.txt|All files (*.*)|*.*";
            if (openFileDialog.ShowDialog() == true)
            {
                string content = File.ReadAllText(openFileDialog.FileName);
                string escapedContent = JsonSerializer.Serialize(content);
                await wv2.ExecuteScriptAsync($"SetText({escapedContent})");
            }
        }

        private async void Save_Click(object sender, RoutedEventArgs e)
        {
            string contentJson = await wv2.ExecuteScriptAsync("GetText()");
            string content = JsonSerializer.Deserialize<string>(contentJson);

            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "Lua files (*.lua)|*.lua|Text files (*.txt)|*.txt|All files (*.*)|*.*";
            if (saveFileDialog.ShowDialog() == true)
            {
                File.WriteAllText(saveFileDialog.FileName, content);
            }
        }

        private void Inject_Click(object sender, RoutedEventArgs e)
        {
            if (isDLL)
            {
                string dllPath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "wowiezz.dll");
                if (DllInjector.Inject("Polytoria Client", dllPath))
                {
                    MessageBox.Show("Inject Success!", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
                    MainTabControl.SelectedItem = AboutTab;
                }
                else
                {
                    MessageBox.Show("Failed to inject wowiezz.dll into Polytoria Client!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                return;
            }

            Process[] processes = Process.GetProcessesByName("Polytoria");
            if (processes.Length == 0)
            {
                // Try alternate name if Polytoria is not found
                processes = Process.GetProcessesByName("Polytoria Client");
            }

            if (processes.Length > 0)
            {
                MessageBox.Show("Inject Success!", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
                MainTabControl.SelectedItem = AboutTab;
            }
            else
            {
                MessageBox.Show("Polytoria.exe not found!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private async void Execute_Click(object sender, RoutedEventArgs e)
        {
            string contentJson = await wv2.ExecuteScriptAsync("GetText()");
            try
            {
                string script = JsonSerializer.Deserialize<string>(contentJson);
                NamedPipes.LuaPipe(script);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Failed to get script content: " + ex.Message);
            }
        }

        private void About_Click(object sender, RoutedEventArgs e)
        {
            MainTabControl.SelectedItem = AboutTab;
        }

        private void BackToEditor_Click(object sender, RoutedEventArgs e)
        {
            MainTabControl.SelectedItem = EditorTab;
        }

        private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
        {
            Process.Start(new ProcessStartInfo(e.Uri.AbsoluteUri) { UseShellExecute = true });
            e.Handled = true;
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            
        }
    }

}