#include "../include/AnimalState.h"
#include "../include/TileSet.h"
#include "../include/SpriteRenderer.h"
#include "../include/TileMap.h"
#include "../include/InputManager.h"
#include "../include/Camera.h"
#include "../include/Character.h"
#include "../include/PlayerController.h"
#include "../include/Collider.h"
#include "../include/Collision.h"
#include "../include/Parallax.h"
#include "SmokeState.h"
#include "LoadingState.h"
#include "Text.h"

#define INCLUDE_SDL
#include "SDL_include.h"

#include <iostream>
#include <algorithm>

AnimalState::AnimalState() {}

AnimalState::~AnimalState()
{
    objectArray.clear(); // Limpa os GameObjects automaticamente graças ao unique_ptr
}

void AnimalState::LoadAssets()
{
    std::cout << "\n Carregando DOG a Animal fase:" << "\n"; // Alertar LoadAssets

    // Fundo -------------------------------------------------------------------------------------------------------------------
    LoadLayers();

    // Carrega o mapa do TMX
    LoadFromTMX("recursos/map/Animal/mapfile.tmx");

    // Cria limite de mapa esquerda ------------------------------------------
    GameObject *leftlimit = new GameObject();
    leftlimit->box.x = 0;
    leftlimit->box.y = 0;
    leftlimit->box.w = 500;
    leftlimit->box.h = 1500;

    Collider *wallCollider = new Collider(*leftlimit);
    wallCollider->tag = "wall";
    leftlimit->AddComponent(wallCollider);

    AddObject(leftlimit);

    // Personagem ----------------------------------------------------------------------------------------------------------------
    GameObject *playerGO = new GameObject();
    playerGO->box.x = 500;
    playerGO->box.y = 350;

    playerGO->AddComponent(new Character(*playerGO, "recursos/img/sprites/Player.png"));
    playerGO->AddComponent(new PlayerController(*playerGO));

    Camera::GetInstance().Follow(playerGO);
    AddObject(playerGO);

    std::cout << "playerGO->box.y = " << playerGO->box.y << "\n";
    std::cout << "playerGO->box.h = " << playerGO->box.h << "\n";

    // Música --------------------------------------------------------------------------------------------------------------------
    backgroundMusic.Open("recursos/audio/BGmusic/animalState.wav");
    backgroundMusic.Play();

    // Texto da vida do personagem------------------------------------------------------------------------------------------------
    SDL_Color white = {255, 255, 255, 255};
    GameObject *textGO = new GameObject();
    std::string hpString = "HP: " + std::to_string(GameData::playerHP);
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

void AnimalState::Update(float dt)
{
    InputManager &input = InputManager::GetInstance();

    Camera::GetInstance().Update(dt);

    // Se o jogador pressionou ESC ou clicou no X
    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY))
    {
        quitRequested = true;
        return;
    }
    // PARA TESTES ==================================================================================================
    if (input.QuitRequested() || input.KeyPress('r'))
    {
        std::vector<std::string> smokeAssets = {
            "recursos/img/Smoke/C.png",
            "recursos/img/Smoke/B.png",
            "recursos/img/Smoke/A.png",
            "recursos/img/Smoke/tile.png",
            "recursos/img/sprites/Player.png"};

        popRequested = true;
        Game::GetInstance().Push(new LoadingState([](){ return new SmokeState(); }, smokeAssets));
    }
    // =============================================================================================================

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

    // CAixa de texto de vida
    if (hpText && Character::player != nullptr)
    {
        std::string hpString = "HP: " + std::to_string(Character::player->GetHP());
        hpText->SetText(hpString);
    }

    // Caixa do cooldown do DOG
    if (dogText && Character::player != nullptr)
    {
        if (Character::player->GetCool() < 4)
        {
            std::string dgCooldown = "DOG esta passeando: " + std::to_string(4 - Character::player->GetCool());
            dogText->SetText(dgCooldown);
        }
        else
        {
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

void AnimalState::Render()
{
    SDL_Renderer *renderer = Game::GetInstance().GetRenderer();

    // Limpa tudo para o próximo frame
    SDL_RenderClear(renderer);

    // Desenha todos os game objects
    RenderArray();

    // Apresenta o frame
    SDL_RenderPresent(renderer);
}

void AnimalState::Start()
{
    LoadAssets();
    StartArray(); // Agora a classe state é responsável
    started = true;
}

void AnimalState::Pause()
{
    // Não sei ainda
}

void AnimalState::Resume()
{
    // Não sei ainda
}

const std::array<std::string, 4u> LayerStrings =
{
    std::string("Background")
};

void AnimalState::LoadLayers() {
    // Camada C
    GameObject *C = new GameObject();
    C->box.x = 0;
    C->box.y = 0;
    C->box.w = 2048;
    C->box.h = 512;
    C->AddComponent(new SpriteRenderer(*C, "recursos/img/background/Animal/C.png"));
    C->AddComponent(new Parallax(*C, 0.6f));
    AddObject(C);

    // Camada B
    GameObject *B = new GameObject();
    B->box.x = 0;
    B->box.y = 0;
    B->box.w = 2048;
    B->box.h = 512;
    B->AddComponent(new SpriteRenderer(*B, "recursos/img/background/Animal/B.png"));
    B->AddComponent(new Parallax(*B, 0.4f));
    AddObject(B);

    // Camada A
    GameObject *A = new GameObject();
    A->box.x = 0;
    A->box.y = 500;
    A->box.w = 2048;
    A->box.h = 512;
    A->AddComponent(new SpriteRenderer(*A, "recursos/img/background/Animal/A.png"));
    A->AddComponent(new Parallax(*A, 0.2f));
    AddObject(A);
}

void AnimalState::LoadFromTMX(std::string file)
{
    tmx::Map map;
    if (map.load(file))
    {
        GameObject* go = new GameObject();
        go->box.x = 0;
        go->box.y = 0;

        TileSet* tileSet = new TileSet(
            249,
            249,
            "recursos/map/Animal/tiles.png"
        );
        TileMap* tileMap = new TileMap(*go, tileSet, map);
        go->AddComponent(tileMap);
        AddObject(go);

        // DEBUG -- verificar tamanhos certos
        std::cout << "TileSet carregado: " << tileSet->GetTileWidth() << "x" << tileSet->GetTileHeight() << "\n";
        std::cout << "TileMap carregado: " << tileMap->GetWidth() << "x" << tileMap->GetHeight() << "x" << tileMap->GetDepth() << "\n \n";

        // gera colisão da camada 0 (chão)
        tileMap->GenerateCollision(0, *this);

        const auto& layers = map.getLayers();
        for (const auto& layer : layers)
        {
            if (layer->getType() == tmx::Layer::Type::Object)
            {
                const auto& objects = layer->getLayerAs<tmx::ObjectGroup>().getObjects();
                for (const auto& object : objects)
                {
                    CreateGameObject(object);
                }
            }
        }
    }
    else
    {
        std::cout << "Failed loading map" << std::endl;
    }
}

void AnimalState::CreateGameObject(const tmx::Object& object)
{
    
}