#pragma once

#include "State.h"
#include "Music.h"
#include "EndState.h"
#include "GameObject.h"
#include "../include/tmxlite/Map.hpp"
#include "FallingBranch.h"
#include "Timer.h"
#include "Gate.h"
#include "Rect_Vec2.h"

struct BranchSpawner
{
    Vec2 position;
    Timer timer;
    float nextSpawnTime;
};

class SpriteRenderer;

class TreeState : public State
{
public:
    TreeState();
    ~TreeState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;

    void Start() override;
    void Pause() override;
    void Resume() override;

    void LoadLayers();
    void LoadFromTMX(std::string file);
    void LoadForeground();

    Text *hpText = nullptr;
    Text *dogText = nullptr;

    GameObject *FindGateObject();

private:
    Music backgroundMusic;
    void CreateGameObject(const tmx::Object &object);
    Timer branchTimer;
    std::vector<Vec2> branchPositions;

    std::vector<BranchSpawner> branchSpawners;
    float RandomFloat(float min, float max);
};
