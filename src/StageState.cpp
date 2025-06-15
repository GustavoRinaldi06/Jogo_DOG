#include "../include/StageState.h"
#include "../include/TileSet.h"
#include "../include/SpriteRenderer.h"
#include "../include/TileMap.h"
#include "../include/InputManager.h"
#include "../include/Camera.h"
#include "../include/Character.h"
#include "../include/PlayerController.h"
#include "../include/Collider.h"
#include "../include/Collision.h"
#include "Text.h"

#define INCLUDE_SDL
#include "SDL_include.h"

#include <iostream>
#include <algorithm>

StageState::StageState() {}

StageState::~StageState()
{
    objectArray.clear(); // Limpa os GameObjects automaticamente graças ao unique_ptr
}


void StageState::LoadAssets()
{
    std::cout << "\n Carregando DOG:" << "\n"; // Alertar LoadAssets

    // Fundo -------------------------------------------------------------------------------------------------------------------
    GameObject *bgObject = new GameObject();
    SpriteRenderer *bgRenderer = new SpriteRenderer(*bgObject);
    bgRenderer->Open("recursos/img/FundoTest.jpg");
    bgRenderer->SetCameraFollower(false);

    bgObject->box.x = 0;
    bgObject->box.y = 0;

    bgObject->AddComponent(bgRenderer);
    AddObject(bgObject);

    // Mapa --------------------------------------------------------------------------------------------------------------------

    // Cria o chão
    GameObject *ground = new GameObject();
    ground->box.x = 0;
    ground->box.y = 700;
    ground->box.w = 1920;
    ground->box.h = 50;

    Collider *groundCollider = new Collider(*ground);
    groundCollider->tag = "ground";
    ground->AddComponent(groundCollider);


    AddObject(ground); 

    // Personagem ----------------------------------------------------------------------------------------------------------------
    GameObject *playerGO = new GameObject();
    playerGO->box.x = 600;  // Centro do mapa
    playerGO->box.y = 650; // 

    playerGO->AddComponent(new Character(*playerGO, "recursos/img/Player.png")); // substitua pela imagem correta
    playerGO->AddComponent(new PlayerController(*playerGO));

    Camera::GetInstance().Follow(playerGO);

    AddObject(playerGO);
    // Música --------------------------------------------------------------------------------------------------------------------

    backgroundMusic.Open("recursos/audio/Fundo.mp3");
    backgroundMusic.Play();

    // Texto da vida do personagem------------------------------------------------------------------------------------------------

    SDL_Color white = {255, 255, 255, 255};
    GameObject *textGO = new GameObject();
    std::string hpString = "HP: " + std::to_string(Character::player->GetHP());
    hpText = new Text(*textGO, "recursos/font/neodgm.ttf", 24, BLENDED, hpString, white);
    textGO->AddComponent(hpText);

    hpText->SetCameraFollower(true);
    // Posição do texto
    textGO->box.x = 60;
    textGO->box.y = 650;

    AddObject(textGO);

    // TExto de cooldown do cachorro --------------------------------------------------------------------------------------------

    GameObject *textGO1 = new GameObject();
    std::string dgCooldown = "DOG esta entre nos";
    dogText = new Text(*textGO1, "recursos/font/neodgm.ttf", 24, BLENDED, dgCooldown, white);
    textGO1->AddComponent(dogText);

    dogText->SetCameraFollower(true);
    // Posição do texto
    textGO1->box.x = 200;
    textGO1->box.y = 650;

    AddObject(textGO1);
}

void StageState::Update(float dt)
{
    InputManager &input = InputManager::GetInstance();

    Camera::GetInstance().Update(dt);

    // Se o jogador pressionou ESC ou clicou no X ou esc
    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY))
    {
        quitRequested = true;
        return;
    }

    // Atualiza todos os GameObjects
    UpdateArray(dt);

    // Atualiza colisores
    for (auto &obj : objectArray)
    {
        Collider *collider = (Collider *)obj->GetComponent("Collider");
        if (collider)
            collider->Update(0); // forçando update
    }

    // Verifica colisões dos GameObjects
    for (size_t i = 0; i < objectArray.size(); ++i)
    {
        GameObject *objA = objectArray[i].get();
        Component *colCompA = objA->GetComponent("Collider");
        if (!colCompA || objA->IsDead())
            continue;

        Collider *colA = static_cast<Collider *>(colCompA);

        for (size_t j = i + 1; j < objectArray.size(); ++j)
        {
            GameObject *objB = objectArray[j].get();
            Component *colCompB = objB->GetComponent("Collider");
            if (!colCompB || objB->IsDead())
                continue;

            Collider *colB = static_cast<Collider *>(colCompB);

            if (IsColliding(colA->box, objA->angleDeg * M_PI / 180, colB->box, objB->angleDeg * M_PI / 180))
            {
                if (!objA->IsDead() && !objB->IsDead())
                {
                    objA->NotifyCollision(*objB);

                    // Se algum morreu, não faz a outra colisão
                    if (!objA->IsDead() && !objB->IsDead())
                    {
                        objB->NotifyCollision(*objA);
                    }
                }
            }
        }
    }

    // Remove os mortos
    for (size_t i = 0; i < objectArray.size();)
    {
        if (objectArray[i]->IsDead())
        {
            objectArray.erase(objectArray.begin() + i);
        }
        else
        {
            ++i;
        }
    }

    //CAixa de texto de vida
    if (hpText && Character::player != nullptr){
        std::string hpString = "HP: " + std::to_string(Character::player->GetHP());
        hpText->SetText(hpString);
    }

    //Caixa do cooldown do DOG
    if (dogText && Character::player != nullptr){
        if(Character::player->GetCool() < 5){
            std::string dgCooldown = "DOG esta passeando: " + std::to_string(5 - Character::player->GetCool());
            dogText->SetText(dgCooldown);
        }else{
            std::string dgCooldown = "DOG esta entre nos";
            dogText->SetText(dgCooldown);
        }
    }

    // Checagem de fim Derrota
    if (Character::player == nullptr || Character::player->GetGameObject()->IsDead()) // Se o player tiver morrido
    {
        GameData::playerVictory = false;
        popRequested = true;
        Game::GetInstance().Push(new EndState());
        return;
    }
}

void StageState::Render()
{
    SDL_Renderer *renderer = Game::GetInstance().GetRenderer();

    // Limpa tudo para o próximo frame
    SDL_RenderClear(renderer);

    // Desenha todos os game objects
    RenderArray();

    // Apresenta o frame
    SDL_RenderPresent(renderer);
}


void StageState::Start()
{
    LoadAssets();
    StartArray(); // Agora a classe state é responsável
    started = true;
}

void StageState::Pause()
{
   // Não sei ainda
}

void StageState::Resume()
{
  // Não sei ainda
}