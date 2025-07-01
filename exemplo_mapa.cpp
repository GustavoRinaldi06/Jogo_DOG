void StageState::LoadTMX(std::string file)
{
    tmx::Map map;
    if (map.load(file))
    {
        GameObject* go = new GameObject();
        TileSet* tileSet = new TileSet(16, 16, "res/map/TilesetPlaceholder.png");
        TileMap* tileMap = new TileMap(*go, tileSet, map);
        go->AddComponent(tileMap);
        AddObject(go);
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
                    std::cout << "Object class" << object.getClass() << std::endl;
                    const auto& properties = object.getProperties();
                    std::cout << "Object has " << properties.size() << " properties" << std::endl;
                    if(object.getClass() == "Chainsaw"){
                      
                    }

                    if(object.getClass() )
                    for (const auto& prop : properties)
                    {
                        std::cout << "Found property: " << prop.getName() << std::endl;
                        std::cout << "Type: " << int(prop.getType()) << std::endl;
                    }
                    GameObject* chao = new GameObject();
                    chao->m_box = { object.getAABB().left, object.getAABB().top, object.getAABB().width, object.getAABB().height};
                    Collider* col = new Collider(*chao, true);
                    AddObject(chao);
                }
            }
        }
    }
    else
    {
        std::cout << "Failed loading map" << std::endl;
    }
}

TileMap::TileMap(GameObject& associated, TileSet* tileSet, const tmx::Map& map) : Component(associated)
{
    m_tileSet = tileSet;
    Load(map);
}

void TileMap::Load(const tmx::Map& map)
{
    const auto& layers = map.getLayers();
    std::cout << "Map has " << layers.size() << " layers" << std::endl;
    for (const auto& layer : layers)
    {
        m_mapWidth = layer->getSize().x;
        m_mapHeight = layer->getSize().y;
        m_mapDepth = 1;
        if (layer->getType() == tmx::Layer::Type::Tile)
        {
            if (layer->getName().find("") == std::string::npos) {
                continue;
            }
            const auto& tiles = layer->getLayerAs<tmx::TileLayer>().getTiles();
            if (tiles.empty())
            {
                const auto& chunks = layer->getLayerAs<tmx::TileLayer>().getChunks();
                if (chunks.empty())
                {
                    std::cout << "Layer has missing tile data\n";
                }
                else
                {
                    std::cout << "Layer has " << chunks.size() << " tile chunks.\n";
                }
            }
            else
            {
                std::cout << "Layer has " << tiles.size() << " tiles.\n";
                for (auto& tile : tiles) {
                    m_tileMatrix.push_back(tile.ID - 1);
                }
            }
        }
    }
}