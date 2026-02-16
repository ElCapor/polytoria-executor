/**
 * @file assert.h
 * @author ElCapor (infosec@yu-tech.cloud)
 * @brief Modern C++23 Windows-only assertion system with MessageBox dialogs
 * @version 0.2
 * @date 2026-02-12
 * 
 * Features:
 * - MessageBox with Abort/Retry/Ignore options
 * - Stack trace capture (C++23 std::stacktrace + Windows API fallback)
 * - CRT assertion hook integration
 * - SEH unhandled exception filter
 * - spdlog logging
 * 
 * CC-BY-SA 4.0 Copyright (c) 2026 @ElCapor
 */
#ifndef NASE_ASSERT_H
#define NASE_ASSERT_H

#include <spdlog/spdlog.h>
#include <string_view>
#include <format>
#include <source_location>
#include <stacktrace>
#include <atomic>
#include <optional>

// Windows-specific headers
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>
#include <crtdbg.h>


namespace nasec::assert {

// Forward declarations
namespace detail {

    /**
     * @brief Action to perform on assertion failure - logs and shows MessageBox
     * 
     * @param message 
     * @param condition 
     * @param file 
     * @param line 
     * @param function 
     * @param stack_trace 
     */
    [[noreturn]] void on_assertion_failure(
        std::string_view message,
        std::string_view condition,
        std::string_view file,
        int line,
        std::string_view function,
        std::string_view stack_trace
    );
    
    /**
     * @brief Capture stack trace as a string (Windows API implementation)
     * 
     * @return std::string 
     */
    std::string capture_stack_trace();

    /**
     * @brief Capture stack trace using Windows API (DbgHelp) - fallback if std::stacktrace is unavailable
     * 
     * @return std::string 
     */
    std::string capture_stack_trace_windows();
    
    /**
     * @brief SEH exception filter callback
     * 
     * @param ep Exception pointers
     * @return LONG Filter result
     */
    LONG WINAPI seh_exception_filter(EXCEPTION_POINTERS* ep);

    /**
     * @brief CRT assertion hook callback
     * 
     * @param reportType Report type (e.g., _CRT_ASSERT)
     * @param userMessage User message
     * @param returnValue Return value pointer
     * @return int 
     */
    int crt_assert_hook(int reportType, char* userMessage, int* returnValue);
}

// ============================================================================
// Configuration
// ============================================================================

struct Config {
    /// Enable/disable the CRT assertion hook
    bool enable_crt_hook = true;
    
    /// Enable/disable the SEH exception filter
    bool enable_seh_filter = true;
    
    /// Show MessageBox on assertion (disable for headless/server)
    bool show_messagebox = true;
    
    /// Log to spdlog
    bool enable_logging = true;
    
    /// Minimum log level for assertions
    spdlog::level::level_enum log_level = spdlog::level::critical;
    
    /// Custom MessageBox title (empty = default)
    std::string_view messagebox_title = "Assertion Failure";
    
    /// Enable DebugBreak on Retry (break into debugger)
    bool enable_debug_break = true;
};

/// Global configuration (can be modified before Install())
inline Config g_config;

// ============================================================================
// Core Assertion Functions
// ============================================================================

/**
 * @brief Core assertion handler - called when assertion fails
 */
inline void assert_handler(
    bool condition,
    std::string_view condition_str,
    std::string_view message,
    const std::source_location& loc = std::source_location::current()
) {
    if (condition) return; // Assertion passed
    
    std::string stack_trace;
    
    // Try C++23 stacktrace first, fallback to Windows API
    if constexpr (requires { std::stacktrace::current(); }) {
        try {
            auto trace = std::stacktrace::current();
            stack_trace = std::to_string(trace);
        } catch (...) {
            stack_trace = detail::capture_stack_trace_windows();
        }
    } else {
        stack_trace = detail::capture_stack_trace_windows();
    }
    
    detail::on_assertion_failure(
        message,
        condition_str,
        loc.file_name(),
        loc.line(),
        loc.function_name(),
        stack_trace
    );
}

// ============================================================================
// Public API
// ============================================================================


inline void Assert(
    std::uintptr_t ptr,
    std::string_view message = "Pointer was nullptr",
    const std::source_location& loc = std::source_location::current()
) {
    assert_handler(ptr != 0, "ptr != nullptr", message, loc);
}

template <typename T>
inline void AssertNotNull(
    T* ptr,
    std::string_view message = "Pointer was nullptr",
    const std::source_location& loc = std::source_location::current()
) {
    assert_handler(ptr != nullptr, "ptr != nullptr", message, loc);
}

/**
 * @brief Simple assertion with message
 * 
 * Usage: nasec::Assert(ptr != nullptr, "Pointer must not be null");
 */
inline void Assert(
    bool condition,
    std::string_view message = "Assertion failed",
    const std::source_location& loc = std::source_location::current()
) {
    assert_handler(condition, "?", message, loc);
}

/**
 * @brief Assertion with formatted message (C++23 std::format)
 * 
 * Usage: nasec::Assert(ptr != nullptr, "Pointer {} was null", ptr_name);
 */
template <typename... Args>
inline void Assert(
    bool condition,
    std::format_string<Args...> format,
    Args&&... args,
    const std::source_location& loc = std::source_location::current()
) {
    if (!condition) {
        auto msg = std::format(format, std::forward<Args>(args)...);
        assert_handler(condition, "?", msg, loc);
    }
}

/**
 * @brief Assertion that shows the condition string
 * 
 * Usage: nasec::AssertVar(ptr != nullptr, ptr);
 */
template <typename T>
inline void AssertVar(
    bool condition,
    const T& value,
    std::string_view message = "Assertion failed",
    const std::source_location& loc = std::source_location::current()
) {
    if (!condition) {
        std::string cond_str = std::format("{} (value: {})", message, value);
        assert_handler(condition, cond_str, message, loc);
    }
}

/**
 * @brief Assertion that throws a custom exception instead of terminating
 * 
 * Usage: nasec::AssertThrow(ptr != nullptr, "Pointer must not be null");
 */
class AssertionException : public std::exception {
public:
    AssertionException(
        std::string_view message,
        std::string_view condition,
        std::string_view file,
        int line,
        std::string_view function
    ) : msg_(std::format(
        "Assertion failed: {}\nCondition: {}\nFile: {}:{} ({})",
        message, condition, file, line, function
    )) {}
    
    const char* what() const noexcept override { return msg_.c_str(); }
    
private:
    std::string msg_;
};

template <typename... Args>
inline void AssertThrow(
    bool condition,
    std::format_string<Args...> format,
    Args&&... args,
    const std::source_location& loc = std::source_location::current()
) {
    if (!condition) {
        auto msg = std::format(format, std::forward<Args>(args)...);
        throw AssertionException(msg, "?", loc.file_name(), loc.line(), loc.function_name());
    }
}

/**
 * @brief Verify assertion - like Assert but continues in release builds
 * 
 * Usage: nasec::Verify(ptr != nullptr, "This should not happen");
 */
inline void Verify(
    bool condition,
    std::string_view message = "Verification failed",
    const std::source_location& loc = std::source_location::current()
) {
#ifdef _DEBUG
    assert_handler(condition, "?", message, loc);
#else
    if (!condition && g_config.enable_logging) {
        spdlog::log(g_config.log_level, "{}:{}: {}: {}",
            loc.file_name(), loc.line(), loc.function_name(), message);
    }
#endif
}

/**
 * @brief Unreachable code assertion
 * 
 * Usage: nasec::Unreachable("This code should never be executed");
 */
[[noreturn]] inline void Unreachable(
    std::string_view message = "Unreachable code reached",
    const std::source_location& loc = std::source_location::current()
) {
    assert_handler(false, "unreachable", message, loc);
}

/**
 * @brief Assume assertion - hints to compiler that condition is true
 * 
 * Usage: nasec::Assume(ptr != nullptr);
 */
#if defined(__GNUC__) || defined(__clang__)
#define nasec_unlikely(condition) __builtin_expect(!!(condition), 0)
#else
#define nasec_unlikely(condition) (condition)
#endif

inline void Assume(
    bool condition,
    std::string_view message = "Assumption violated",
    const std::source_location& loc = std::source_location::current()
) {
    if (nasec_unlikely(!condition)) {
        assert_handler(false, "?", message, loc);
    }
}

// ============================================================================
// Installation
// ============================================================================

/**
 * @brief Install assertion handlers (CRT hook + SEH filter)
 * 
 * Call this once at program startup (e.g., in DllMain or main())
 */
inline void Install(const Config& config = {}) {
    g_config = config;
    
    if (g_config.enable_crt_hook) {
        // Install CRT assertion hook
        _CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, detail::crt_assert_hook);
        // Disable default message box; we'll show our own
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    }
    
    if (g_config.enable_seh_filter) {
        // Install SEH exception filter
        SetUnhandledExceptionFilter(detail::seh_exception_filter);
    }
}

/**
 * @brief Uninstall assertion handlers
 */
inline void Uninstall() {
    _CrtSetReportHook2(_CRT_RPTHOOK_REMOVE, detail::crt_assert_hook);
}

// ============================================================================
// Implementation Details
// ============================================================================

namespace detail {

/**
 * @brief Capture stack trace using Windows API (DbgHelp)
 */
inline std::string capture_stack_trace_windows() {
    std::string result;
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    
    // Initialize symbol handler
    static bool initialized = []() -> bool {
        SymInitialize(GetCurrentProcess(), nullptr, TRUE);
        return true;
    }();
    
    // Capture stack trace
    void* stack[100];
    DWORD hash = 0;
    WORD frames = CaptureStackBackTrace(0, 100, stack, &hash);
    
    // Demangle and format symbols
    char symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(symbol_buffer);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;
    
    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    DWORD displacement = 0;
    
    for (WORD i = 0; i < frames; i++) {
        std::string frame_info;
        
        // Demangle symbol name if available
        DWORD64 address = reinterpret_cast<DWORD64>(stack[i]);
        if (SymFromAddr(process, address, nullptr, symbol)) {
            // Undecorate symbol name
            char und_name[MAX_SYM_NAME];
            if (UnDecorateSymbolName(symbol->Name, und_name, MAX_SYM_NAME, 
                    UNDNAME_COMPLETE | UNDNAME_NO_MS_KEYWORDS)) {
                frame_info = std::format("  {:2}: {} (0x{:016X})", i, und_name, address);
            } else {
                frame_info = std::format("  {:2}: {} (0x{:016X})", i, symbol->Name, address);
            }
        } else {
            frame_info = std::format("  {:2}: 0x{:016X}", i, address);
        }
        
        // Try to get line number
        if (SymGetLineFromAddr64(process, address, &displacement, &line)) {
            frame_info += std::format(" ({}:{})", line.FileName, line.LineNumber);
        }
        
        result += frame_info + "\n";
    }
    
    return result;
}

/**
 * @brief Capture stack trace (C++23 preferred, Windows fallback)
 */
inline std::string capture_stack_trace() {
#if defined(__cpp_lib_stacktrace) && __cpp_lib_stacktrace >= 202011L
    try {
        auto trace = std::stacktrace::current();
        return std::to_string(trace);
    } catch (...) {
        return capture_stack_trace_windows();
    }
#else
    return capture_stack_trace_windows();
#endif
}

/**
 * @brief Format full assertion message
 */
inline std::string format_assertion_message(
    std::string_view message,
    std::string_view condition,
    std::string_view file,
    int line,
    std::string_view function,
    std::string_view stack_trace
) {
    return std::format(
        "===============================================================\n"
        "                    ASSERTION FAILED                            \n"
        "===============================================================\n"
        "\n"
        "  Message:   {}\n"
        "  Condition: {}\n"
        "  Location:  {}:{} ({})\n"
        "\n"
        "===============================================================\n"
        "                       STACK TRACE                              \n"
        "===============================================================\n"
        "\n"
        "{}\n"
        "===============================================================\n"
        "\n"
        "  [ Abort ]  - Terminate the program\n"
        "  [ Retry ]  - Break into debugger (if attached)\n"
        "  [ Ignore ] - Continue execution (may cause undefined behavior)\n"
        "\n",
        message, condition, file, line, function, stack_trace
    );
}

/**
 * @brief Handle assertion failure - logs and shows dialog
 */
[[noreturn]] inline void on_assertion_failure(
    std::string_view message,
    std::string_view condition,
    std::string_view file,
    int line,
    std::string_view function,
    std::string_view stack_trace
) {
    // Format the full message
    std::string full_msg = format_assertion_message(
        message, condition, file, line, function, stack_trace
    );
    
    // Log with spdlog
    if (g_config.enable_logging) {
        spdlog::log(g_config.log_level,
            "ASSERTION FAILED: {} at {}:{} ({})",
            message, file, line, function);
        spdlog::log(g_config.log_level, "Stack trace:\n{}", stack_trace);
    }
    
    // Show MessageBox if enabled
    if (g_config.show_messagebox) {
        // Determine title
        LPCSTR title = g_config.messagebox_title.empty() 
            ? "Assertion Failure" 
            : g_config.messagebox_title.data();
        
        int result = MessageBoxA(
            nullptr,
            full_msg.c_str(),
            title,
            MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON2 | MB_SYSTEMMODAL
        );
        
        switch (result) {
            case IDABORT:
                // Terminate the program
                std::abort();
                
            case IDRETRY:
                // Break into debugger
                if (g_config.enable_debug_break) {
                    DebugBreak();
                }
                // If no debugger, fall through to ignore
                [[fallthrough]];
                
            case IDIGNORE:
                // Continue execution - return to caller
                // This is dangerous but allows recovery in some cases
                return;
        }
    }
    
    // Fallback: terminate
    std::abort();
}

/**
 * @brief CRT assertion hook - intercepts _ASSERT and _ASSERTE
 */
inline int crt_assert_hook(int reportType, char* userMessage, int* returnValue) {
    if (reportType != _CRT_ASSERT) {
        return FALSE; // Not handled, let default handling occur
    }
    
    // Log with spdlog
    if (g_config.enable_logging) {
        spdlog::critical("CRT ASSERTION FAILED:\n{}", userMessage);
    }
    
    // Capture stack trace
    std::string stack_trace = capture_stack_trace();
    
    if (g_config.enable_logging) {
        spdlog::critical("Stack trace:\n{}", stack_trace);
    }
    
    // Show our custom dialog
    std::string full_msg = std::format(
        "CRT Assertion Failed\n\n{}\n\n{}\n\n"
        "[ Abort ]  - Terminate\n"
        "[ Retry ]  - Debug\n"
        "[ Ignore ] - Continue",
        userMessage, stack_trace
    );
    
    int result = MessageBoxA(
        nullptr,
        full_msg.c_str(),
        "CRT Assertion Failure",
        MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON2 | MB_SYSTEMMODAL
    );
    
    switch (result) {
        case IDABORT:
            *returnValue = FALSE; // Signal abort to CRT
            return TRUE;          // We've handled it
            
        case IDRETRY:
            if (g_config.enable_debug_break) {
                DebugBreak();
            }
            *returnValue = FALSE;
            return TRUE;
            
        case IDIGNORE:
            *returnValue = TRUE; // Tell CRT to continue
            return TRUE;
    }
    
    return FALSE;
}

/**
 * @brief SEH exception filter for unhandled exceptions
 */
inline LONG WINAPI seh_exception_filter(EXCEPTION_POINTERS* ep) {
    DWORD code = ep->ExceptionRecord->ExceptionCode;
    
    // Log exception
    if (g_config.enable_logging) {
        spdlog::critical("🔥 UNHANDLED EXCEPTION: Code = 0x{:08X}", code);
    }
    
    // Capture stack trace
    std::string stack_trace = capture_stack_trace_windows();
    
    if (g_config.enable_logging) {
        spdlog::critical("Stack trace:\n{}", stack_trace);
    }
    
    // Map exception code to string
    const char* exception_name = "Unknown";
    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:      exception_name = "Access Violation"; break;
        case EXCEPTION_DATATYPE_MISALIGNMENT: exception_name = "Datatype Misalignment"; break;
        case EXCEPTION_BREAKPOINT:            exception_name = "Breakpoint"; break;
        case EXCEPTION_SINGLE_STEP:           exception_name = "Single Step"; break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: exception_name = "Array Bounds Exceeded"; break;
        case EXCEPTION_FLT_DENORMAL_OPERAND:  exception_name = "Floating Point Denormal"; break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:    exception_name = "Floating Point Divide by Zero"; break;
        case EXCEPTION_FLT_INEXACT_RESULT:    exception_name = "Floating Point Inexact Result"; break;
        case EXCEPTION_FLT_INVALID_OPERATION: exception_name = "Floating Point Invalid Operation"; break;
        case EXCEPTION_FLT_OVERFLOW:          exception_name = "Floating Point Overflow"; break;
        case EXCEPTION_FLT_STACK_CHECK:       exception_name = "Floating Point Stack Check"; break;
        case EXCEPTION_FLT_UNDERFLOW:         exception_name = "Floating Point Underflow"; break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:    exception_name = "Integer Divide by Zero"; break;
        case EXCEPTION_INT_OVERFLOW:          exception_name = "Integer Overflow"; break;
        case EXCEPTION_PRIV_INSTRUCTION:      exception_name = "Privileged Instruction"; break;
        case EXCEPTION_STACK_OVERFLOW:        exception_name = "Stack Overflow"; break;
        case EXCEPTION_INVALID_DISPOSITION:   exception_name = "Invalid Disposition"; break;
        case EXCEPTION_GUARD_PAGE:            exception_name = "Guard Page"; break;
        case EXCEPTION_INVALID_HANDLE:        exception_name = "Invalid Handle"; break;
    }
    
    // Format message
    std::string full_msg = std::format(
        "===============================================================\n"
        "                    UNHANDLED EXCEPTION                        \n"
        "===============================================================\n"
        "\n"
        "  Exception: {} (0x{:08X})\n"
        "\n"
        "===============================================================\n"
        "                       STACK TRACE                              \n"
        "===============================================================\n"
        "\n"
        "{}\n"
        "===============================================================\n"
        "\n"
        "  [ Abort ]  - Terminate the program\n"
        "  [ Retry ]  - Break into debugger (if attached)\n"
        "  [ Ignore ] - Continue (dangerous!)\n"
        "\n",
        exception_name, code, stack_trace
    );
    
    // Show dialog
    int result = MessageBoxA(
        nullptr,
        full_msg.c_str(),
        "Unhandled Exception",
        MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON2 | MB_SYSTEMMODAL
    );
    
    switch (result) {
        case IDABORT:
            return EXCEPTION_EXECUTE_HANDLER; // Terminate
            
        case IDRETRY:
            if (g_config.enable_debug_break) {
                DebugBreak();
            }
            return EXCEPTION_CONTINUE_EXECUTION;
            
        case IDIGNORE:
            return EXCEPTION_CONTINUE_EXECUTION; // Dangerous!
    }
    
    return EXCEPTION_EXECUTE_HANDLER;
}

} // namespace detail

// ============================================================================
// Convenience Macros
// ============================================================================

/**
 * @brief Assertion macro with automatic condition stringification
 * 
 * Usage: NASE_ASSERT(ptr != nullptr, "Pointer must not be null");
 */
#define NASE_ASSERT(condition, ...) \
    nasec::assert::Assert(!(condition), #condition, ##__VA_ARGS__)

/**
 * @brief Assertion macro for simple checks without message
 * 
 * Usage: NASE_CHECK(ptr);
 */
#define NASE_CHECK(condition) \
    nasec::assert::Assert(!(condition), #condition)

/**
 * @brief Static assertion with custom message
 * 
 * Usage: NASE_STATIC_ASSERT(sizeof(int) == 4, "Int must be 4 bytes");
 */
#define NASE_STATIC_ASSERT(condition, message) \
    static_assert(condition, message)

/**
 * @brief Debug-only assertion (disabled in release)
 * 
 * Usage: NASE_DEBUG_ASSERT(ptr != nullptr, "Should not be null");
 */
#ifdef _DEBUG
    #define NASE_DEBUG_ASSERT(condition, ...) NASE_ASSERT(condition, ##__VA_ARGS__)
#else
    #define NASE_DEBUG_ASSERT(condition, ...) ((void)0)
#endif

/**
 * @brief Macro to mark unreachable code
 * 
 * Usage: NASE_UNREACHABLE("This should never happen");
 */
#define NASE_UNREACHABLE(...) \
    nasec::assert::Unreachable(__VA_ARGS__)

/**
 * @brief Macro to state assumptions for the optimizer
 * 
 * Usage: NASE_ASSUME(ptr != nullptr);
 */
#if defined(_MSC_VER)
    #define NASE_ASSUME(condition) __assume(condition)
#elif defined(__GNUC__) || defined(__clang__)
    #define NASE_ASSUME(condition) __builtin_assume(condition)
#else
    #define NASE_ASSUME(condition) nasec::assert::Assume(condition)
#endif

} // namespace nasec::assert

// Convenience: bring main types to nasec namespace
namespace nasec {
    using assert::Assert;
    using assert::Verify;
    using assert::AssertThrow;
    using assert::AssertVar;
    using assert::Unreachable;
    using assert::Assume;
    using assert::Install;
    using assert::Uninstall;
    using assert::Config;
    using assert::AssertionException;
}

#endif // NASE_ASSERT_H
