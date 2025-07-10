#include "EndState.h"
#include "SpriteRenderer.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Parallax.h"
#include "Camera.h"

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

        // Camada B
        GameObject *B = new GameObject();
        B->box.x = 0;
        B->box.y = 0;
        B->box.w = 2048;
        B->box.h = 512;
        B->AddComponent(new SpriteRenderer(*B, "recursos/img/End/B.png"));
        B->AddComponent(new Parallax(*B, 0.6f));
        AddObject(B);

        // Camada A
        GameObject *A = new GameObject();
        A->box.x = 0;
        A->box.y = -100;
        A->box.w = 2048;
        A->box.h = 512;
        A->AddComponent(new SpriteRenderer(*A, "recursos/img/End/A.png"));
        A->AddComponent(new Parallax(*A, 0.2f));
        AddObject(A);

        GameObject *textGO1 = new GameObject();
        Text *title = new Text(*textGO1, "recursos/font/neodgm.ttf", 48, BLENDED, "Credits:\n\nBryan G. Silva\nKivia\nGustavo Rinaldi\nLucas Almeida\nEnzo Nogueira\nAntonio Duarte", white);
        textGO1->AddComponent(title);

        title->SetCameraFollower(true);
        // Posição do texto
        textGO1->box.x = 430;
        textGO1->box.y = 150;

        AddObject(textGO1);
    }
    else // Se o jogador morreu
    {
        auto *bgObject = new GameObject();
        auto *bgRenderer = new SpriteRenderer(*bgObject);
        bgRenderer->Open("recursos/img/End/Lose.jpg");
        bgRenderer->SetCameraFollower(true);

        bgObject->box.x = 0;
        bgObject->box.y = 0;

        bgObject->AddComponent(bgRenderer);
        AddObject(bgObject);

        backgroundMusic.Open("recursos/audio/teen_titans.mp3");   
    }

    backgroundMusic.Play(); // musica de vitória ou derrota

    // Texto indicando para voltar ao início
    GameObject *textGO = new GameObject();
    pressSpace = new Text(*textGO, "recursos/font/neodgm.ttf", 24, BLENDED, "PRESS SPACE TO START AGAIN", white);
    textGO->AddComponent(pressSpace);

    pressSpace->SetCameraFollower(true);
    // Posição do texto
    textGO->box.x = 430;
    textGO->box.y = 600;

    AddObject(textGO);
}

void EndState::Update(float dt)
{
    exitTime.Update(dt);

    if (exitTime.Get() > 60.0f){
        popRequested = true;
        return;
    }
    Camera::GetInstance().Move(100 * dt, 0);

    // FAzer o texto piscar ---------------------------------------

    if (!pressSpace)
        return;

    static float timerBlink = 0.0f;
    static bool visible = true;
    timerBlink += Game::GetInstance().GetDeltaTime();
    if (timerBlink >= 0.5f)
    {
        // alterna os valores e zera o contador de blink
        visible = !visible;
        timerBlink = 0.0f;
        pressSpace->SetText(visible ? "PRESS SPACE TO START AGAIN" : ""); // Tira texto para piscar
    }

    InputManager &input = InputManager::GetInstance();

    // Se apertar ESC, sai do jogo
    if (input.QuitRequested() || input.KeyPress(SDLK_ESCAPE))
    {
        quitRequested = true;
        return;
    }

    // Se apertar ESPAÇO, volta ao TitleState
    if (input.KeyPress(SDLK_SPACE))
    {
        popRequested = true;
        return;
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
