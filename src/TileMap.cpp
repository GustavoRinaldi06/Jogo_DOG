#include "TileMap.h"
#include "Game.h"
#include "GameObject.h"
#include "Camera.h"
#include "State.h"
#include "Collider.h"
#include "tmxlite/TileLayer.hpp"
#include "GameData.h"
#include <fstream>
#include <sstream> 
#include <iostream>


TileMap::TileMap(GameObject& associated, TileSet* tileSet, const tmx::Map& map) : Component(associated)
{
    this->tileSet = std::unique_ptr<TileSet>(tileSet);
    LoadFromTMX(map);
}

void TileMap::SetTileSet(TileSet *tileSet)
{
    this->tileSet.reset(tileSet);
}

int &TileMap::At(int x, int y, int z)
{
    int index = x + (y * mapWidth) + (z * mapWidth * mapHeight);
    return tileMatrix[index]; // Retorna o tile na posição (x, y, z)
}

void TileMap::Render()
{
    for (int i = 0; i < mapDepth; ++i)
    {
        float parallaxFactor = 1.0f + (i * 0.2f); //  fundo se move mais devagar
        RenderLayer(i, parallaxFactor);
    }
}

void TileMap::RenderLayer(int layer, float parallaxFactor)
{
    int tileW = tileSet->GetTileWidth();
    int tileH = tileSet->GetTileHeight();

    // Garante que a câmera será aplicada manualmente
    tileSet->SetCameraFollower(false);

    // Calcula deslocamento da câmera com parallax
    Vec2 cam = Camera::GetInstance().GetPosition();

    for (int y = 0; y < mapHeight; ++y)
    {
        for (int x = 0; x < mapWidth; ++x)
        {
            int index = At(x, y, layer);
            if (index != -1)
            {
                //  posição do tile
                float worldX = associated.box.x + x * tileW;
                float worldY = associated.box.y + y * tileH;

                // adicona o parallax e a camera
                float posX = worldX;
                float posY = worldY;

                // ! Bug de altura dos tiles estava nessa linha
                // ! precisa investigar se o fator de camera e parallax deveriam de fato serem aplicados aqui
                //float posY = worldY - cam.y * parallaxFactor;


                tileSet->RenderTile(index, posX, posY);
            }
        }
    }
}

int TileMap::GetWidth() const
{
    return mapWidth;
}

int TileMap::GetHeight() const
{
    return mapHeight;
}

int TileMap::GetDepth() const
{
    return mapDepth;
}


// Importadas de Component
void TileMap::Update(float dt)
{
    // TileMap é estático, então talvez nada precise ser feito aqui
}

bool TileMap::Is(const std::string &type)
{
    return type == "TileMap";
}

void TileMap::GenerateCollision(int collisionLayer, State &state)
{
    int tileW = tileSet->GetTileWidth();
    int tileH = tileSet->GetTileHeight();

    for (int y = 0; y < mapHeight; ++y)
    {
        for (int x = 0; x < mapWidth; ++x)
        {
            int index = At(x, y, collisionLayer);
            if(GameData::state == 2 && index == 7) continue;
            if (index != -1)
            {
                GameObject *block = new GameObject();
                block->box.x = associated.box.x + x * tileW;
                block->box.y = associated.box.y + y * tileH - 100;
                block->box.w = tileW;
                block->box.h = tileH;

                auto c = new Collider(*block);
                c->tag = "ground"; 
                block->AddComponent(c);
                state.AddObject(block);
            }
        }
    }
}

void TileMap::LoadFromTMX(const tmx::Map& map) {
    const auto& layers = map.getLayers();
    // 1) Conta quantos tile-layers existem e pega tamanho de cada tile-layer (assume todos mesmos dims)
    int width = 0, height = 0, depth = 0;
    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Tile) {
            ++depth;
            width  = layer->getSize().x;
            height = layer->getSize().y;
        }
    }
    if (depth == 0) {
        std::cerr << "Nenhum layer de tile encontrado.\n";
        return;
    }

    // 2) Ajusta membros da classe e prepara a matriz
    mapWidth  = width;
    mapHeight = height;
    mapDepth  = depth;
    tileMatrix.clear();
    tileMatrix.resize(width * height * depth, -1);

    // 3) Preenche camada por camada
    int z = 0;
    for (const auto& layer : layers) {
        if (layer->getType() != tmx::Layer::Type::Tile) continue;

        const auto& tiles = layer->getLayerAs<tmx::TileLayer>().getTiles();
        if (tiles.empty()) {
            std::cerr << "Layer '" << layer->getName() << "' sem dados de tiles.\n";
        } else {
            for (int idx = 0; idx < (int)tiles.size(); ++idx) {
                int x = idx % width;
                int y = idx / width;
                int id = tiles[idx].ID;

                if (id == 0) {
                    tileMatrix[(z * height + y) * width + x] = -1;
                } else {
                    tileMatrix[(z * height + y) * width + x] = id - 1;
                }
            }
        }
        ++z;
    }

     // DEBUG - Mostrar valores reais
    std::cout << "Valores reais na matriz:\n";
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            std::cout << At(x, y, 0) << " ";
        }
        std::cout << "\n";
    }

    std::cout << "Map loaded: " 
              << mapWidth << "x" << mapHeight 
              << "x" << mapDepth << "\n";
}