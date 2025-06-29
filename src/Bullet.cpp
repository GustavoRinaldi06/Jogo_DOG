
#include "Bullet.h"
#include "animator.h"
#include "SpriteRenderer.h"
#include "Character.h"
#include "Collider.h"
#include <cmath>

Bullet::Bullet(GameObject &associated, float angle, float speed, int damage, float maxDistance, bool targetsPlayer, const std::string &spritePath, const std::string &explodepath)
    : Component(associated), distanceLeft(maxDistance), damage(damage), targetsPlayer(targetsPlayer), Explode(explodepath)
{
    velocity = Vec2(std::cos(angle), std::sin(angle)) * speed;

    auto renderer = new SpriteRenderer(associated, spritePath, 1, 1); // ajustar para animações
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);

    associated.angleDeg = angle * 180 / M_PI + 90; // Adicionar 90 para a bala ficar horizontal em relação a arma

    auto animator = new Animator(associated);
    animator->AddAnimation("spawn", Animation(0, 0, 0.5f));
    animator->AddAnimation("running", Animation(0, 0, 0.2f));
    animator->AddAnimation("explode", Animation(0, 0, 0));
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));

    SpawnTime.Restart();
    animator->SetAnimation("spawn");
}

void Bullet::Update(float dt)
{
    if (exploded)
    {
        explodeTimer.Update(dt);
        if (explodeTimer.Get() > 0.5f) // tempo de explosão
        {
            associated.RequestDelete();
        }
        return;
    }

    SpawnTime.Update(dt);

    Animator *animator = static_cast<Animator *>(associated.GetComponent("Animator"));
    if (animator){
        if (SpawnTime.Get() > 1.0f){
            animator->SetAnimation("running");
        }
    }

    Vec2 displacement = velocity * dt;
    associated.box.x += displacement.x;
    associated.box.y += displacement.y;
    distanceLeft -= displacement.Magnitude();

    if (distanceLeft <= 0)
    {
        TriggerExplosion();
    }
}

void Bullet::Render()
{
}

bool Bullet::Is(const std::string &type)
{
    return type == "Bullet";
}

int Bullet::GetDamage() const
{
    return damage;
}

// Trabalho 6
void Bullet::NotifyCollision(GameObject &other)
{
    if (!other.GetComponent("Bullet") && !associated.IsDead() && !exploded)
    {
        Component *comp = other.GetComponent("Character");
        Character *character = (Character *)comp;
        if (character)
        {
            if ((targetsPlayer && character == Character::player) || (!targetsPlayer && character != Character::player)){

                TriggerExplosion();
            }
        }
        else
        {
            TriggerExplosion();
        }
    }
}

void Bullet::TriggerExplosion()
{
    Animator *animator = static_cast<Animator *>(associated.GetComponent("Animator"));
    if (animator)
        animator->SetAnimation("explode");

    Explode.Play(1);
    exploded = true;
    explodeTimer.Restart();
}