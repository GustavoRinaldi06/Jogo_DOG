#include "TitleState.h"
#include "Game.h"
#include "TreeState.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "GameObject.h"
#include "Character.h"
#include "PlayerController.h"
#include "LoadingState.h"
#include "Camera.h"
#include "CutsceneIntroState.h"

#define INCLUDE_SDL
#include "SDL_include.h"

TitleState::TitleState() : State()
{
}

TitleState::~TitleState() = default;

void TitleState::LoadAssets()
{
    // Carregar imagem ------------------------------------------------------------------------------------
    GameObject *titleGO = new GameObject();

    SpriteRenderer *spriter = new SpriteRenderer(*titleGO);
    spriter->Open("recursos/img/Menu.png");
    spriter->SetCameraFollower(true);

    titleGO->AddComponent(spriter);
    AddObject(titleGO);

    // Carregar titulo ------------------------------------------------------------------------------------------------
    SDL_Color white = {255, 255, 255, 255};

    GameObject *textGO1 = new GameObject();
    Text *title = new Text(*textGO1, "recursos/font/Titulo.ttf", 224, BLENDED, "D O G", white);
    textGO1->AddComponent(title);

    title->SetCameraFollower(true);
    // Posição do texto
    textGO1->box.x = 520;
    textGO1->box.y = 25;

    AddObject(textGO1);

    // Carregar musica -----------------------------------------------------------------------------------
    backgroundMusic.Open("recursos/audio/BGmusic/Tema.mp3");
    backgroundMusic.Play();

    // Botões -------------------------------------------------------------------------------------------=
    // Botão: Novo Jogo
    newGameGO = new GameObject();
    Text *newGameText = new Text(*newGameGO, "recursos/font/fonteBase.ttf", 48, BLENDED, "Novo Jogo", white);
    newGameGO->AddComponent(newGameText);
    newGameText->SetCameraFollower(true);
    newGameGO->box.x = 750;
    newGameGO->box.y = 350;
    AddObject(newGameGO);
    clickSound = new Sound("recursos/audio/Menu/select.wav");
    clickSound->Stop();

    // Botão: Sair
    exitGO = new GameObject();
    Text *exitText = new Text(*exitGO, "recursos/font/fonteBase.ttf", 48, BLENDED, "Sair", white);
    exitGO->AddComponent(exitText);
    exitText->SetCameraFollower(true);
    exitGO->box.x = 825;
    exitGO->box.y = 475;
    AddObject(exitGO);
}

void TitleState::Update(float dt)
{
    // VErificar ações de input ---------------------------------------

    InputManager &input = InputManager::GetInstance();

    // Ao clicar X na janela
    if (input.QuitRequested())
    {
        quitRequested = true;
        return;
    }

    if (input.MousePress(SDL_BUTTON_LEFT))
    {
        int mouseX = input.GetMouseX();
        int mouseY = input.GetMouseY();

        if (newGameGO->box.Contains(mouseX, mouseY)) // Clica em novo jogo
        {
            if(clickSound){
                clickSound->Play(1); // Toca o som de clique
            }
            backgroundMusic.Stop(500); // Para a música atual
            // Novo Jogo clicado
            Game::GetInstance().Push(
                new CutsceneIntroState(
                    []()
                    {
                        std::vector<std::string> treeAssets = {
                            // background
                            "recursos/img/background/Vinheta.png",
                            "recursos/map/Tree/tiles.png",
                            // resized
                            "recursos/img/background/Tree/resized/E.png",
                            "recursos/img/background/Tree/resized/D.png",
                            "recursos/img/background/Tree/resized/C.png",
                            "recursos/img/background/Tree/resized/B.png",
                            "recursos/img/background/Tree/resized/A.png",
                            // sprites
                            "recursos/img/sprites/Player.png",
                            "recursos/img/sprites/Hand.png",
                            "recursos/img/sprites/Thorn.png",
                            "recursos/img/sprites/Chainsaw.png",
                            "recursos/img/sprites/DogHowling.png",
                            "recursos/img/sprites/DogShoot.png",
                            // sons
                            "recursos/audio/BGmusic/treeState.mp3",
                            "recursos/audio/DOG/explode.mp3",
                            "recursos/audio/DOG/explode.mp3",
                            "recursos/audio/DOG/explode.mp3",
                        };

                        GameData::state = 1;
                        GameData::playerHP = 100;
                        return new LoadingState([]()
                                                { return new TreeState(); }, treeAssets);
                    }));
        }

        if (exitGO->box.Contains(mouseX, mouseY)) // Clica em sair
        {
            quitRequested = true;
        }
    }
}

void TitleState::Render()
{
    SDL_Renderer *renderer = Game::GetInstance().GetRenderer();

    // Limpa
    SDL_RenderClear(renderer);

    // Desenha
    RenderArray();

    // Apresenta
    SDL_RenderPresent(renderer);
}

void TitleState::Start()
{
    LoadAssets();
    StartArray();
    started = true;
}
