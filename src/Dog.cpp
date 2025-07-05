#include "Dog.h"
#include "animator.h"
#include "SpriteRenderer.h"
#include "Character.h"
#include "Collider.h"
#include <cmath>

Dog::Dog(GameObject &associated, float angle, float speed, int damage, float maxDistance, bool targetsPlayer, const std::string &soundpath, const std::string &explodepath)
    : Component(associated), distanceLeft(maxDistance), damage(damage), targetsPlayer(targetsPlayer), Run(soundpath), Explode(explodepath)
{
    const std::string spritePath = "recursos/img/tiles_movimentos.png"; // caminho da imagem da bala

    velocity = Vec2(std::cos(angle), std::sin(angle)) * speed;

    auto renderer = new SpriteRenderer(associated, spritePath, 8, 10); // ajustar para animações
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);

    associated.angleDeg = angle * 180 / M_PI + 90; // Adicionar 90 para a bala ficar horizontal em relação a arma

    auto animator = new Animator(associated);
    animator->AddAnimation("spawn", Animation(0, 8, 0.5f));
    animator->AddAnimation("running", Animation(0, 8, 0.2f));
    animator->AddAnimation("explode", Animation(0, 0, 0));
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));

    SpawnTime.Restart();
    animator->SetAnimation("spawn");

    Run.Play(1);
}

void Dog::Update(float dt)
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

void Dog::Render()
{
}

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
    if (!other.GetComponent("Dog") && !associated.IsDead() && !exploded)
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

void Dog::TriggerExplosion()
{
    Animator *animator = static_cast<Animator *>(associated.GetComponent("Animator"));
    if (animator)
        animator->SetAnimation("explode");

    Explode.Play(1);
    exploded = true;
    explodeTimer.Restart();
}