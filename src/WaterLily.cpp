#include "WaterLily.h"
#include "SpriteRenderer.h"
#include "Collider.h"
#include "Animator.h"

WaterLily::WaterLily(GameObject &associated)
    : Component(associated)
{
    auto renderer = new SpriteRenderer(associated, "recursos/map/Animal/tiles.png", 5, 4);
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);

    auto animator = new Animator(associated);
    animator->AddAnimation("t-on", Animation(17, 17, 0.5f));
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));
    animator->SetAnimation("t-on");
}

void WaterLily::Update(float dt)
{
    //speed.y += gravity * dt;
    //associated.box.y += speed.y * dt;
}

void WaterLily::Render()
{
    // Rendering handled by SpriteRenderer
}

bool WaterLily::Is(const std::string &type)
{
    return type == "WaterLily";
}

void WaterLily::NotifyCollision(GameObject &other)
{
    // No special collision logic for now
}