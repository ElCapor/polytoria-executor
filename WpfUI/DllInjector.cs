using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace PolyHack
{
    public static class DllInjector
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr OpenProcess(uint dwDesiredAccess, bool bInheritHandle, int dwProcessId);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out uint lpNumberOfBytesWritten);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GetModuleHandle(string lpModuleName);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, IntPtr lpThreadId);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool CloseHandle(IntPtr hObject);

        private const uint PROCESS_ALL_ACCESS = 0x000F0000 | 0x00100000 | 0xFFFF;
        private const uint MEM_COMMIT = 0x1000;
        private const uint MEM_RESERVE = 0x2000;
        private const uint PAGE_READWRITE = 0x40;

        public static bool Inject(string processName, string dllPath)
        {
            if (!File.Exists(dllPath))
                return false;

            Process[] processes = Process.GetProcessesByName(processName);
            if (processes.Length == 0)
                return false;

            Process targetProcess = processes[0];
            IntPtr hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, targetProcess.Id);

            if (hProcess == IntPtr.Zero)
                return false;

            IntPtr lpAddress = VirtualAllocEx(hProcess, IntPtr.Zero, (uint)((dllPath.Length + 1) * Marshal.SizeOf(typeof(char))), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            if (lpAddress == IntPtr.Zero)
            {
                CloseHandle(hProcess);
                return false;
            }

            byte[] bytes = Encoding.Default.GetBytes(dllPath);
            if (!WriteProcessMemory(hProcess, lpAddress, bytes, (uint)bytes.Length, out _))
            {
                CloseHandle(hProcess);
                return false;
            }

            IntPtr loadLibraryAddr = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
            if (loadLibraryAddr == IntPtr.Zero)
            {
                CloseHandle(hProcess);
                return false;
            }

            IntPtr hThread = CreateRemoteThread(hProcess, IntPtr.Zero, 0, loadLibraryAddr, lpAddress, 0, IntPtr.Zero);
            if (hThread == IntPtr.Zero)
            {
                CloseHandle(hProcess);
                return false;
            }

            CloseHandle(hThread);
            CloseHandle(hProcess);

            return true;
        }
    }
}
