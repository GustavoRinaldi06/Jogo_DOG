
#include "DamageObj.h"
#include "animator.h"
#include "SpriteRenderer.h"
#include "Character.h"
#include "Collider.h"
#include <cmath>

DamageObj::DamageObj(GameObject &associated, int damage, const std::string &spritePath, const std::string &soundpath, const std::string &dmgpath, std::string objectName)
    : Component(associated), damage(damage), originalDamage(damage), Spawn(soundpath), Damage(dmgpath), objectName(objectName), 
      gravity(800.0f),
      isOnGround(false),
      applyGravity(true),
      isFalling(false)
{
    auto renderer = new SpriteRenderer(associated, spritePath, 1, 1); // ajustar para animações
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
    speed = Vec2(0, 0);
}

DamageObj::DamageObj(
    GameObject &associated, 
    int damage, 
    const std::string &spritePath, 
    const std::string &soundpath, 
    const std::string &dmgpath,
    int frameCountX,
    int frameCountY,
    std::string objectName
)
    : Component(associated), damage(damage), originalDamage(damage), Spawn(soundpath), Damage(dmgpath), objectName(objectName),
      gravity(800.0f),
      isOnGround(false),
      applyGravity(true),
      isFalling(false)
{
    auto renderer = new SpriteRenderer(associated, spritePath, frameCountX, frameCountY); // ajustar para animações
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);

    animator = new Animator(associated);
    animator->AddAnimation("t-on", Animation(1, 3, 0.5f));
    animator->AddAnimation("active", Animation(4, 4, 0));
    animator->AddAnimation("t-off", Animation(3, 1, 0.5f));
    animator->AddAnimation("inactive", Animation(0, 0, 0));
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));

    currentState = "inactive";
    animator->SetAnimation("inactive");
    active = false;
    activeTimer.Restart();
}

void DamageObj::Update(float dt)
{
    Animator *animator = (Animator *)associated.GetComponent("Animator");
    if (!animator) return; 

    animator->Update(dt);
    activeTimer.Update(dt);

    // Controle simplificado de estados e temporização
    if (currentState == "inactive" && activeTimer.Get() > 3.0f)
    {
        animator->SetAnimation("t-on");
        currentState = "t-on";
        activeTimer.Restart();
        Spawn.Play(1);
    }
    else if (currentState == "t-on" && activeTimer.Get() > 0.5f)
    {
        animator->SetAnimation("active");
        currentState = "active";
        active = true;
        damage = originalDamage;
        activeTimer.Restart();
    }
    else if (currentState == "active" && activeTimer.Get() > 5.0f)
    {
        animator->SetAnimation("t-off");
        currentState = "t-off";
        active = false;
        activeTimer.Restart();
    }
    else if (currentState == "t-off" && activeTimer.Get() > 0.5f)
    {
        animator->SetAnimation("inactive");
        currentState = "inactive";
        damage = 0;
        activeTimer.Restart();
    }

    if (applyGravity && !isOnGround) {
        speed.y += gravity * dt;
        associated.box.y += speed.y * dt;
        isFalling = true;
    }
    else {
        isFalling = false;
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
    Collider* otherCollider = (Collider*)other.GetComponent("Collider");
    if (otherCollider && otherCollider->tag == "ground") {
        isOnGround = true;
        speed.y = 0;
    
        Rect& box = associated.box;
        box.y = other.box.y - box.h;
    }
    

    if(!active) return;
    Damage.Play(1);
    originalDamage++;
}

std::string DamageObj::GetObjectName() const
{
    return objectName;
}