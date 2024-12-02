#include <memory>
#include <string>
#include "../playdate-cpp/inc/pdcpp/pdnewlib.h"
#include "../playdate-cpp-extensions/inc/pdcpp/core/GlobalPlaydateAPI.h"
#include "GameManager.h"

/**
 * You can use STL containers! Be careful with some stdlib objects which rely
 * on an OS though, those will cause your app to crash on launch.
 */
std::unique_ptr<GameManager> gameManager;


/**
 * The Playdate API requires a C-style, or static function to be called as the
 * main update function. Here we use such a function to delegate execution to
 * our class.
 */
static int gameTick(void* userdata)
{
    gameManager->Update();
    return 1;
};

/**
 * the `eventHandler` function is the entry point for all Playdate applications
 * and Lua extensions. It requires C-style linkage (no name mangling) so we
 * must wrap the entire thing in this `extern "C" {` block
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is the main event handler. Using the `Init` and `Terminate` events, we
 * allocate and free the `gameManager` handler accordingly.
 *
 * You only need this `_WINDLL` block if you want to run this on a simulator on
 * a Windows machine, but for the sake of broad compatibility, we'll leave it
 * here.
 */
#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
    /*
     * This is required, otherwise linker errors abound
     */
    eventHandler_pdnewlib(pd, event, arg);

    // Initialization just creates our "game" object
    if (event == kEventInit)
    {
        //Initialize extension library for cpp
        pdcpp::GlobalPlaydateAPI::initialize(pd);

        pd->display->setRefreshRate(20);
        gameManager = std::make_unique<GameManager>(pd);

        // and sets the tick function to be called on update to turn off the
        // typical 'Lua'-ness.
        pd->system->setUpdateCallback(gameTick, pd);
    }

    // Destroy the global state to prevent memory leaks
    if (event == kEventTerminate)
    {
        pd->system->logToConsole("Tearing down...");
        gameManager = nullptr;
    }
    return 0;
}
#ifdef __cplusplus
}
#endif
