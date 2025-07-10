#ifndef ENDSTATE_H
#define ENDSTATE_H

#include "State.h"
#include "Music.h"
#include "GameData.h"
#include "InputManager.h"
#include "Game.h"
#include "TitleState.h"
#include "Timer.h"
#include <memory>

class EndState : public State
{
public:
    EndState();
    ~EndState() override;

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;

    void Start() override;
    void Pause() override;
    void Resume() override;

private:
    Music backgroundMusic;
    Text *pressSpace = nullptr;

    Timer exitTime;
};

#endif // ENDSTATE_H
