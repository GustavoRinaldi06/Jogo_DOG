#include "Deer.h"
#include "Game.h"

Deer::Deer(GameObject &associated)
    : Component(associated), gravity(800.0f), isOnGround(false), facingRight(true)
{
    // Configurações gráficas
    SpriteRenderer *renderer = new SpriteRenderer(associated, "recursos/img/sprites/Deer1.png", 13, 1);
    associated.AddComponent(renderer);

    animator = new Animator(associated);
    animator->AddAnimation("chase", Animation(0, 3, 0.1f));
    animator->SetAnimation("chase");
    associated.AddComponent(animator);
    associated.AddComponent(new Collider(associated));

    // Som de perseguição
    // chaseSound.Open("recursos/audio/Deer/chase.wav");

    target = Character::player;
}

void Deer::Update(float dt)
{
    if (!target || target->GetGameObject()->IsDead())
        return;

    // Persegue o jogador
    Vec2 targetPos = target->GetGameObject()->box.GetCenter();
    Vec2 myPos = associated.box.GetCenter();

    Vec2 direction = (targetPos - myPos).Normalize();
    speed.x = direction.x * chaseSpeed;

    // Aplica gravidade
    if (!isOnGround)
    {
        speed.y += gravity * dt;
        associated.box.y += speed.y * dt;
    }

    associated.box.x += speed.x * dt;

    // Atualiza direção
    if (speed.x > 0)
        facingRight = true;
    else if (speed.x < 0)
        facingRight = false;

    // Atualiza animação
    SpriteRenderer *renderer = static_cast<SpriteRenderer *>(associated.GetComponent("SpriteRenderer"));
    if (renderer)
    {
        renderer->SetFlip(facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
    }
}

void Deer::Render() {}

bool Deer::Is(const std::string &type)
{
    return type == "Deer";
}

void Deer::NotifyCollision(GameObject &other)
{
    // Colisão com o chão
    Collider *otherCollider = static_cast<Collider *>(other.GetComponent("Collider"));
    if (otherCollider && otherCollider->tag == "ground")
    {
        isOnGround = true;
        speed.y = 0;
        associated.box.y = other.box.y - associated.box.h;
    }

    if (other.GetComponent("Character"))
    {
        Damage.Play(1);
    }
}