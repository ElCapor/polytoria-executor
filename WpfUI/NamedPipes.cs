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
        public static string luapipename = "wowiezz_pipe";

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
    }
}
