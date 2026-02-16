#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <ui/texteditor.h>

struct ExecutorUI
{
    static void Init();
    static void DrawTab();

    static TextEditor* scriptEditor;
};

#endif /* EXECUTOR_H */
