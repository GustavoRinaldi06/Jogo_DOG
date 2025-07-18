#pragma once

#include "State.h"
#include "Music.h"
#include "EndState.h"
#include "FallingBranch.h"
#include "Timer.h"
#include "../include/tmxlite/Map.hpp"

struct BranchSpawner
{
    Vec2 position;
    Timer timer;
    float nextSpawnTime;
};
class SpriteRenderer;

class SmokeState : public State
{
public:
    SmokeState();
    ~SmokeState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;

    void Start() override;
    void Pause() override;
    void Resume() override;

    Text *hpText = nullptr;
    Text *dogText = nullptr;

    void LoadLayers();
    void LoadForeground();
    void LoadFromTMX(std::string file);
    void CreateGameObject(const tmx::Object &object);

private:
    Music backgroundMusic;

    Timer branchTimer;
    std::vector<BranchSpawner> branchSpawners;
    float RandomFloat(float min, float max);
};
