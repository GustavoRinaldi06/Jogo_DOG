#include "LoadingState.h"
#include "Resources.h"
#include "Game.h"
#include "SpriteRenderer.h"

#include "SDL_include.h"

LoadingState::LoadingState(std::function<State *()> nextStateFactory, std::vector<std::string> assets)
    : currentIndex(0), createNext(std::move(nextStateFactory)), assetList(std::move(assets)), progressText(nullptr) {}

LoadingState::~LoadingState()
{
    // GameObjects e Components são limpos pela base State
}

void LoadingState::LoadAssets()
{
    // Fundo de loading
    GameObject *bgGO = new GameObject();
    SpriteRenderer *bgSR = new SpriteRenderer(*bgGO);
    bgSR->Open("recursos/img/Load.png");
    bgSR->SetCameraFollower(true);
    bgGO->AddComponent(bgSR);
    AddObject(bgGO);

    // Texto de progresso
    SDL_Color white = {255, 255, 255, 255};
    GameObject *textGO = new GameObject();
    progressText = new Text(*textGO, "recursos/font/fonteBase.ttf", 28,BLENDED, "Loading: 0%", white);
    textGO->AddComponent(progressText);
    progressText->SetCameraFollower(true);
    textGO->box.x = 500;
    textGO->box.y = 600;
    AddObject(textGO);
}

void LoadingState::Start()
{
    // Pré-carrega imagem de fundo e fonte do loading
    Resources::GetImage("recursos/img/Load.png");
    Resources::GetFont("recursos/font/fonteBase.ttf", 24);

    LoadAssets(); // monta visual da tela de loading
}

void LoadingState::Update(float dt)
{
    if (currentIndex < assetList.size())
    {
        const std::string &path = assetList[currentIndex++];
        // Detecta tipo e carrega corretamente
        if (path.find(".png") != std::string::npos || path.find(".jpg") != std::string::npos)
            Resources::GetImage(path);
        else if (path.find(".ttf") != std::string::npos)
            Resources::GetFont(path, 24);
        else if (path.find(".mp3") != std::string::npos || path.find(".ogg") != std::string::npos)
            Resources::GetMusic(path);

        // Atualiza texto de progresso
        if (progressText)
        {
            int pct = int((float(currentIndex) / assetList.size()) * 100);
            progressText->SetText("Loading: " + std::to_string(pct) + "%");
        }
    }
    else
    {
        State *next = createNext();     // Cria o próximo estado
        Game::GetInstance().Push(next); // Empilha o novo estado
        popRequested = true;            // Solicita remoção do LoadingState
    }
}

void LoadingState::Render()
{
    SDL_Renderer *renderer = Game::GetInstance().GetRenderer();
    SDL_RenderClear(renderer);
    RenderArray();
    SDL_RenderPresent(renderer);
}

void LoadingState::Pause() {}
void LoadingState::Resume() {}
