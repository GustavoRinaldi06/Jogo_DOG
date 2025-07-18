#include "EndState.h"
#include "SpriteRenderer.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Parallax.h"
#include "Camera.h"
#include "Character.h"

#define INCLUDE_SDL
#include "SDL_include.h"

EndState::EndState() {
    exitTime.Restart();
}

EndState::~EndState() = default;

void EndState::LoadAssets()
{
    SDL_Color white = {255, 255, 255, 255};

    if (GameData::playerVictory) // Se o jogador ganhou e detorrou as waves
    {
        Camera::GetInstance().Unfollow();
        Character::player = nullptr; // DEBUG

        // Camada B
        GameObject *B = new GameObject();
        B->box.x = 0;
        B->box.y = 0;
        B->box.w = 2048;
        B->box.h = 512;
        B->AddComponent(new SpriteRenderer(*B, "recursos/img/End/B.png"));
        B->AddComponent(new Parallax(*B, 0.2f));
        AddObject(B);

        // Camada A
        GameObject *A = new GameObject();
        A->box.x = 0;
        A->box.y = -100;
        A->box.w = 2048;
        A->box.h = 512;
        A->AddComponent(new SpriteRenderer(*A, "recursos/img/End/A.png"));
        A->AddComponent(new Parallax(*A, 0.6f));
        AddObject(A);

        // Creditos -------------------------------------
        GameObject *textGO1 = new GameObject();
        Text *title = new Text(*textGO1, "recursos/font/fonteBase.ttf", 48, BLENDED, "Credits:\n\nBryan G. Silva - designer\nKivia Fernanda - designer\nGustavo Rinaldi - programador\nLucas Almeida - programador\nEnzo Nogueira - musico\nTom Duarte - musico", white);
        textGO1->AddComponent(title);

        title->SetCameraFollower(true);
        // Posição do texto
        textGO1->box.x = 330;
        textGO1->box.y = 100;

        AddObject(textGO1);

        backgroundMusic.Open("recursos/audio/BGmusic/Tema.mp3");
    }
    else // Se o jogador morreu
    {
        auto *bgObject = new GameObject();
        auto *bgRenderer = new SpriteRenderer(*bgObject);
        bgRenderer->Open("recursos/img/End/Derrota.png");
        bgRenderer->SetCameraFollower(true);

        bgObject->box.x = 0;
        bgObject->box.y = 0;

        bgObject->AddComponent(bgRenderer);
        AddObject(bgObject);

        backgroundMusic.Open("recursos/audio/BGmusic/Lose.mp3");   
    }

    backgroundMusic.Play(); // musica de vitória ou derrota

    // Botão: Novo Jogo
    menuGO = new GameObject();
    Text *newGameText = new Text(*menuGO, "recursos/font/fonteBase.ttf", 38, BLENDED, "Novo Jogo", white);
    menuGO->AddComponent(newGameText);
    newGameText->SetCameraFollower(true);
    menuGO->box.x = 500;
    menuGO->box.y = 500;
    AddObject(menuGO);

    // Botão: Sair
    exitGO = new GameObject();
    Text *exitText = new Text(*exitGO, "recursos/font/fonteBase.ttf", 38, BLENDED, "Sair", white);
    exitGO->AddComponent(exitText);
    exitText->SetCameraFollower(true);
    exitGO->box.x = 550;
    exitGO->box.y = 570;
    AddObject(exitGO);
}

void EndState::Update(float dt)
{
    exitTime.Update(dt);

    if (exitTime.Get() > 60.0f){
        popRequested = true;
        return;
    }
    Camera::GetInstance().Move(100 * dt, 0);

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

        if (menuGO->box.Contains(mouseX, mouseY))
        {
            popRequested = true;
            return;
        }
        if (exitGO->box.Contains(mouseX, mouseY))
        {
            quitRequested = true;
            return;
        }
    }
}

void EndState::Render()
{
    RenderArray();
}

void EndState::Start()
{
    LoadAssets();
    StartArray();
    started = true;
}

void EndState::Pause()
{}

void EndState::Resume()
{}
