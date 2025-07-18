#pragma once

#include "State.h"
#include "Music.h"
#include "EndState.h"
#include "../include/tmxlite/Map.hpp"

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
};
