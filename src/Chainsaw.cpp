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
    Spawn(soundpath), Damage(dmgpath), moveRange(moveRange), moveSpeed(moveSpeed)
{
    // Guarda posição inicial para movimento
    startX = associated.box.x;
    direction = 1; // Começa movendo para a direita
    
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

    // Estado inicial: invisível e inativo
    currentState = "hidden";
    active = false;
    renderer->SetAlpha(0);
    
    stateTimer.Restart();
    moveTimer.Restart();
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
            Spawn.Play(1);
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
    if (active) {
        Damage.Play(1);
        // Não aumenta dano infinitamente
        damage = std::min(damage + 1, originalDamage + 5); // Limite máximo
    }
}