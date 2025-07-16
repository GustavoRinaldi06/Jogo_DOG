#include "TreeState.h"
#include "TileSet.h"
#include "SpriteRenderer.h"
#include "TileMap.h"
#include "InputManager.h"
#include "Camera.h"
#include "Character.h"
#include "PlayerController.h"
#include "Collider.h"
#include "Collision.h"
#include "Parallax.h"
#include "DamageObj.h"
#include "Chainsaw.h"
#include "AnimalState.h"
#include "LoadingState.h"
#include "Text.h"
#include "windows.h"
#include "ObjectFactory.h" 

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

    LoadLayers();
    LoadFromTMX("recursos/map/Tree/mapfile.tmx");

    // Música --------------------------------------------------------------------------------------------------------------------
    backgroundMusic.Open("recursos/audio/BGmusic/treeState.mp3");
    backgroundMusic.Play();

    // Vinheta ----------------------------------------------------------------------------------------------------------------
    GameObject *vinhetaGO = new GameObject();

    SpriteRenderer *spriter = new SpriteRenderer(*vinhetaGO);
    spriter->Open("recursos/img/tree/Vinheta.png");
    spriter->SetCameraFollower(true); // igual a imagem de fundo

    vinhetaGO->AddComponent(spriter);
    AddObject(vinhetaGO);

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

    // Texto de cooldown do cachorro --------------------------------------------------------------------------------------------
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
    // PARA TESTES ==================================================================================================
    if (input.QuitRequested() || input.KeyPress('r'))
    {
        std::vector<std::string> animalAssets = {
            // background
            "recursos/img/background/Animal/C.png",
            "recursos/img/background/Animal/B.png",
            "recursos/img/background/Animal/A.png",
            "recursos/map/Animal/tiles.png",
            // sprites
            "recursos/img/sprites/Player.png",
            "recursos/img/sprites/Hand.png", 
            "recursos/img/sprites/Thorn.png", 
            "recursos/img/sprites/Chainsaw.png",
            "recursos/img/sprites/DogHowling.png",
            "recursos/img/sprites/DogShoot.png",
            // sons
            "recursos/audio/BGmusic/treeState.mp3",
            "recursos/audio/Hunter/boing.mp3", 
            "recursos/audio/DOG/explode.mp3",
            "recursos/audio/Hunter/boing.mp3", 
            "recursos/audio/DOG/explode.mp3",
            "recursos/audio/Hunter/boing.mp3", 
            "recursos/audio/DOG/explode.mp3",
        };

        GameData::state = 2;
        popRequested = true;
        Game::GetInstance().Push(new LoadingState([](){ 
            return new AnimalState(); 
        }, animalAssets));
    }

    branchTimer.Update(dt);
    if (branchTimer.Get() > 3.0f) { // a cada 3 segundos
        GameObject* branchGO = ObjectFactory::CreateFallingBranchObject(tmx::Object());
        AddObject(branchGO);
        std::cout << "Falling Branch object created at position: " << branchGO->box.x << ", " << branchGO->box.y << std::endl;
        branchTimer.Restart();
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

    //CAixa de texto de vida
    if (hpText && Character::player != nullptr){
        std::string hpString = "HP: " + std::to_string(Character::player->GetHP());
        hpText->SetText(hpString);
    }

    //Caixa do cooldown do DOG
    if (dogText && Character::player != nullptr){
        if(Character::player->GetCool() < 4){
            std::string dgCooldown = "DOG esta passeando: " + std::to_string(4 - Character::player->GetCool());
            dogText->SetText(dgCooldown);
        }else{
            std::string dgCooldown = "DOG esta entre nos";
            dogText->SetText(dgCooldown);
        }
    }

    // Checagem de fim Derrota
    if (Character::player == nullptr || Character::player->GetGameObject()->IsDead()) // Se o player tiver morrido
    {
        GameData::playerVictory = true;
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
{}

void TreeState::Resume()
{
  // Não sei ainda
}

void TreeState::LoadLayers()
{
    // Camada E (mais distante)
    GameObject *E = new GameObject();
    E->box.x = 0;
    E->box.y = 0;
    E->box.w = 2048;
    E->box.h = 512;
    E->AddComponent(new SpriteRenderer(*E, "recursos/img/background/Tree/E.png"));
    E->AddComponent(new Parallax(*E, 0.0f));
    AddObject(E);

    // Camada D
    GameObject *D = new GameObject();
    D->box.x = 0;
    D->box.y = 0;
    D->box.w = 2048;
    D->box.h = 512;
    D->AddComponent(new SpriteRenderer(*D, "recursos/img/background/Tree/D.png"));
    D->AddComponent(new Parallax(*D, 0.2f));
    AddObject(D);

    // Camada C
    GameObject *C = new GameObject();
    C->box.x = 0;
    C->box.y = 0;
    C->box.w = 2048;
    C->box.h = 512;
    C->AddComponent(new SpriteRenderer(*C, "recursos/img/background/Tree/C.png"));
    C->AddComponent(new Parallax(*C, 0.3f));
    AddObject(C);

    // Camada B
    GameObject *B = new GameObject();
    B->box.x = 0;
    B->box.y = 0;
    B->box.w = 2048;
    B->box.h = 512;
    B->AddComponent(new SpriteRenderer(*B, "recursos/img/background/Tree/B.png"));
    B->AddComponent(new Parallax(*B, 0.4f));
    AddObject(B);

    // Camada A
    GameObject *A = new GameObject();
    A->box.x = 0;
    A->box.y = 0;
    A->box.w = 2048;
    A->box.h = 512;
    A->AddComponent(new SpriteRenderer(*A, "recursos/img/background/Tree/A.png"));
    A->AddComponent(new Parallax(*A, 0.6f));
    AddObject(A);
}

const std::array<std::string, 4u> LayerStrings =
{
    std::string("Chainsaw"),
    std::string("Background"),
    std::string("Hand"),
};

void TreeState::LoadFromTMX(std::string file)
{
    tmx::Map map;
    if (map.load(file))
    {
        GameObject* go = new GameObject();
        go->box.x = 0;
        go->box.y = 0;

        TileSet* tileSet = new TileSet(
            250,
            250,
            "recursos/map/Tree/tiles.png"
        );
        TileMap* tileMap = new TileMap(*go, tileSet, map);
        go->AddComponent(tileMap);
        AddObject(go);

        // DEBUG -- verificar tamanhos certos
        std::cout << "TileSet carregado: " << tileSet->GetTileWidth() << "x" << tileSet->GetTileHeight() << "\n";
        std::cout << "TileMap carregado: " << tileMap->GetWidth() << "x" << tileMap->GetHeight() << "x" << tileMap->GetDepth() << "\n \n";

        // gera colisão da camada 0 (chão)
        //tileMap->GenerateCollision(0, *this);

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

void TreeState::CreateGameObject(const tmx::Object& object)
{
    if(object.getClass() == "Collider")
    {
        GameObject* colliderGO = ObjectFactory::CreateColliderObject(object);
        AddObject(colliderGO);
        std::cout << "Collider object created at position: " << colliderGO->box.x << ", " << colliderGO->box.y << std::endl;
        return;
    }

    if(object.getClass() == "Hand")
    {
        GameObject* handGO = ObjectFactory::CreateHandObject(object);
        AddObject(handGO);
        std::cout << "Hand object created at position: " << handGO->box.x << ", " << handGO->box.y << std::endl;
        return;
    }

    if(object.getClass() == "Espinho")
    {
        GameObject* thornGO = ObjectFactory::CreateThornObject(object);
        AddObject(thornGO);
        std::cout << "Thorn object created at position: " << thornGO->box.x << ", " << thornGO->box.y << std::endl;
        return;
    }

    if(object.getClass() == "Chainsaw")
    {
        GameObject* chainSawGO = ObjectFactory::CreateChainSawObject(object);
        AddObject(chainSawGO);
        std::cout << "Chainsaw object created at position: " << chainSawGO->box.x << ", " << chainSawGO->box.y << std::endl;
        return;
    }

    if(object.getClass() == "Player")
    {
        GameObject* playerGO = ObjectFactory::CreatePlayerObject(object);
        AddObject(playerGO);
        std::cout << "Player object created at position: " << playerGO->box.x << ", " << playerGO->box.y << std::endl;
        std::cout << "playerGO->box.y = " << playerGO->box.y << "\n";
        std::cout << "playerGO->box.x = " << playerGO->box.x << "\n";
        std::cout << "playerGO->box.h = " << playerGO->box.h << "\n";
        return;
    }
}