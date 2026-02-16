#include <cheat/pipe.h>
#include <ptoria/scriptservice.h>
#include <ptoria/scriptinstance.h>
#include <thread>
#include <atomic>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>

// +--------------------------------------------------------+
// |                       Variables                        |
// +--------------------------------------------------------+
static std::atomic<bool> g_PipeServerRunning(false);
static std::atomic<bool> g_PipeServerShouldStop(false);
static std::thread g_PipeServerThread;
static HANDLE g_hStopEvent = nullptr;

// +--------------------------------------------------------+
// |                   Pipe Server Thread                   |
// +--------------------------------------------------------+
static void PipeServerThreadFunc()
{
    spdlog::info("Pipe server thread started");
    
    while (!g_PipeServerShouldStop.load())
    {
        // Create the named pipe
        HANDLE hPipe = CreateNamedPipeA(
            NAMED_PIPE_NAME,
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,                          // Max instances
            PIPE_BUFFER_SIZE,           // Output buffer size
            PIPE_BUFFER_SIZE,           // Input buffer size
            0,                          // Default timeout
            nullptr                     // Default security
        );
        
        if (hPipe == INVALID_HANDLE_VALUE)
        {
            spdlog::error("Failed to create named pipe: {}", GetLastError());
            
            // Wait a bit before retrying
            for (int i = 0; i < 50 && !g_PipeServerShouldStop.load(); ++i)
            {
                Sleep(100);
            }
            continue;
        }
        
        // Set up overlapped structure for async connect
        OVERLAPPED overlapped = {};
        overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        
        if (overlapped.hEvent == nullptr)
        {
            spdlog::error("Failed to create overlapped event: {}", GetLastError());
            CloseHandle(hPipe);
            continue;
        }
        
        // Wait for client connection (async)
        BOOL connected = ConnectNamedPipe(hPipe, &overlapped);
        
        if (!connected)
        {
            DWORD error = GetLastError();
            
            if (error == ERROR_IO_PENDING)
            {
                // Wait for connection or stop signal
                HANDLE waitHandles[] = { overlapped.hEvent, g_hStopEvent };
                DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
                
                if (waitResult == WAIT_OBJECT_0 + 1)
                {
                    // Stop event signaled
                    CloseHandle(overlapped.hEvent);
                    CloseHandle(hPipe);
                    break;
                }
                else if (waitResult == WAIT_OBJECT_0)
                {
                    // Client connected
                    DWORD bytesTransferred;
                    if (!GetOverlappedResult(hPipe, &overlapped, &bytesTransferred, FALSE))
                    {
                        spdlog::error("GetOverlappedResult failed: {}", GetLastError());
                        CloseHandle(overlapped.hEvent);
                        CloseHandle(hPipe);
                        continue;
                    }
                }
                else
                {
                    spdlog::error("WaitForMultipleObjects failed: {}", GetLastError());
                    CloseHandle(overlapped.hEvent);
                    CloseHandle(hPipe);
                    continue;
                }
            }
            else if (error == ERROR_PIPE_CONNECTED)
            {
                // Client already connected
            }
            else
            {
                spdlog::error("ConnectNamedPipe failed: {}", error);
                CloseHandle(overlapped.hEvent);
                CloseHandle(hPipe);
                continue;
            }
        }
        
        CloseHandle(overlapped.hEvent);
        
        spdlog::info("Client connected to pipe");
        
        // Read data from the pipe
        std::vector<char> buffer;
        char tempBuffer[4096];
        DWORD bytesRead;
        BOOL success;
        
        do
        {
            success = ReadFile(
                hPipe,
                tempBuffer,
                sizeof(tempBuffer) - 1,
                &bytesRead,
                nullptr
            );
            
            if (success && bytesRead > 0)
            {
                buffer.insert(buffer.end(), tempBuffer, tempBuffer + bytesRead);
            }
            
            // Check if there's more data
            DWORD bytesAvailable = 0;
            PeekNamedPipe(hPipe, nullptr, 0, nullptr, &bytesAvailable, nullptr);
            
            if (bytesAvailable == 0)
                break;
                
        } while (success && !g_PipeServerShouldStop.load());
        
        if (!buffer.empty())
        {
            // Null-terminate the script
            buffer.push_back('\0');
            
            std::string script(buffer.data());
            
            spdlog::info("Received script via pipe ({} bytes)", script.length());
            
            // Execute the script
            try
            {
                ScriptService::RunScript<ScriptInstance>(script);
                spdlog::info("Script executed successfully");
            }
            catch (const std::exception& e)
            {
                spdlog::error("Script execution error: {}", e.what());
            }
        }
        
        // Flush and disconnect
        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
        
        spdlog::info("Client disconnected from pipe");
    }
    
    spdlog::info("Pipe server thread stopped");
}

// +--------------------------------------------------------+
// |                   Public Functions                     |
// +--------------------------------------------------------+
void StartPipeServer()
{
    if (g_PipeServerRunning.load())
    {
        spdlog::warn("Pipe server already running");
        return;
    }
    
    g_PipeServerShouldStop.store(false);
    g_hStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    
    if (g_hStopEvent == nullptr)
    {
        spdlog::error("Failed to create stop event: {}", GetLastError());
        return;
    }
    
    g_PipeServerThread = std::thread(PipeServerThreadFunc);
    g_PipeServerRunning.store(true);
    
    spdlog::info("Pipe server started on {}", NAMED_PIPE_NAME);
}

void StopPipeServer()
{
    if (!g_PipeServerRunning.load())
    {
        return;
    }
    
    spdlog::info("Stopping pipe server...");
    
    g_PipeServerShouldStop.store(true);
    
    // Signal the stop event to wake up the pipe thread
    if (g_hStopEvent != nullptr)
    {
        SetEvent(g_hStopEvent);
    }
    
    // Wait for the thread to finish
    if (g_PipeServerThread.joinable())
    {
        g_PipeServerThread.join();
    }
    
    // Clean up
    if (g_hStopEvent != nullptr)
    {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = nullptr;
    }
    
    g_PipeServerRunning.store(false);
    
    spdlog::info("Pipe server stopped");
}

bool IsPipeServerRunning()
{
    return g_PipeServerRunning.load();
}
