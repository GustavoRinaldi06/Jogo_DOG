
#include "TreeBoss.h"
#include "animator.h"
#include "SpriteRenderer.h"
#include "Character.h"
#include "Collider.h"
#include <cmath>

TreeBoss::TreeBoss(
    GameObject &associated
)
    : Component(associated)
{
    auto renderer = new SpriteRenderer(associated, "recursos/img/sprites/TreeBoss.png", 7, 1); // ajustar para animações
    renderer->SetCameraFollower(false);
    renderer->SetScale(0.7f, 0.7f); // ajustar escala
    associated.AddComponent(renderer);

    animator = new Animator(associated);
    animator->AddAnimation("t-on", Animation(0, 6, 3.0f));
    associated.AddComponent(animator);

    active = false;
    activeTimer.Restart();

    animator->SetAnimation("t-on");
}

void TreeBoss::Update(float dt)
{
    Animator *animator = (Animator *)associated.GetComponent("Animator");
    if (!animator) return; 

    animator->Update(dt);
    activeTimer.Update(dt);
}

void TreeBoss::Render()
{}

bool TreeBoss::Is(const std::string &type)
{
    return type == "TreeBoss";
}

int TreeBoss::GetDamage() const
{
    return damage;
}

void TreeBoss::NotifyCollision(GameObject &other)
{
    // sem dano
}

std::string TreeBoss::GetObjectName() const
{
    return objectName;
}