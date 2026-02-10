#ifndef SPED
#define SPED

#include <skipsped/state_machine.hpp>
#include <imgui/TextEditor.h>
#include <d3d11.h>
#include <vector>
#include <string>

// declare instead of including wp
namespace polytoria{
class BaseScript;
}

namespace polytoria
{
    struct Instance;
}

namespace sped
{
    void main_thread();

    /**
     * @brief Represents the cheat
     *
     */
    class Sped
    {
    public:
        SpedState state;
        polytoria::Instance *selectedInstance = nullptr;

        Sped() : state(SpedState::X) {}

        static Sped &GetInstance()
        {
            static Sped instance;
            return instance;
        }

        ~Sped() = default;
        Sped(const Sped &) = delete;
        Sped &operator=(const Sped &) = delete;

        void Init();
        /**
         * @brief Render function, called every frame by the hooked Present function
         *
         * @param swap
         * @param swapInterval
         * @param flags
         * @return true
         * @return false do not render
         */
        bool Render(IDXGISwapChain *swap, UINT swapInterval, UINT flags);

    private:
        void LoadUnityAPI();

        void LoadImGui();
        static HRESULT HookedPresent(IDXGISwapChain *swap, UINT swapInterval, UINT flags);

        void RenderExplorerTab();
        void RenderSaveInstanceTab();
        void RenderExecutorTab();

    private:
        struct ScriptDecompileTab
        {
            polytoria::BaseScript *instance;
            TextEditor editor;
            bool isEditorReady = false;
        };

        void RenderDecompileTab(ScriptDecompileTab* tab);
        void OpenNewScriptDecompileTab(polytoria::BaseScript *instance);

        void LoadConsole();

    private:
        bool isEditorSetup = false;
        TextEditor *mScriptEditor;

        // TBD
        // for decompiled scripts through properties window
        std::vector<ScriptDecompileTab> mDecompileTabs;
    };
}

#endif /* SPED */
