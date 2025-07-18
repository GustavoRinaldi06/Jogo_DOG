#pragma once

#include "State.h"
#include "Music.h"
#include "EndState.h"
#include "../include/tmxlite/Map.hpp"
#include <string>

class SpriteRenderer;

class AnimalState : public State
{
public:
    AnimalState();
    ~AnimalState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;

    void Start() override;
    void Pause() override;
    void Resume() override;

    Text *hpText = nullptr;
    Text *dogText = nullptr;

    void LoadLayers();
    void CreateGameObject(const tmx::Object &object);
    void LoadFromTMX(const std::string file);
    void LoadForeground();
    GameObject *CreateWaterLilyObject(const tmx::Object &object);

private:
    Music backgroundMusic;
    Timer dangerTimer;
    bool deerSpawned = false;
    bool dangerActive = false;

    void NextState();
};
