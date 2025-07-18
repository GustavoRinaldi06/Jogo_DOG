#include "CutsceneGhostHunterState.h"
#include "Game.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "ObjectFactory.h"
#include <iostream>
#include "GameData.h"

CutsceneGhostHunterState::CutsceneGhostHunterState(std::function<State *()> nextStateFactory)
    : createNext(std::move(nextStateFactory))
{
}

CutsceneGhostHunterState::~CutsceneGhostHunterState()
{
}

void CutsceneGhostHunterState::LoadAssets()
{
    images.emplace(std::string("1"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/1.png"));
    images.emplace(std::string("2"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/2.png"));
    images.emplace(std::string("3"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/3.png"));
    images.emplace(std::string("4"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/4.png"));
    images.emplace(std::string("5"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/5.png"));
    images.emplace(std::string("6"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/6.png"));
    images.emplace(std::string("7"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/7.png"));
    images.emplace(std::string("8"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/8.png"));
    images.emplace(std::string("9"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/9.png"));
    images.emplace(std::string("10"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/10.png"));
    images.emplace(std::string("11"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/11.png"));
    images.emplace(std::string("12"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/12.png"));
    images.emplace(std::string("13"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/13.png"));
    images.emplace(std::string("14"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/14.png"));
    images.emplace(std::string("15"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/GhostHunter/15.png"));
    images.emplace(std::string("16"), ObjectFactory::CreateSceneImageObject("recursos/img/Cutscenes/capas/C3-O_Caçador.png"));

    sounds.emplace("noise", new Sound("recursos/audio/CutsceneGhost/Low_dog.wav"));
    sounds.emplace("shot", new Sound("recursos/audio/CutsceneIntro/shot.wav"));
    sounds.emplace("heartbeat", new Sound("recursos/audio/CutsceneGhost/Batimentos.wav"));
    sounds.emplace("respiration", new Sound("recursos/audio/CutsceneGhost/Arfando_1.wav"));
    // Adicione mais cenas conforme necessário, seguindo o padrão acima

    for (const auto &pair : images)
    {
        auto &image = pair.second;
        AddObject(image);
        HideImage(pair.first); // Esconde todas as imagens inicialmente
    }
}

void CutsceneGhostHunterState::Update(float dt)
{
    InputManager &input = InputManager::GetInstance();

    // Ao clicar X na janela
    if (input.QuitRequested() || input.KeyPress(SDLK_ESCAPE))
    {
        quitRequested = true;
        return;
    }

    timer.Update(dt);
    if (timer.Between(0.f, 5.5f) && scenes[0] == false)
    {
        scenes[0] = true;
        sounds.at("noise")->Play(1); // Toca o som de ruído
        ShowImage("1");
    }
    if (timer.Between(5.5f, 7.5f) && scenes[1] == false)
    {
        HideImage("1");
        ShowImage("2");
        scenes[1] = true;
    }
    if (timer.Between(7.5f, 9.5f) && scenes[2] == false)
    {
        HideImage("2");
        ShowImage("3");
        scenes[2] = true;
    }
    if (timer.Between(9.5f, 11.5f) && scenes[3] == false)
    {
        HideImage("3");
        ShowImage("4");
        scenes[3] = true;
    }
    if (timer.Between(11.5f, 13.5f) && scenes[4] == false)
    {
        HideImage("4");
        ShowImage("5");
        scenes[4] = true;
    }
    if (timer.Between(13.5f, 15.5f) && scenes[5] == false)
    {
        HideImage("5");
        ShowImage("6");
        scenes[5] = true;
    }
    if (timer.Between(15.5f, 17.5f) && scenes[6] == false)
    {
        HideImage("6");
        ShowImage("7");
        scenes[6] = true;
    }
    if (timer.Between(17.5f, 19.5f) && scenes[7] == false)
    {
        HideImage("7");
        ShowImage("8");
        scenes[7] = true;
    }
    if (timer.Between(19.5f, 23.5f) && scenes[8] == false)
    {
        HideImage("8");
        ShowImage("9");
        scenes[8] = true;
    }
    if (timer.Between(23.5f, 25.5f) && scenes[9] == false)
    {
        HideImage("9");
        ShowImage("10");
        scenes[9] = true;
    }
    if (timer.Between(25.5f, 27.5f) && scenes[10] == false)
    {
        HideImage("10");
        ShowImage("11");
        scenes[10] = true;
    }
    if (timer.Between(27.5f, 29.5f) && scenes[11] == false)
    {
        HideImage("11");
        ShowImage("1");
        sounds.at("noise")->Stop();
        sounds.at("shot")->Play(1);
        sounds.at("heartbeat")->Play(10, 10000);
        scenes[11] = true;
    }
    if (timer.Between(29.5f, 34.f) && scenes[12] == false)
    {
        sounds.at("respiration")->Play(10, 10000);
        scenes[12] = true;
    }
    if (timer.Between(34.f, 38.f) && scenes[13] == false)
    {
        HideImage("1");
        ShowImage("12");
        scenes[13] = true;
    }
    if (timer.Between(38.f, 42.f) && scenes[14] == false)
    {
        HideImage("12");
        ShowImage("13");
        scenes[14] = true;
    }
    if (timer.Between(42.f, 45.f) && scenes[15] == false)
    {
        HideImage("13");
        ShowImage("14");
        scenes[15] = true;
    }
    if (timer.Between(45.f, 48.f) && scenes[16] == false)
    {
        HideImage("14");
        ShowImage("15");
        scenes[16] = true;
    }
    if (timer.Between(48.f, 53.f) && scenes[17] == false)
    {
        HideImage("15");
        ShowImage("16");
        sounds.at("heartbeat")->Stop();
        sounds.at("respiration")->Stop();
        scenes[17] = true;
    }

    if (timer.Get() > 53.f)
    {
        CallNextState();
    }
}

void CutsceneGhostHunterState::Render()
{
    SDL_Renderer *renderer = Game::GetInstance().GetRenderer();

    // Limpa tudo para o próximo frame
    SDL_RenderClear(renderer);

    // Desenha todos os game objects
    RenderArray();

    // Apresenta o frame
    SDL_RenderPresent(renderer);
}

void CutsceneGhostHunterState::Start()
{
    LoadAssets();
    started = true;
}

void CutsceneGhostHunterState::CallNextState()
{
    for (const auto &pair : sounds)
    {
        auto &sound = pair.second;
        sound->Stop();
    }
    State *next = createNext();     // Cria o próximo estado
    Game::GetInstance().Push(next); // Empilha o novo estado
    popRequested = true;
}

void CutsceneGhostHunterState::ShowImage(const std::string &imageName)
{
    SpriteRenderer *sprite = GetSpriteRenderer(imageName);
    if (sprite)
    {
        sprite->setActive(true); // Define a escala para o tamanho da janela
    }
}

void CutsceneGhostHunterState::HideImage(const std::string &imageName)
{
    SpriteRenderer *sprite = GetSpriteRenderer(imageName);
    if (sprite)
    {
        sprite->setActive(false); // Define a escala para 0 (totalmente invisível)
    }
}

void CutsceneGhostHunterState::ScaleImageFixed(const std::string &imageName, Vec2 amount)
{
    SpriteRenderer *sprite = GetSpriteRenderer(imageName);
    if (sprite)
    {
        Vec2 newScale = sprite->GetScale() + amount; // Incrementa o scale atual
        sprite->SetScale(newScale.x, newScale.y);    // Define o scale do sprite
    }
}

SpriteRenderer *CutsceneGhostHunterState::GetSpriteRenderer(const std::string &imageName)
{
    if (images.find(imageName) == images.end())
    {
        std::cerr << "Image not found: " << imageName << std::endl;
        return nullptr;
    }
    GameObject *gameObject = images.at(imageName);
    return (SpriteRenderer *)gameObject->GetComponent("SpriteRenderer");
}
