#include "Gate.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Animation.h"
#include "Collider.h"


Gate::Gate(GameObject& associated) 
    : Component(associated), barkCount(0) 
{
    SpriteRenderer* renderer = new SpriteRenderer(associated, "recursos/img/sprites/Gate.png", 8, 1);

    auto animator = new Animator(associated);
    animator->AddAnimation("closed", Animation(0, 0, 0.5));
    animator->AddAnimation("opening", Animation(1, 7, 0.5));
    animator->SetAnimation("closed");

    associated.AddComponent(renderer);
    associated.AddComponent(animator);
}

void Gate::Update(float dt) {}
void Gate::Render() {}

bool Gate::Is(const std::string& type) {
    return type == "Gate";
}

void Gate::NotifyBark() {
    barkCount++;
    if (barkCount >= 3) {
        associated.RequestDelete(); // Destrói o portão após 3 latidos
    }
}