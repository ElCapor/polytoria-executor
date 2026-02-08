using System;
using System.IO;
using System.IO.Pipes;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows;

namespace PolyHack
{
    class NamedPipes
    {
        public static string luapipename = "PolytoriaPipe";

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool WaitNamedPipe(string name, int timeout);

        public static bool NamedPipeExist(string pipeName)
        {
            try
            {
                if (!WaitNamedPipe($"\\\\.\\pipe\\{pipeName}", 0))
                {
                    int lastWin32Error = Marshal.GetLastWin32Error();
                    if (lastWin32Error == 0 || lastWin32Error == 2)
                    {
                        return false;
                    }
                }
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }

        public static void LuaPipe(string script)
        {
            if (NamedPipeExist(luapipename))
            {
                new Thread(() =>
                {
                    try
                    {
                        using (NamedPipeClientStream namedPipeClientStream = new NamedPipeClientStream(".", luapipename, PipeDirection.Out))
                        {
                            namedPipeClientStream.Connect(2000);
                            using (StreamWriter streamWriter = new StreamWriter(namedPipeClientStream, System.Text.Encoding.Default, 999999))
                            {
                                streamWriter.Write(script);
                                streamWriter.Flush();
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        Application.Current.Dispatcher.Invoke(() => 
                        {
                            MessageBox.Show(ex.Message, "Pipe Error", MessageBoxButton.OK, MessageBoxImage.Error);
                        });
                    }
                }).Start();
            }
            else
            {
                MessageBox.Show("Please inject before executing!", "Error", MessageBoxButton.OK, MessageBoxImage.Exclamation);
            }
        }

        public static string Request(string command)
        {
            if (!NamedPipeExist(luapipename)) return string.Empty;

            try
            {
                using (NamedPipeClientStream pipeClient = new NamedPipeClientStream(".", luapipename, PipeDirection.InOut))
                {
                    pipeClient.Connect(2000);
                    
                    // Send command
                    using (StreamWriter sw = new StreamWriter(pipeClient))
                    {
                        sw.AutoFlush = true;
                        sw.WriteLine("__REQ:" + command);
                        
                        // Read response
                        using (StreamReader sr = new StreamReader(pipeClient))
                        {
                            return sr.ReadToEnd();
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Pipe Request Error: " + ex.Message);
                return string.Empty;
            }
        }
    }
}
