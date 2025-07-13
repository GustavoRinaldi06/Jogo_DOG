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
    GameObject* createHandObject(const tmx::Object& object);
    GameObject* createThornObject(const tmx::Object& object);
    GameObject* createChainSawObject(const tmx::Object& object);
    GameObject* createFallingBranchObject(const tmx::Object& object);
    Timer branchTimer;
};
