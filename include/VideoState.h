#ifndef VIDEOSTATE_H
#define VIDEOSTATE_H

#include "State.h"
#define INCLUDE_SDL
#include "SDL_include.h"
#include <fstream>
#include <string>
#include <functional>

class VideoState : public State {
public:
    VideoState(const std::string& filePath, int width, int height, float fps, std::function<State *()> nextStateFactory);
    ~VideoState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override {}
    void Resume() override {}

    bool IsValid() const { return valid; }

private:
    std::string videoPath;
    int width;
    int height;
    float frameRate;
    float frameDelay;
    float videoTimer;
    bool videoFinished;
    bool valid = false;
    std::ifstream videoFile;
    SDL_Texture* texture;
    uint8_t* frameBuffer;
    size_t frameSize;

    std::function<State *()> createNext;

    void LoadNextFrame();
    void CloseVideo();
    void CallNextState();
};

#endif // VIDEOSTATE_H