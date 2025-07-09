#include "Dog.h"
#include "animator.h"
#include "Barrier.h"
#include "SpriteRenderer.h"
#include "Character.h"
#include "Collider.h"
#include <cmath>

Dog::Dog(GameObject &associated, int damage, bool targetsPlayer, const std::string &spritePath, const std::string &soundpath)
    : Component(associated), damage(damage), targetsPlayer(targetsPlayer), action(soundpath)
{
    auto renderer = new SpriteRenderer(associated, spritePath, 3, 2); // ajustar para animações
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);


    auto animator = new Animator(associated);
    animator->AddAnimation("action", Animation(0, 5, 0.2f));
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));

    animator->SetAnimation("action");

    RunTime.Restart();
    action.Play(1);
}

void Dog::Update(float dt)
{
    RunTime.Update(dt);

    if (RunTime.Get() >  0.8f){
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