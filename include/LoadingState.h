#pragma once

#include "State.h"
#include "Text.h"
#include <functional>
#include <vector>
#include <string>

class LoadingState : public State
{
public:
    LoadingState(std::function<State *()> nextStateFactory, std::vector<std::string> assets);
    ~LoadingState();

    void LoadAssets() override;
    void Start() override;
    void Update(float dt) override;
    void Render() override;
    void Pause() override;
    void Resume() override;

private:
    size_t currentIndex;
    std::function<State *()> createNext;
    std::vector<std::string> assetList;
    Text *progressText;
};
