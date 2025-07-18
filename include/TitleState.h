#ifndef TITLESTATE_H
#define TITLESTATE_H

#include "State.h"
#include "Music.h"
#include "Text.h"

class TitleState : public State
{
public:
    TitleState();
    ~TitleState() override;

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;

    void Start() override;

    // Sem funçõo por enquanto
    void Pause() override {}
    void Resume() override {}

    GameObject *newGameGO;
    GameObject *exitGO;

    Music backgroundMusic;
};

#endif // TITLESTATE_H
