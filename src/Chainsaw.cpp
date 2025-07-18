#include "Chainsaw.h"
#include "animator.h"
#include "SpriteRenderer.h"
#include "Collider.h"
#include <cmath>

Chainsaw::Chainsaw(
    GameObject& associated,
    int damage,
    const std::string& spritePath,
    const std::string& soundpath,
    const std::string& dmgpath,
    int frameCountX,
    int frameCountY,
    float moveRange,
    float moveSpeed
) : Component(associated), damage(damage), originalDamage(damage),
    Spawn(soundpath), Damage(dmgpath), moveRange(moveRange), moveSpeed(moveSpeed),
    gravity(800.0f), isOnGround(false), applyGravity(true), isFalling(false)
{
    // Guarda posição inicial para movimento
    startX = associated.box.x;
    direction = -1; // Começa movendo para a esquerda
    
    auto renderer = new SpriteRenderer(associated, spritePath, frameCountX, frameCountY);
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);
    spriteRenderer = renderer;
    spriteRenderer->SetAlpha(0);

    animator = new Animator(associated);
    animator->AddAnimation("appear", Animation(1, 3, 0.5f));  // Animação de aparecimento
    animator->AddAnimation("active", Animation(4, 4, 0));     // Estado ativo
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));
    Damage = Sound("recursos/audio/Enemy/CS.wav");
    Spawn = Sound("recursos/audio/Enemy/Off.wav");

    // Estado inicial: invisível e inativo
    currentState = "hidden";
    active = false;
    renderer->SetAlpha(0);
    
    stateTimer.Restart();
    moveTimer.Restart();
    speed = Vec2(0, 0);
}

void Chainsaw::Update(float dt)
{
    animator->Update(dt);
    stateTimer.Update(dt);
    moveTimer.Update(dt);

    // Máquina de estados
    if (currentState == "hidden") {
        // Espera 2 segundos antes de aparecer
        if (stateTimer.Get() > 2.0f) {
            spriteRenderer->SetAlpha(255);  // Torna visível
            animator->SetAnimation("appear");
            currentState = "appearing";
            stateTimer.Restart();
        }
    }
    else if (currentState == "appearing") {
        // Terminou animação de aparecimento?
        if (stateTimer.Get() >= 0.5f) {
            animator->SetAnimation("active");
            currentState = "active";
            active = true;
            stateTimer.Restart();
        }
    }
    else if (currentState == "active") {
        // Movimento contínuo de vai-e-volta
        float newX = associated.box.x + (moveSpeed * dt * direction);
        
        // Verifica se atingiu os limites do movimento
        if (newX > startX + moveRange) {
            newX = startX + moveRange;
            direction = -1; // Inverte para esquerda
        } 
        else if (newX < startX - moveRange) {
            newX = startX - moveRange;
            direction = 1; // Inverte para direita
        }
        
        associated.box.x = newX;

         if (direction == 1) {
            spriteRenderer->SetFlip(SDL_FLIP_HORIZONTAL); // Vira para direita
        } else {
            spriteRenderer->SetFlip(SDL_FLIP_NONE); // Vira para esquerda
        }
    }

    // Aplica gravidade
    if (applyGravity && !isOnGround) {
        speed.y += gravity * dt;
        associated.box.y += speed.y * dt;
        isFalling = true;
    }
    else {
        isFalling = false;
    }
}

void Chainsaw::Render()
{}

bool Chainsaw::Is(const std::string& type)
{
    return type == "Chainsaw";
}

int Chainsaw::GetDamage() const
{
    return active ? damage : 0;  // Só causa dano quando ativo
}

void Chainsaw::NotifyCollision(GameObject& other)
{
    if (other.GetComponent("Dog") || other.GetComponent("Bullet"))
    {
        Spawn.Play(1);
    }
        Collider *otherCollider = (Collider *)other.GetComponent("Collider");
    if (otherCollider && otherCollider->tag == "ground") {
        isOnGround = true;
        speed.y = 0;  // Para o movimento vertical
        
        // Ajusta a posição para ficar sobre o chão
        Rect& box = associated.box;
        box.y = other.box.y - box.h;
    }

    if (active && other.GetComponent("Character")) {
        Damage.Play(1);
        // Não aumenta dano infinitamente
        damage = std::min(damage + 1, originalDamage + 5); // Limite máximo
    }
}