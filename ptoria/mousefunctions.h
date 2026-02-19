#ifndef MOUSEFUNCTIONS_H
#define MOUSEFUNCTIONS_H

#include <moonsharp/cinterface.h>

// Mouse control Lua callbacks
DynValue* mouse_move(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* mouse_get_position(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* mouse_click(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* mouse_down(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* mouse_up(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* mouse_scroll(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* mouse_lock(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* mouse_visible(void*, ScriptExecutionContext*, CallbackArguments*);

#endif /* MOUSEFUNCTIONS_H */
