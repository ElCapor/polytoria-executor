#include <ptoria/mousefunctions.h>
#include <unity/unity.h>
#include <spdlog/spdlog.h>
#include <Windows.h>
#include <format>

// Helper: extract number from DynValue arg
static double GetNumber(CallbackArguments* args, int idx) {
    DynValue* val = args->RawGet(idx, false);
    if (val == nullptr || val->Type() != DynValue::DataType::Number) return 0.0;
    return val->AsNumber();
}

// mouse_move(x, y) — Sets cursor screen position via Win32
DynValue* mouse_move(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 2)
        return DynValue::FromString("mouse_move requires 2 arguments: x, y");

    int x = static_cast<int>(GetNumber(args, 0));
    int y = static_cast<int>(GetNumber(args, 1));

    SetCursorPos(x, y);
    return DynValue::FromNil();
}

// mouse_get_position() — Returns current cursor screen position as "x,y" string
DynValue* mouse_get_position(void*, ScriptExecutionContext*, CallbackArguments*) {
    POINT pt;
    GetCursorPos(&pt);
    return DynValue::FromString(std::format("{},{}", pt.x, pt.y));
}

// mouse_click(button) — Simulates a full click (down+up). 0=left, 1=right, 2=middle
DynValue* mouse_click(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 1)
        return DynValue::FromString("mouse_click requires 1 argument: button (0=left, 1=right, 2=middle)");

    int button = static_cast<int>(GetNumber(args, 0));
    DWORD down = 0, up = 0;

    switch (button) {
        case 0: down = MOUSEEVENTF_LEFTDOWN;   up = MOUSEEVENTF_LEFTUP;   break;
        case 1: down = MOUSEEVENTF_RIGHTDOWN;  up = MOUSEEVENTF_RIGHTUP;  break;
        case 2: down = MOUSEEVENTF_MIDDLEDOWN; up = MOUSEEVENTF_MIDDLEUP; break;
        default: return DynValue::FromString("Invalid button: use 0 (left), 1 (right), or 2 (middle)");
    }

    mouse_event(down, 0, 0, 0, 0);
    mouse_event(up, 0, 0, 0, 0);
    return DynValue::FromNil();
}

// mouse_down(button) — Simulates button press
DynValue* mouse_down(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 1)
        return DynValue::FromString("mouse_down requires 1 argument: button");

    int button = static_cast<int>(GetNumber(args, 0));
    DWORD flag = 0;

    switch (button) {
        case 0: flag = MOUSEEVENTF_LEFTDOWN;   break;
        case 1: flag = MOUSEEVENTF_RIGHTDOWN;  break;
        case 2: flag = MOUSEEVENTF_MIDDLEDOWN; break;
        default: return DynValue::FromString("Invalid button: use 0, 1, or 2");
    }

    mouse_event(flag, 0, 0, 0, 0);
    return DynValue::FromNil();
}

// mouse_up(button) — Simulates button release
DynValue* mouse_up(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 1)
        return DynValue::FromString("mouse_up requires 1 argument: button");

    int button = static_cast<int>(GetNumber(args, 0));
    DWORD flag = 0;

    switch (button) {
        case 0: flag = MOUSEEVENTF_LEFTUP;   break;
        case 1: flag = MOUSEEVENTF_RIGHTUP;  break;
        case 2: flag = MOUSEEVENTF_MIDDLEUP; break;
        default: return DynValue::FromString("Invalid button: use 0, 1, or 2");
    }

    mouse_event(flag, 0, 0, 0, 0);
    return DynValue::FromNil();
}

// mouse_scroll(amount) — Simulates scroll wheel. Positive = up, negative = down
DynValue* mouse_scroll(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 1)
        return DynValue::FromString("mouse_scroll requires 1 argument: amount");

    int amount = static_cast<int>(GetNumber(args, 0));
    mouse_event(MOUSEEVENTF_WHEEL, 0, 0, static_cast<DWORD>(amount * WHEEL_DELTA), 0);
    return DynValue::FromNil();
}

// mouse_lock(locked) — Sets Unity Cursor.lockState. 1=Locked, 0=None
DynValue* mouse_lock(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 1)
        return DynValue::FromString("mouse_lock requires 1 argument: locked (1=lock, 0=unlock)");

    int locked = static_cast<int>(GetNumber(args, 0));
    if (locked)
        UnityResolve::UnityType::Cursor::set_lockState(UnityResolve::UnityType::CursorLockMode::Locked);
    else
        UnityResolve::UnityType::Cursor::set_lockState(UnityResolve::UnityType::CursorLockMode::None);

    return DynValue::FromNil();
}

// mouse_visible(visible) — Sets Unity Cursor.visible. 1=visible, 0=hidden
DynValue* mouse_visible(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 1)
        return DynValue::FromString("mouse_visible requires 1 argument: visible (1=show, 0=hide)");

    int visible = static_cast<int>(GetNumber(args, 0));
    UnityResolve::UnityType::Cursor::set_visible(visible != 0);
    return DynValue::FromNil();
}
