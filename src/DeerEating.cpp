#include "DeerEating.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Animation.h"
#include "Collider.h"
#include "TreeState.h"
#include "Character.h"
#include <iostream>

DeerEating::DeerEating(GameObject &associated)
    : Component(associated), gravity(800.0f), isOnGround(false), applyGravity(true), isFalling(false)
{
    SpriteRenderer *renderer = new SpriteRenderer(associated, "recursos/img/sprites/DeerEating.png", 5, 1);
    renderer->SetScale(0.2f, 0.2f);

    auto animator = new Animator(associated);
    animator->AddAnimation("eating", Animation(0, 1, 2.0f));
    animator->AddAnimation("looking", Animation(2, 2, 0.5f));
    animator->AddAnimation("discover-player", Animation(2, 4, 0.1));
    animator->SetAnimation("eating");

    associated.AddComponent(renderer);
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));
}

void DeerEating::Update(float dt)
{
    std::cout << "Current state: " << currentState << "\n";
    dangerTimer.Update(dt);
    Animator *animator = static_cast<Animator *>(associated.GetComponent("Animator"));

    if (discoverTimer.Get() > 0)
    {
        discoverTimer.Update(dt);
        if (currentState != "discover-player")
        {
            animator->SetAnimation("discover-player");
            currentState = "discover-player";
        }
        if (discoverTimer.Get() <= 0)
        {
            // Transição para estado final
        }
        return;
    }

    // 2. Comportamento normal
    if (Character::player && Character::player->IsInDangerArea())
    {
        if (currentState != "looking")
        {
            animator->SetAnimation("looking");
            currentState = "looking";
            discoverTimer.Set(3.0f); // Inicia contagem regressiva
        }
    }
    else if (currentState != "eating")
    {
        animator->SetAnimation("eating");
        currentState = "eating";
    }

    // Física de queda
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
void DeerEating::Render() {}

bool DeerEating::Is(const std::string &type)
{
    return type == "DeerEating";
}

void DeerEating::NotifyCharacterInDangerArea(GameObject &other)
{
}

void DeerEating::NotifyCollision(GameObject &other)
{
    Collider *otherCollider = (Collider *)other.GetComponent("Collider");

    if (other.GetComponent("Character"))
    {
        return;
    }

    if (otherCollider && otherCollider->tag == "danger_area")
    {
        return; // ignorar colisão com DangerArea
    }

    if (otherCollider && otherCollider->tag == "ground")
    {
        isOnGround = true;
        speed.y = 0; // Para o movimento vertical

        // Ajusta a posição para ficar sobre o chão
        Rect &box = associated.box;
        box.y = other.box.y - box.h;
    }
}
