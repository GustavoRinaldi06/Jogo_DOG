#include "VideoState.h"
#include "Game.h"
#include "InputManager.h"

// comando que converter video 
//C:\ffmpeg\bin\ffmpeg.exe -i "C:\Users\lukas\Downloads\intro-dog.mp4" -c:v rawvideo -pix_fmt uyvy422 -f rawvideo -an "output.uyvy"
VideoState::VideoState(const std::string& filePath, int width, int height, float fps, std::function<State *()> nextStateFactory)
    : videoPath(filePath), width(width), height(height), frameRate(fps), createNext(std::move(nextStateFactory))
{
    videoTimer = 0.0f;
    frameDelay = 1.0f / frameRate;
    videoFinished = false;
    texture = nullptr;
    frameBuffer = nullptr;
    frameSize = static_cast<size_t>(width) * height * 2;
}

VideoState::~VideoState() {
    CloseVideo();
}

void VideoState::CloseVideo() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    
    if (frameBuffer) {
        delete[] frameBuffer;
        frameBuffer = nullptr;
    }
    
    if (videoFile.is_open()) {
        videoFile.close();
    }
}

void VideoState::LoadAssets() {
    // Tentar abrir o arquivo
    videoFile.open(videoPath, std::ios::binary);
    if (!videoFile.is_open()) {
        SDL_Log("Failed to open video file: %s", videoPath.c_str());
        return;
    }

    // Verificar tamanho do arquivo
    videoFile.seekg(0, std::ios::end);
    const size_t fileSize = static_cast<size_t>(videoFile.tellg());
    videoFile.seekg(0, std::ios::beg);

    if (fileSize < frameSize) {
        SDL_Log("Video file too small: %s", videoPath.c_str());
        return;
    }

    // Alocar buffer
    try {
        frameBuffer = new uint8_t[frameSize];
    } catch (const std::bad_alloc&) {
        SDL_Log("Memory allocation failed for video buffer");
        return;
    }

    // Criar textura
    texture = SDL_CreateTexture(
        Game::GetInstance().GetRenderer(),
        SDL_PIXELFORMAT_UYVY,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    if (!texture) {
        SDL_Log("Texture creation failed: %s", SDL_GetError());
        return;
    }

    valid = true;
}

void VideoState::LoadNextFrame() {
    if (!valid || videoFinished) return;

    videoFile.read(reinterpret_cast<char*>(frameBuffer), frameSize);
    
    if (videoFile.gcount() < static_cast<std::streamsize>(frameSize)) {
        videoFinished = true;
        return;
    }
    
    SDL_UpdateTexture(texture, nullptr, frameBuffer, width * 2);
}

void VideoState::Update(float dt) {
    InputManager& input = InputManager::GetInstance();
    
    // Verificar se alguma tecla foi pressionada (compatível com seu InputManager)
    bool keyPressed = false;
    for (int i = 0; i < 512; i++) { // Verifica todas as teclas possíveis
        if (input.KeyPress(i)) {
            keyPressed = true;
            break;
        }
    }
    
    // Pular vídeo com qualquer tecla, clique ou sair
    if (keyPressed || input.MousePress(0) || input.QuitRequested()) {
        CallNextState();
        return;
    }

    if (!valid || videoFinished) {
        CallNextState();
        return;
    }

    videoTimer += dt;
    while (videoTimer >= frameDelay) {
        videoTimer -= frameDelay;
        LoadNextFrame();
    }
}

void VideoState::Render() {
    SDL_Renderer* renderer = Game::GetInstance().GetRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    if (texture && valid && !videoFinished) {
        int winW, winH;
        SDL_GetWindowSize(Game::GetInstance().GetWindow(), &winW, &winH);
        
        const float videoAspect = static_cast<float>(width) / height;
        const float screenAspect = static_cast<float>(winW) / winH;
        
        SDL_Rect destRect;
        if (videoAspect > screenAspect) {
            destRect.w = winW;
            destRect.h = static_cast<int>(winW / videoAspect);
            destRect.x = 0;
            destRect.y = (winH - destRect.h) / 2;
        } else {
            destRect.h = winH;
            destRect.w = static_cast<int>(winH * videoAspect);
            destRect.x = (winW - destRect.w) / 2;
            destRect.y = 0;
        }
        
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    }
    
    SDL_RenderPresent(renderer);
}

void VideoState::Start() {
    LoadAssets();
    started = true;
    
    // Carregar primeiro frame imediatamente
    if (valid) LoadNextFrame();
}

void VideoState::CallNextState() {
    State *next = createNext();     // Cria o próximo estado
    Game::GetInstance().Push(next); // Empilha o novo estado
    popRequested = true;  
}