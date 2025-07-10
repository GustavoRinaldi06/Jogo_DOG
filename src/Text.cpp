#include "Text.h"
#include "Game.h"
#include "Resources.h"
#include "Camera.h"
#include <sstream>

Text::Text(GameObject &associated, const std::string &fontFile, int fontSize, TextStyle style, const std::string &text, SDL_Color color)
    : Component(associated), arqFonte(fontFile), tamFonte(fontSize), estilo(style), cor(color), texto(text), texture(nullptr)
{
    // Cria a textura inicial
    RemakeTexture();
}

Text::~Text()
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void Text::Update(float dt)
{}

void Text::Render()
{
    if (!texture)
        return;

    // Renderizar de acordo com a camera, por mais que não seja usado nesse trabalho
    //Vec2 camPos = Camera::GetInstance().GetPosition();
    Vec2 renderPos(associated.box.x, associated.box.y);
    if (!cameraFollower)
    {
        renderPos = renderPos - Camera::GetInstance().GetPosition();
    }

    SDL_Rect dstRect;
    dstRect.x = static_cast<int>(renderPos.x);
    dstRect.y = static_cast<int>(renderPos.y);
    dstRect.w = clipRect.w;
    dstRect.h = clipRect.h;

    SDL_RenderCopyEx(Game::GetInstance().GetRenderer(), texture, &clipRect, &dstRect, associated. angleDeg, nullptr, SDL_FLIP_NONE);
}

// Set de texto: Atualizar valor e mandar refazer
void Text::SetText(const std::string &newText)
{
    texto = newText;
    RemakeTexture();
}

void Text::SetColor(SDL_Color newColor)
{
    cor = newColor;
    RemakeTexture();
}

void Text::SetStyle(TextStyle newStyle)
{
    estilo = newStyle;
    RemakeTexture();
}

void Text::SetFontSize(int newFontSize)
{
    tamFonte = newFontSize;
    RemakeTexture();
}

void Text::RemakeTexture()
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    TTF_Font *font = Resources::GetFont(arqFonte, tamFonte);
    if (!font)
    {
        return;
    }

    std::vector<SDL_Surface *> lineSurfaces;
    int totalWidth = 0;
    int totalHeight = 0;

    std::istringstream stream(texto);
    std::string line;
    while (std::getline(stream, line))
    {
        SDL_Surface *lineSurface = nullptr;

        switch (estilo)
        {
        case SOLID:
            lineSurface = TTF_RenderText_Solid(font, line.c_str(), cor);
            break;
        case SHADED:
        {
            SDL_Color bg = {0, 0, 0, 255};
            lineSurface = TTF_RenderText_Shaded(font, line.c_str(), cor, bg);
            break;
        }
        case BLENDED:
            lineSurface = TTF_RenderText_Blended(font, line.c_str(), cor);
            break;
        }

        if (lineSurface)
        {
            totalWidth = std::max(totalWidth, lineSurface->w);
            totalHeight += lineSurface->h;
            lineSurfaces.push_back(lineSurface);
        }
    }

    if (lineSurfaces.empty())
        return;

    SDL_Surface *finalSurface = SDL_CreateRGBSurface(0, totalWidth, totalHeight, 32,
                                                     0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    int yOffset = 0;
    for (auto *surface : lineSurfaces)
    {
        SDL_Rect dstRect = {0, yOffset, surface->w, surface->h};
        SDL_BlitSurface(surface, nullptr, finalSurface, &dstRect);
        yOffset += surface->h;
        SDL_FreeSurface(surface);
    }

    SDL_Renderer *renderer = Game::GetInstance().GetRenderer();
    texture = SDL_CreateTextureFromSurface(renderer, finalSurface);

    clipRect.x = 0;
    clipRect.y = 0;
    clipRect.w = finalSurface->w;
    clipRect.h = finalSurface->h;

    associated.box.w = finalSurface->w;
    associated.box.h = finalSurface->h;

    SDL_FreeSurface(finalSurface);
}

bool Text::Is(const std::string &type)
{
    return (type == "Text");
}

void Text::SetCameraFollower(bool follow)
{
    cameraFollower = follow;
}
