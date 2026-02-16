#ifndef CINTERFACE_H
#define CINTERFACE_H

#include <moonsharp/dynvalue.h>
#include <moonsharp/scriptexecutioncontext.h>
#include <moonsharp/callbackarguments.h>

/**
 * @brief Callback Function type, for interfacing C++ code with moonsharp
 * @note I suspect the first void* to be MulticastDelegate*, but i can't prove it yet
 */
using CallbackFunction = DynValue *(*)(void*, ScriptExecutionContext *, CallbackArguments *);


void RegisterCallback(Table* globals, const std::string& name, CallbackFunction func);

#endif /* CINTERFACE_H */
