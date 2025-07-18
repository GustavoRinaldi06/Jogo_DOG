#include "Gate.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Animation.h"
#include "Collider.h"
#include "TreeState.h"

Gate::Gate(GameObject& associated) 
    : Component(associated), barkCount(0) 
{
    SpriteRenderer* renderer = new SpriteRenderer(associated, "recursos/img/sprites/Gate.png", 8, 1);

    auto animator = new Animator(associated);
    animator->AddAnimation("closed", Animation(0, 0, 0.5));
    animator->AddAnimation("opening", Animation(1, 7, 0.9));
    animator->SetAnimation("closed");

    associated.AddComponent(renderer);
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));
}

void Gate::Update(float dt) {}
void Gate::Render() {}

bool Gate::Is(const std::string& type) {
    return type == "Gate";
}

void Gate::NotifyBark() {
    barkCount++;
    if (barkCount >= 3) {
        Animator* animator = static_cast<Animator*>(associated.GetComponent("Animator"));
        if (animator) {
            animator->SetAnimation("opening");
        }
    }
}

bool Gate::IsOpen() {
    return barkCount >= 3;
}