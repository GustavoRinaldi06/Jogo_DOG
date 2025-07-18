#pragma once

#include "State.h"
#define INCLUDE_SDL
#include "SDL_include.h"
#include <string>
#include <unordered_map>
#include <functional>
#include "Timer.h"
#include "Rect_Vec2.h"

class CutsceneIntroState : public State {
public:
    CutsceneIntroState(std::function<State *()> nextStateFactory);
    ~CutsceneIntroState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override {}
    void Resume() override {}

private:
    std::function<State *()> createNext;
    void CallNextState();
    std::unordered_map<std::string, GameObject*> images;
    Timer timer;

    void ShowImage(const std::string &imageName);
    void HideImage(const std::string &imageName);
    void ScaleImageFixed(const std::string &imageName, Vec2 amount);
};