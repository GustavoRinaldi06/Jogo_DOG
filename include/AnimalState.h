#include "State.h"
#include "Music.h"
#include "EndState.h"

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

private:
    Music backgroundMusic;
};
