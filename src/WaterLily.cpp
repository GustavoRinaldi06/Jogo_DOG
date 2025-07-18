#include "WaterLily.h"
#include "SpriteRenderer.h"
#include "Collider.h"
#include "Animator.h"

WaterLily::WaterLily(GameObject &associated)
    : Component(associated), gravity(800.0f),
      isOnGround(false), applyGravity(true), isFalling(false)
{
    auto renderer = new SpriteRenderer(associated, "recursos/img/sprites/WaterLily.png", 1, 1);
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);

    auto animator = new Animator(associated);
    animator->AddAnimation("t-on", Animation(0, 0, 0.5f));
    associated.AddComponent(animator);

    auto collider = new Collider(associated);
    collider->tag = "waterlily";
    associated.AddComponent(collider);
    animator->SetAnimation("t-on");
    speed = Vec2(0, 0);
}

void WaterLily::Update(float dt)
{
    if (applyGravity && !isOnGround)
    {
        speed.y += gravity * dt;
        associated.box.y += speed.y * dt;
        isFalling = true;
    }
    else
    {
        isFalling = false;
    }
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
    Collider *otherCollider = (Collider *)other.GetComponent("Collider");
    if (otherCollider && otherCollider->tag == "ground")
    {
        isOnGround = true;
        speed.y = 0; // Para o movimento vertical

        // Ajusta a posição para ficar sobre o chão
        Rect &box = associated.box;
        box.y = other.box.y - box.h;
    }
}