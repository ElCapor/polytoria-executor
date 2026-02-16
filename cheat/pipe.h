#ifndef PIPE_H
#define PIPE_H

#include <windows.h>

#define NAMED_PIPE_NAME "\\\\.\\pipe\\wowiezz_pipe"
#define PIPE_BUFFER_SIZE 65536

// Start the pipe server in a background thread
void StartPipeServer();

// Stop the pipe server (call when DLL is unloading)
void StopPipeServer();

// Check if pipe server is running
bool IsPipeServerRunning();

#endif /* PIPE_H */
