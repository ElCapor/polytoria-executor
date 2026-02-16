#ifndef UI_H
#define UI_H

class UI
{
public:
    static void Setup();
    static void Draw();

public:
    enum UiState : int {
        NotReady, // Not ready, i.e waiting for unity
        Ready, // Ready, can draw and interact with the user
        Closed, // Closed, the user closed the UI, we should not draw or interact with the user
    };

    static UiState state;
    static int keybind;
    static bool blockGameInput; // When true, game input is blocked when UI is open

private:
    static void DrawWaitingScreen();
    static void DrawMainUI();
    static void DrawClosedHint();
};


#endif /* UI_H */
