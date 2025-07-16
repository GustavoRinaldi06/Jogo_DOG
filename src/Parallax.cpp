#include "Parallax.h"
#include "Camera.h"

Parallax::Parallax(GameObject &associated, float factor)
    : Component(associated), horizontalParallaxFactor(factor), verticalParallaxFactor(factor) {}

    Parallax::Parallax(GameObject &associated, float horizontalFactor, float verticalFactor)
    : Component(associated), horizontalParallaxFactor(horizontalFactor), verticalParallaxFactor(verticalFactor) {}

void Parallax::Update(float dt)
{
    Vec2 camPos = Camera::GetInstance().GetPosition();
    associated.box.x = camPos.x * horizontalParallaxFactor;
    associated.box.y = camPos.y * verticalParallaxFactor;
}

bool Parallax::Is(const std::string &type)
{
    return type == "Parallax";
}
