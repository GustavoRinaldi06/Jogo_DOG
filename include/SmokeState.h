#include "State.h"
#include "Music.h"
#include "EndState.h"

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

private:
    Music backgroundMusic;
};
