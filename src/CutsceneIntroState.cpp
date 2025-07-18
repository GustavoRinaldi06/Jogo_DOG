#include "CutsceneIntroState.h"
#include "Game.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "ObjectFactory.h"
#include <iostream>

// comando que converter video
// C:\ffmpeg\bin\ffmpeg.exe -i "C:\Users\lukas\Downloads\intro-dog.mp4" -c:v rawvideo -pix_fmt uyvy422 -f rawvideo -an "output.uyvy"
CutsceneIntroState::CutsceneIntroState(std::function<State *()> nextStateFactory)
    : createNext(std::move(nextStateFactory))
{
}

CutsceneIntroState::~CutsceneIntroState()
{
}

void CutsceneIntroState::LoadAssets()
{
    images.emplace("trees", ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/Intro/trees.png"));
    images.emplace("primeira_cena", ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/Intro/primeira_cena.png"));
    images.emplace("dog", ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/Intro/dog.png"));
    images.emplace("hunter_1", ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/Intro/hunter_1.png"));
    images.emplace("hunter_2", ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/Intro/hunter_2.png"));
    images.emplace("turn_1", ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/Intro/turn_1.png"));
    images.emplace("turn_2", ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/Intro/turn_2.png"));
    images.emplace("background", ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/Intro/background.png"));
    // Adicione mais cenas conforme necessário, seguindo o padrão acima

    for(const auto &pair : images)
    {
        auto &image = pair.second;
        AddObject(image);
    }
}

void CutsceneIntroState::Update(float dt)
{
    InputManager &input = InputManager::GetInstance();

    // Ao clicar X na janela
    if (input.QuitRequested() || input.KeyPress(SDLK_ESCAPE))
    {
        quitRequested = true;
        return;
    }

    timer.Update(dt);
    if (timer.Between(0.f, 1.f))
    {
        std::cout << "primeira_cena: " << timer.Get() << std::endl;
        ShowImage("primeira_cena");
    }
    else if (timer.Between(1.f, 2.f))
    {
        std::cout << "dog: " << timer.Get() << std::endl;
        HideImage("primeira_cena");
        ShowImage("dog");
    }
    else if (timer.Between(2.f, 3.f))
    {
        HideImage("dog");
        ShowImage("hunter_1");
    }
    else if (timer.Between(3.f, 4.f))
    {
        HideImage("hunter_1");
        ShowImage("hunter_2");
    }
    else if (timer.Between(4.f, 5.f))
    {
        HideImage("hunter_2");
        ShowImage("turn_1");
    }
    else if (timer.Between(5.f, 6.f))
    {
        HideImage("turn_1");
        ShowImage("turn_2");
    }
    else if (timer.Between(6.f, 7.f))
    {
        HideImage("turn_2");
        CallNextState();
    } else if(timer.Get() > 7.f) {
        HideImage("background");
        CallNextState();
    }
}

void CutsceneIntroState::Render()
{
    SDL_Renderer *renderer = Game::GetInstance().GetRenderer();

    // Limpa tudo para o próximo frame
    SDL_RenderClear(renderer);

    // Desenha todos os game objects
    RenderArray();

    // Apresenta o frame
    SDL_RenderPresent(renderer);
}

void CutsceneIntroState::Start()
{
    LoadAssets();
    started = true;
}

void CutsceneIntroState::CallNextState()
{
    State *next = createNext();     // Cria o próximo estado
    Game::GetInstance().Push(next); // Empilha o novo estado
    popRequested = true;
}

void CutsceneIntroState::ShowImage(const std::string &imageName)
{
    SpriteRenderer *sprite = (SpriteRenderer *)images.at(imageName)->GetComponent("SpriteRenderer");
    if (sprite)
    {
        std::cout << "Showing image: " << imageName << std::endl;
        sprite->SetAlpha(255); // Define a transparência para 255 (totalmente visível)
    }
}

void CutsceneIntroState::HideImage(const std::string &imageName)
{
    SpriteRenderer *sprite = (SpriteRenderer *)images.at(imageName)->GetComponent("SpriteRenderer");
    if (sprite)
    {
        sprite->SetAlpha(0); // Define a transparência para 0 (totalmente invisível)
    }
}

void CutsceneIntroState::ScaleImage(const std::string &imageName, float scale)
{
    SpriteRenderer *sprite = (SpriteRenderer *)images.at(imageName)->GetComponent("SpriteRenderer");
    if (sprite)
    {
        sprite->SetScale(scale, scale); // Define o scale do sprite
    }
}
