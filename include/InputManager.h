#ifndef INPUT_MANAGER
#define INPUT_MANAGER
// Controle SDL -----------------------------------
#define LEFT_ARROW_KEY SDLK_LEFT
#define RIGHT_ARROW_KEY SDLK_RIGHT
#define UP_ARROW_KEY SDLK_UP
#define DOWN_ARROW_KEY SDLK_DOWN
#define ESCAPE_KEY SDLK_ESCAPE
#define LEFT_MOUSE_BUTTON SDL_BUTTON_LEFT
//------------------------------------------------

#include <unordered_map>

#define INCLUDE_SDL
#include "SDL_include.h"

class InputManager
{
public:
    void Update();

    bool KeyPress(int key);
    bool KeyRelease(int key);
    bool IsKeyDown(int key);

    bool MousePress(int button);
    bool MouseRelease(int button);
    bool IsMouseDown(int button);

    int GetMouseX();
    int GetMouseY();

    bool QuitRequested();

    static InputManager& GetInstance();

private:
    InputManager();
    ~InputManager();

    std::unordered_map<int, bool> keyState;
    std::unordered_map<int, int> keyUpdate;

    bool mouseState[6];
    int mouseUpdate[6];

    bool quitRequested;
    int updateCounter, mouseX, mouseY;
};

#endif // INPUT_MANAGER