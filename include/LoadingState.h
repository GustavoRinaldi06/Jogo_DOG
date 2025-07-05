#include <functional>
#include <vector>
#include <string>
#include "State.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "Game.h"
#include "SDL_include.h"

class LoadingState : public State {
public:
    explicit LoadingState(std::function<State*()> nextStateFactory);
    ~LoadingState() override;

    void LoadAssets() override;
    void Start() override;
    void Update(float dt) override;
    void Render() override;
    void Pause() override;
    void Resume() override;

private:
    std::vector<std::string> assetList;
    size_t currentIndex;
    std::function<State*()> createNext;
    Text* progressText;
};