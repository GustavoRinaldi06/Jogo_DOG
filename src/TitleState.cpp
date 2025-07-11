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
    spriter->Open("recursos/img/Abertura.jpg"); 
    spriter->SetCameraFollower(true); // igual a imagem de fundo   

    titleGO->AddComponent(spriter);
    AddObject(titleGO);

// Carregar texto de pressStart ------------------------------------------------------------------------------------
    SDL_Color white = {255, 255, 255, 255};
    GameObject *textGO = new GameObject();
    pressStart = new Text(*textGO, "recursos/font/neodgm.ttf", 24, BLENDED, "PRESS SPACE TO START !", white);
    textGO->AddComponent(pressStart);

    pressStart->SetCameraFollower(true);
    // Posição do texto
    textGO->box.x = 450;
    textGO->box.y = 600;

    AddObject(textGO);

// Carregar titulo ------------------------------------------------------------------------------------------------
    GameObject *textGO1 = new GameObject();
    Text* title = new Text(*textGO1, "recursos/font/GoWW.ttf", 224, BLENDED, "D O G", white);
    textGO1->AddComponent(title);

    title->SetCameraFollower(true);
    // Posição do texto
    textGO1->box.x = 375;
    textGO1->box.y = 250;

    AddObject(textGO1);

// Carregar musica -----------------------------------------------------------------------------------
    backgroundMusic.Open("recursos/audio/BGmusic/titleState.mp3");
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
        std::vector<std::string> treeAssets = {
            "recursos/img/Tree/E.png",
            "recursos/img/Tree/D.png",
            "recursos/img/Tree/C.png",
            "recursos/img/Tree/B.png",
            "recursos/img/Tree/A.png",
            "recursos/img/Tree/tile.png",
            "recursos/img/Player.png",
            "recursos/img/tree/Vinheta.png",
            "recursos/audio/BGmusic/treeState.mp3"};

        GameData::state = 1;
        Game::GetInstance().Push(new LoadingState([](){ return new TreeState(); }, treeAssets));
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
