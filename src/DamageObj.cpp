
#include "DamageObj.h"
#include "animator.h"
#include "SpriteRenderer.h"
#include "Character.h"
#include "Collider.h"
#include <cmath>

DamageObj::DamageObj(
    GameObject &associated, 
    int damage, 
    const std::string &spritePath, 
    const std::string &soundpath, 
    const std::string &dmgpath,
    int frameCountX,
    int frameCountY
)
    : Component(associated), damage(damage), originalDamage(damage), Spawn(soundpath), Damage(dmgpath)
{
    auto renderer = new SpriteRenderer(associated, spritePath, frameCountX, frameCountY); // ajustar para animações
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);

    auto animator = new Animator(associated);
    animator->AddAnimation("t-on", Animation(0, 0, 0.5f));
    animator->AddAnimation("active", Animation(0, 0, 0));
    animator->AddAnimation("t-off", Animation(0, 0, 0.5f));
    animator->AddAnimation("inactive", Animation(0, 0, 0));
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));

    animator->SetAnimation("t-on");

    Spawn.Play(1);
    active = true;
    activeTimer.Restart();
    AnimTime.Restart();
}

void DamageObj::Update(float dt)
{
    Animator *animator = (Animator *)associated.GetComponent("Animator");
    if (animator)
    {
        animator->Update(dt);
    }

    activeTimer.Update(dt);
    if(active && activeTimer.Get() > 5.0f){
        damage = 0;
        if (animator)
        {
            //animator->SetAnimation("t-off");
        }

        active = false;
        activeTimer.Restart();
        AnimTime.Restart();
    }
    else if(!active && activeTimer.Get() > 3.0f){
        damage = originalDamage;
        if(animator){
            //animator->SetAnimation("t-on");
        }

        Spawn.Play(1);
        active = true;
        activeTimer.Restart();
        AnimTime.Restart();
    }
    else if (active && AnimTime.Get() > 1.0f)
    {
        if (animator)
        {
            animator->SetAnimation("active");
        }
    }
    else if (!active && AnimTime.Get() > 1.0f)
    {
        if (animator)
        {
            animator->SetAnimation("inactive");
        }
    }
    else{
        return;
    }
}

void DamageObj::Render()
{}

bool DamageObj::Is(const std::string &type)
{
    return type == "DamageObj";
}

int DamageObj::GetDamage() const
{
    return damage;
}

void DamageObj::NotifyCollision(GameObject &other)
{
    // FAz barulho ao passar e aumenta o dano cada vez que colide
    Damage.Play(1);
    damage++;
    originalDamage++;
}

