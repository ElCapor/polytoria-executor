#ifndef SCRIPTSOURCE_H
#define SCRIPTSOURCE_H

#include <ui/texteditor.h>
#include <ptoria/basescript.h>

class ScriptSourceUI
{
public:
    struct ScriptDecompileTab
    {
        BaseScript *instance;
        TextEditor* editor;
        bool isEditorReady = false;
    };
public:
    static void Init();
    static void DrawTab(ScriptSourceUI::ScriptDecompileTab* tab);
    static void OpenNewScriptDecompileTab(BaseScript *instance);
    static bool IsTabAlreadyOpen(BaseScript *instance);
    static std::vector<ScriptDecompileTab> openTabs;


};

#endif /* SCRIPTSOURCE_H */
