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
    for (size_t i = 0; i < images.size(); i++)
    {
        if(timer.Between(i * 2.f, (i + 1) * 2.f))
        {
            ShowImage(std::to_string(i + 1));
            if(i > 0){
                HideImage(std::to_string(i));
            }
        }
    }
    if (timer.Get() > images.size() * 2.f)
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
    State *next = createNext();     // Cria o próximo estado
    Game::GetInstance().Push(next); // Empilha o novo estado
    popRequested = true;
}



void CutsceneGhostHunterState::ShowImage(const std::string &imageName)
{
    SpriteRenderer *sprite = GetSpriteRenderer(imageName);
    if (sprite)
    {
        std::cout << "Showing image: " << imageName << std::endl;
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
