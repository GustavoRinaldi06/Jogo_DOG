#include "Parallax.h"
#include "Camera.h"

Parallax::Parallax(GameObject &associated, float factor)
    : Component(associated), parallaxFactor(factor) {}

void Parallax::Update(float dt)
{
    Vec2 camPos = Camera::GetInstance().GetPosition();
    associated.box.x = camPos.x * parallaxFactor;
    associated.box.y = camPos.y * parallaxFactor;
}

bool Parallax::Is(const std::string &type)
{
    return type == "Parallax";
}
