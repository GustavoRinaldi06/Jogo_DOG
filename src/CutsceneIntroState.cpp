#include "CutsceneIntroState.h"
#include "Game.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "ObjectFactory.h"
#include <iostream>

CutsceneIntroState::CutsceneIntroState(std::function<State *()> nextStateFactory)
    : createNext(std::move(nextStateFactory)), grilos("recursos/audio/CutsceneIntro/SonsFloresta.mp3")
{
    for (int i = 0; i < 8; ++i) {
        scenes[i] = false;
    }
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

    sounds.emplace("dog", new Sound("recursos/audio/DOG/panting.wav"));
    sounds.emplace("leaves", new Sound("recursos/audio/CutsceneIntro/leaves.wav"));
    sounds.emplace("shot", new Sound("recursos/audio/CutsceneIntro/shot.wav"));

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
    
    if(input.KeyPress(SDLK_RETURN))
    {
        CallNextState();
        return;
    }

    timer.Update(dt);
    if (timer.Between(0.f, 5.f) && scenes[0] == false)
    {
        if(scenes[0] == false){
            scenes[0] = true;
            ShowImage("primeira_cena");
            ShowImage("trees");
            sounds.at("dog")->Play(10); // Toca o som do cachorro
            sounds.at("dog")->SetVolume(15); // Define o volume do som do cachorro
        }
        ScaleImageFixed("trees", Vec2{0.05f, 0.05f} * dt); // Ajuste de escala, se necessário
    }
    else if (timer.Between(5.f, 9.f) && scenes[1] == false)
    {
        scenes[1] = true;
        HideImage("primeira_cena");
        HideImage("trees");
        ShowImage("dog");
        sounds.at("dog")->SetVolume(40); // Define o volume do som do cachorro
    }
    else if (timer.Between(9.f, 12.f) && scenes[2] == false)
    {
        scenes[2] = true;
        HideImage("dog");
        ShowImage("hunter_1");
        sounds.at("dog")->SetVolume(15); // Define o volume do som do cachorro
        //sounds.at("dog")->Stop(); // Para o som do cachorro
    }
    else if (timer.Between(12.f, 14.f) && scenes[3] == false)
    {
        scenes[3] = true;
        sounds.at("leaves")->Play(1); // Toca o som de folhas
        HideImage("hunter_1");
        ShowImage("hunter_2");
    }
    else if (timer.Between(14.f, 16.f) && scenes[4] == false)
    {
        scenes[4] = true;
        HideImage("hunter_2");
        ShowImage("turn_1");
        ShowImage("background");
    }
    else if (timer.Between(16.f, 17.f) && scenes[5] == false)
    {
        scenes[5] = true;
        HideImage("turn_1");
        ShowImage("turn_2");
    }
    else if (timer.Between(17.f, 19.f) && scenes[6] == false)
    {
        scenes[6] = true;
        HideImage("turn_2");
        HideImage("background");
        sounds.at("dog")->Stop(); // Para o som de folhas
        grilos.Stop(); // Para a música de grilos
        sounds.at("shot")->Play(); // Toca o som de tiro
    }
    else if (timer.Get() > 19.f && scenes[7] == false)
    {
        scenes[7] = true;
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
    grilos.Play(-1, 2000);
}

void CutsceneIntroState::CallNextState()
{
    State *next = createNext();     // Cria o próximo estado
    Game::GetInstance().Push(next); // Empilha o novo estado
    popRequested = true;
}

void CutsceneIntroState::ShowImage(const std::string &imageName)
{
    SpriteRenderer *sprite = GetSpriteRenderer(imageName);
    if (sprite)
    {
        sprite->setActive(true); // Define a transparência para 255 (totalmente visível)
    }
}

void CutsceneIntroState::HideImage(const std::string &imageName)
{
    SpriteRenderer *sprite = GetSpriteRenderer(imageName);
    if (sprite)
    {
        sprite->setActive(false); // Define a transparência para 0 (totalmente invisível)
    }
}

void CutsceneIntroState::ScaleImageFixed(const std::string &imageName, Vec2 amount)
{
    SpriteRenderer *sprite = GetSpriteRenderer(imageName);
    if (sprite)
    {
        Vec2 newScale = sprite->GetScale() + amount; // Incrementa o scale atual
        sprite->SetScale(newScale.x, newScale.y); // Define o scale do sprite
    }
}

SpriteRenderer *CutsceneIntroState::GetSpriteRenderer(const std::string &imageName)
{
    if (images.find(imageName) == images.end())
    {
        std::cerr << "Image not found: " << imageName << std::endl;
        return nullptr;
    }
    GameObject *gameObject = images.at(imageName);
    return (SpriteRenderer *)gameObject->GetComponent("SpriteRenderer");
}