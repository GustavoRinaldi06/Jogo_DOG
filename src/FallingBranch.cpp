#include "FallingBranch.h"
#include "Game.h"

FallingBranch::FallingBranch(GameObject &associated,
                             float fallSpeed,
                             float rotationSpeed)
    : Component(associated),
      fallSpeed(fallSpeed),
      rotationSpeed(rotationSpeed),
      currentRotation(0.0f),
      falling(false)
{

    auto renderer = new SpriteRenderer(associated, "recursos/img/sprites/FallingBranch.png", 8, 1);
    associated.AddComponent(renderer);
    spriteRenderer = renderer;

    animator = new Animator(associated);
    animator->AddAnimation("idle", Animation(3, 5, 0.5f));
    associated.AddComponent(animator);

    // Collider para trigger de início de queda
    associated.AddComponent(new Collider(associated));
    animator->SetAnimation("idle");
}

void FallingBranch::Update(float dt)
{
    animator->Update(dt);

    if (!falling)
    {
        falling = true;
    }

    if (falling)
    {
        // Cai para baixo
        associated.box.y += fallSpeed * dt;

        // Incrementa rotação
        currentRotation += rotationSpeed * dt;
        if (currentRotation >= 360.0f)
            currentRotation -= 360.0f;

        // Aplica transformação
        spriteRenderer->SetRotation(currentRotation);
    }
}

void FallingBranch::Render()
{
    // A rotação já é aplicada pelo SpriteRenderer
}

bool FallingBranch::Is(const std::string &type)
{
    return type == "FallingBranch";
}
