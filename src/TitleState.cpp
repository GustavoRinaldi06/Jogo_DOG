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
#include "VideoState.h"

#define INCLUDE_SDL
#include "SDL_include.h"

TitleState::TitleState(): State()
{}

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

// Carregar texto de pressStart ------------------------------------------------------------------------------------
    SDL_Color white = {255, 255, 255, 255};
    GameObject *textGO = new GameObject();
    pressStart = new Text(*textGO, "recursos/font/fonteBase.ttf", 34, BLENDED, "PRESS SPACE TO START !", white);
    textGO->AddComponent(pressStart);

    pressStart->SetCameraFollower(true);
    // Posição do texto
    textGO->box.x = 600;
    textGO->box.y = 350;

    AddObject(textGO);

// Carregar titulo ------------------------------------------------------------------------------------------------
    GameObject *textGO1 = new GameObject();
    Text* title = new Text(*textGO1, "recursos/font/Titulo.ttf", 224, BLENDED, "D O G", white);
    textGO1->AddComponent(title);

    title->SetCameraFollower(true);
    // Posição do texto
    textGO1->box.x = 520;
    textGO1->box.y = 25;

    AddObject(textGO1);

// Carregar musica -----------------------------------------------------------------------------------
    backgroundMusic.Open("recursos/audio/BGmusic/Tema.mp3");
    backgroundMusic.Play();
}

void TitleState::Update(float dt)
{
    // VErificar ações de input ---------------------------------------

    InputManager &input = InputManager::GetInstance();

    // Ao clicar X na janela
    if (input.QuitRequested() || input.KeyPress(SDLK_ESCAPE))
    {
        quitRequested = true;
        return;
    }

    // Se ESPAÇO for pressionado, empilha StageState
    if (input.KeyPress(SDLK_SPACE))
    {
        // Reproduzir vídeo antes de carregar o jogo
        Game::GetInstance().Push(
            new VideoState("recursos/videos/output.uyvy", 1920, 1080, 60.0f, 
                []() {
                    std::vector<std::string> treeAssets = {
                        // background
                        "recursos/img/background/Tree/E.png",
                        "recursos/img/background/Tree/B.png",
                        "recursos/img/background/Vinheta.png",
                        "recursos/map/Tree/tiles.png",
                        // sprites
                        "recursos/img/sprites/Player.png",
                        "recursos/img/sprites/Hand.png", 
                        "recursos/img/sprites/Thorn.png", 
                        "recursos/img/sprites/Chainsaw.png",
                        "recursos/img/sprites/DogHowling.png",
                        "recursos/img/sprites/DogShoot.png",
                        // sons
                        "recursos/audio/BGmusic/treeState.mp3",
                        "recursos/audio/Hunter/boing.mp3", 
                        "recursos/audio/DOG/explode.mp3",
                        "recursos/audio/Hunter/boing.mp3", 
                        "recursos/audio/DOG/explode.mp3",
                        "recursos/audio/Hunter/boing.mp3", 
                        "recursos/audio/DOG/explode.mp3",
                    };

                    GameData::state = 1;
                    GameData::playerHP = 100;
                    return new LoadingState([]() {
                        return new TreeState();
                    }, treeAssets);
                }
            )
        );
        //Game::GetInstance().Push(new LoadingState([](){ return new TreeState(); }, treeAssets));
    }

    // FAzer o texto piscar ---------------------------------------

    if (!pressStart)
        return;

    static float timerBlink = 0.0f;
    static bool visible = true;
    timerBlink += Game::GetInstance().GetDeltaTime();
    if (timerBlink >= 0.5f)
    {
        // alterna os valores e zera o contador de blink
        visible = !visible;
        timerBlink = 0.0f;
        pressStart->SetText(visible ? "PRESS SPACE TO START" : ""); //Tira texto para piscar
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
