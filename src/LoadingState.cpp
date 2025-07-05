#include "LoadingState.h"
#include "Resources.h"
#include "Game.h"
#include "SDL_include.h"

LoadingState::LoadingState(std::function<State*()> nextStateFactory)
    : currentIndex(0), createNext(std::move(nextStateFactory)), progressText(nullptr)
{}

LoadingState::~LoadingState() {
    // Components e GameObjects são limpos pelo State base
}

void LoadingState::LoadAssets() {
    // Tela de abertura de loading
    GameObject* bgGO = new GameObject();
    SpriteRenderer* bgSR = new SpriteRenderer(*bgGO);
    bgSR->Open("recursos/img/Abertura.jpg");
    bgSR->SetCameraFollower(true);
    bgGO->AddComponent(bgSR);
    AddObject(bgGO);

    // Texto de progresso inicial
    SDL_Color white = {255, 255, 255, 255};
    GameObject* textGO = new GameObject();
    progressText = new Text(*textGO, "recursos/font/neodgm.ttf", 24,
                            BLENDED, "Loading: 0%", white);
    textGO->AddComponent(progressText);
    progressText->SetCameraFollower(true);
    textGO->box.x = 500;
    textGO->box.y = 600;
    AddObject(textGO);
}

void LoadingState::Start() {
    // Lista de assets a carregar após a tela inicial
    assetList = {
        "recursos/img/background/fase_1/E.png",
        "recursos/img/background/fase_1/D.png",
        "recursos/img/background/fase_1/C.png",
        "recursos/img/background/fase_1/B.png",
        "recursos/img/background/fase_1/A.png",
        "recursos/img/background/fase_1/Efeito_Vinheta.png",
        "recursos/img/tiles/map_tiles.png",
        "recursos/img/player/walk.png"
    };

    // Preload da fonte e da imagem de abertura
    Resources::GetImage("recursos/img/Abertura.jpg");
    Resources::GetFont("recursos/font/neodgm.ttf", 24);

    // Monta UI de loading
    LoadAssets();
}

void LoadingState::Update(float dt) {
    if (currentIndex < assetList.size()) {
        const std::string& path = assetList[currentIndex++];
        Resources::GetImage(path);
        if (path.find(".mp3") != std::string::npos) Resources::GetMusic(path);
        if (path.find(".ttf") != std::string::npos) Resources::GetFont(path, 24);
        // Atualiza texto de porcentagem
        if (progressText) {
            int pct = int((float(currentIndex) / assetList.size()) * 100);
            progressText->SetText("Loading: " + std::to_string(pct) + "%");
        }
    } else {
        State* next = createNext();
        Game::GetInstance().Push(next);
        popRequested = true;
    }
}

void LoadingState::Render() {
    SDL_Renderer* renderer = Game::GetInstance().GetRenderer();
    SDL_RenderClear(renderer);
    RenderArray();
    SDL_RenderPresent(renderer);
}

void LoadingState::Pause() {}
void LoadingState::Resume() {}
