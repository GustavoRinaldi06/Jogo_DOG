#include "Game.h"
#include "TitleState.h"

#define INCLUDE_SDL
#include "SDL_include.h"

int main(int argc, char **argv)
{
    // Cria o estado inicial
    Game &game = Game::GetInstance();
    game.Push(new TitleState()); // adiciona estado de titulo, primeiro da pilha
    game.Run();

    return 0;
}
