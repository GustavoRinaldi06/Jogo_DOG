#include "../include/TreeState.h"
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
#include "Text.h"

#define INCLUDE_SDL
#include "SDL_include.h"

#include <iostream>
#include <algorithm>

TreeState::TreeState() {}

TreeState::~TreeState()
{
    objectArray.clear(); // Limpa os GameObjects automaticamente graças ao unique_ptr
}

void TreeState::LoadAssets()
{
    std::cout << "\n Carregando DOG a Tree fase:" << "\n"; // Alertar LoadAssets

    // Fundo -------------------------------------------------------------------------------------------------------------------
    LoadLayers();

    LoadFromTMX("recursos/map/fase_1/mapfile.tmx");

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
    playerGO->box.y = -950;

    playerGO->AddComponent(new Character(*playerGO, "recursos/img/Player.png"));
    playerGO->AddComponent(new PlayerController(*playerGO));

    Camera::GetInstance().Follow(playerGO);
    AddObject(playerGO);

    std::cout << "playerGO->box.y = " << playerGO->box.y << "\n";
    std::cout << "playerGO->box.h = " << playerGO->box.h << "\n";

    // Música --------------------------------------------------------------------------------------------------------------------

    backgroundMusic.Open("recursos/audio/Fundo.mp3");
    backgroundMusic.Play();

    // Foreground ----------------------------------------------------------------------------------------------------------------
    /*
        GameObject *A = new GameObject();
        A->box.x = 0;
        A->box.y = 500;
        A->box.w = 2048;
        A->box.h = 512;
        A->AddComponent(new SpriteRenderer(*A, "recursos/img/Tree/A.png"));
        A->AddComponent(new Parallax(*A, 0.6f));
        AddObject(A);
    */
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

void TreeState::LoadLayers()
{
    // Camada E (mais distante)
    GameObject *E = new GameObject();
    E->box.x = 0;
    E->box.y = 0;
    E->box.w = 2048;
    E->box.h = 512;
    E->AddComponent(new SpriteRenderer(*E, "recursos/img/Tree/E.png"));
    E->AddComponent(new Parallax(*E, 0.0f));
    AddObject(E);

    // Camada D
    GameObject *D = new GameObject();
    D->box.x = 0;
    D->box.y = 0;
    D->box.w = 2048;
    D->box.h = 512;
    D->AddComponent(new SpriteRenderer(*D, "recursos/img/Tree/D.png"));
    D->AddComponent(new Parallax(*D, 0.2f));
    AddObject(D);

    // Camada C
    GameObject *C = new GameObject();
    C->box.x = 0;
    C->box.y = 0;
    C->box.w = 2048;
    C->box.h = 512;
    C->AddComponent(new SpriteRenderer(*C, "recursos/img/Tree/C.png"));
    C->AddComponent(new Parallax(*C, 0.3f));
    AddObject(C);

    // Camada B
    GameObject *B = new GameObject();
    B->box.x = 0;
    B->box.y = 0;
    B->box.w = 2048;
    B->box.h = 512;
    B->AddComponent(new SpriteRenderer(*B, "recursos/img/Tree/B.png"));
    B->AddComponent(new Parallax(*B, 0.4f));
    AddObject(B);
}

void TreeState::Update(float dt)
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

void TreeState::Render()
{
    SDL_Renderer *renderer = Game::GetInstance().GetRenderer();

    // Limpa tudo para o próximo frame
    SDL_RenderClear(renderer);

    // Desenha todos os game objects
    RenderArray();

    // Apresenta o frame
    SDL_RenderPresent(renderer);
}

void TreeState::Start()
{
    LoadAssets();
    StartArray(); // Agora a classe state é responsável
    started = true;
}

void TreeState::Pause()
{
   // Não sei ainda
}

void TreeState::Resume()
{
  // Não sei ainda
}

const std::array<std::string, 4u> LayerStrings =
{
    std::string("Tree"),
    std::string("Background"),
};

void TreeState::LoadFromTMX(std::string file)
{
    tmx::Map map;
    if (map.load(file))
    {
        GameObject* go = new GameObject();
        go->box.x = 0;
        go->box.y = 0;

        auto tileSize = map.getTileSize();
        TileSet* tileSet = new TileSet(
            static_cast<int>(tileSize.x),
            static_cast<int>(tileSize.y),
            "recursos/img/Tree/tile.png"
        );
        TileMap* tileMap = new TileMap(*go, tileSet, map);
        go->AddComponent(tileMap);
        AddObject(go);

        // DEBUG -- verificar tamanhos certos
        std::cout << "TileSet carregado: " << tileSet->GetTileWidth() << "x" << tileSet->GetTileHeight() << "\n";
        std::cout << "TileMap carregado: " << tileMap->GetWidth() << "x" << tileMap->GetHeight() << "x" << tileMap->GetDepth() << "\n \n";

        // gera colisão da camada 0 (chão)
        tileMap->GenerateCollision(0, *this);

        std::cout << "Loaded Map version: " << map.getVersion().upper << ", " << map.getVersion().lower << std::endl;
        if (map.isInfinite())
        {
            std::cout << "Map is infinite.\n";
        }
        else
        {
            std::cout << "Map Dimensions: " << map.getBounds() << std::endl;
        }

        const auto& layers = map.getLayers();
        std::cout << "Map has " << layers.size() << " layers" << std::endl;
        for (const auto& layer : layers)
        {
            std::cout << "Found Layer: " << layer->getName() << std::endl;
            std::cout << "Layer Type: " << LayerStrings[static_cast<std::int32_t>(layer->getType())] << std::endl;
            std::cout << "Layer Dimensions: " << layer->getSize() << std::endl;
            std::cout << "Layer Tint: " << layer->getTintColour() << std::endl;

            if (layer->getType() == tmx::Layer::Type::Object)
            {
                const auto& objects = layer->getLayerAs<tmx::ObjectGroup>().getObjects();
                std::cout << "Found " << objects.size() << " objects in layer" << std::endl;
                for (const auto& object : objects)
                {
                    std::cout << "Object " << object.getUID() << ", " << object.getName() << std::endl;
                    const auto& properties = object.getProperties();
                    std::cout << "Object has " << properties.size() << " properties" << std::endl;
                    for (const auto& prop : properties)
                    {
                        std::cout << "Found property: " << prop.getName() << std::endl;
                        std::cout << "Type: " << int(prop.getType()) << std::endl;
                    }
                }
            }
        }
    }
    else
    {
        std::cout << "Failed loading map" << std::endl;
    }
}
