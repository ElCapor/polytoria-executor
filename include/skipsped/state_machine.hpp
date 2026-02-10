#ifndef STATE_MACHINE
#define STATE_MACHINE


enum class SpedState {
    X, // Initial state, waiting Unity API to be loaded

    Ready, // Unity API loaded, waiting for the game to start
};



#endif /* STATE_MACHINE */
