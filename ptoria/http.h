#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <moonsharp/dynvalue.h>
#include <moonsharp/script.h>
#include <moonsharp/scriptexecutioncontext.h>
#include <moonsharp/callbackarguments.h>

std::string PerformHttpGet(const std::string& url);
DynValue* httpget(void* _, ScriptExecutionContext* ctx, CallbackArguments* args);

#endif
