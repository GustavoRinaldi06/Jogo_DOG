#include "State.h"
#include "Music.h"
#include "EndState.h"
#include "GameObject.h"
#include "../include/tmxlite/Map.hpp"
#include "FallingBranch.h"
#include "Timer.h"

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

    Text *hpText = nullptr;
    Text *dogText = nullptr;
private:
    Music backgroundMusic;
    void CreateGameObject(const tmx::Object& object);
    GameObject* CreateHandObject(const tmx::Object& object);
    GameObject* CreateThornObject(const tmx::Object& object);
    GameObject* CreateChainSawObject(const tmx::Object& object);
    GameObject* CreateFallingBranchObject(const tmx::Object& object);
    Timer branchTimer;
};
