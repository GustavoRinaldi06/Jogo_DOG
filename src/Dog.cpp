#include "Dog.h"
#include "animator.h"
#include "Barrier.h"
#include "SpriteRenderer.h"
#include "Character.h"
#include "Collider.h"
#include "State.h"
#include "Gate.h"
#include <cmath>

Dog::Dog(GameObject &associated, int damage, bool targetsPlayer, const std::string &spritePath, const std::string &soundpath)
    : Component(associated), damage(damage), targetsPlayer(targetsPlayer), action(soundpath), collisionChecked(false)
{
    auto renderer = new SpriteRenderer(associated, spritePath, 8, 1); // ajustar para animações
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);
    renderer->SetScale(0.6f, 0.6f);

    auto animator = new Animator(associated);
    animator->AddAnimation("action", Animation(0, 5, 0.3f));
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));

    animator->SetAnimation("action");

    RunTime.Restart();
    action.Play(1);
}

void Dog::Update(float dt)
{
    RunTime.Update(dt);

    SpriteRenderer *renderer = static_cast<SpriteRenderer *>(associated.GetComponent("SpriteRenderer"));
    if (renderer && Character::player)
    {
        float dogX = associated.box.GetCenter().x;
        float playerX = Character::player->GetPosition().x;

        if (playerX < dogX)
        {
            renderer->SetFlip(SDL_FLIP_NONE); // vira para a direita
        }
        else
        {
            renderer->SetFlip(SDL_FLIP_HORIZONTAL); // vira para a esquerda
        }
    }

    // Verificar colisões apenas uma vez no início
    if (!collisionChecked) {
        CheckDestructibleCollisions();
        collisionChecked = true;
    }

    if (RunTime.Get() >  1.0f){
        associated.RequestDelete();

        return;
    }
    
    Animator *animator = static_cast<Animator *>(associated.GetComponent("Animator"));
    if (animator)
    {
        animator->Update(dt);
    }
}
void Dog::Render(){}

bool Dog::Is(const std::string &type)
{
    return type == "Dog";
}

int Dog::GetDamage() const
{
    return damage;
}

void Dog::NotifyCollision(GameObject &other)
{
    if (other.GetComponent("Barrier")) // caso colidir com barrier
    {
        Collider *col = dynamic_cast<Collider *>(associated.GetComponent("Collider"));
        if (col)
        {
            associated.RemoveComponent(col);
            delete col;
        }
    }
}

void Dog::CheckDestructibleCollisions()
{
    Vec2 dogCenter = associated.box.GetCenter();
    float radius = 300.0f; // Raio de efeito do uivo
    
    for (auto& obj : Game::GetInstance().GetCurrentState().GetObjects()) {
        if (obj->GetComponent("Chainsaw") || obj->GetComponent("DamageObj")) {
            Vec2 objCenter = obj->box.GetCenter();
            if (dogCenter.Distance(objCenter) <= radius) {
                obj->RequestDelete(); // Destrói objetos próximos
            }
        }
        else if (auto gateComp = obj->GetComponent("Gate")) {
            Gate* gate = dynamic_cast<Gate*>(gateComp);
            if (gate) {
                gate->NotifyBark();
            }
        }
    }
}