#include "../include/TileMap.h"
#include "../include/Game.h"
#include "../include/GameObject.h"
#include "../include/Camera.h"
#include "../include/State.h"
#include "../include/Collider.h"
#include "../include/tmxlite/TileLayer.hpp"

#include <fstream>
#include <sstream> 
#include <iostream>

TileMap::TileMap(GameObject &associated, const std::string &file, TileSet *tileSet):Component(associated), mapWidth(0), mapHeight(0), mapDepth(0)
{
    Load(file);
}

TileMap::TileMap(GameObject& associated, TileSet* tileSet, const tmx::Map& map) : Component(associated)
{
    this->tileSet = std::unique_ptr<TileSet>(tileSet);
    LoadFromTMX(map);
}

void TileMap::Load(const std::string &file)
{
    std::ifstream mapFile(file);
    char comma; // pular a vírgula

    if (mapFile.is_open())
    {
        // Lê as dimensões do mapa
        mapFile >> mapWidth >> comma >> mapHeight >> comma >> mapDepth >> comma;

        // Verifica se as dimensões foram lidas corretamente (DEBUG)
        if (mapFile.fail())
        {
            std::cerr << "Erro ao ler as dimensões do mapa no arquivo: " << file << std::endl;
            return;
        }

        // Cria a matriz de tiles para o mapa
        tileMatrix.resize(mapWidth * mapHeight * mapDepth);

      
        // le os tiles para preencher a matriz
        for (int z = 0; z < mapDepth; ++z)
        {
            for (int y = 0; y < mapHeight; ++y)
            {
                for (int x = 0; x < mapWidth; ++x)
                {
                    int index;
                    char c; // pular a vírgula
                    mapFile >> index >> c;

                    // DEBUG
                    if (mapFile.fail()){
                        std::cerr << "Erro ao ler o índice do tile em " << x << ", " << y << ", " << z << "\n";
                        return;
                    }

                    tileMatrix[(z * mapHeight + y) * mapWidth + x] = index;
                }
            }
        }
        mapFile.close();
    }
    else
    {
        std::cerr << "Erro ao abrir o arquivo do mapa: " << file << std::endl;
    }
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
                float posX = worldX - cam.x * parallaxFactor;
                float posY = worldY - cam.y * parallaxFactor;

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
            if (index != -1)
            {
                GameObject *block = new GameObject();
                block->box.x = associated.box.x + x * tileW;
                block->box.y = associated.box.y + y * tileH;
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
                int id = tiles[idx].ID - 1;  // se você quer 0‑based
                tileMatrix[(z * height + y) * width + x] = id;
            }
        }
        ++z;
    }

    std::cout << "Map loaded: " 
              << mapWidth << "x" << mapHeight 
              << "x" << mapDepth << "\n";
}
