#include "FallingBranch.h"
#include "Game.h"

FallingBranch::FallingBranch(GameObject& associated,
                             float fallSpeed,
                             float rotationSpeed,
                             const std::string& spritePath,
                             int frameCountX,
                             int frameCountY)
    : Component(associated),
      fallSpeed(fallSpeed),
      rotationSpeed(rotationSpeed),
      currentRotation(0.0f),
      falling(false) {
    
    auto renderer = new SpriteRenderer(associated, spritePath, frameCountX, frameCountY);
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);
    spriteRenderer = renderer;

    animator = new Animator(associated);
    animator->AddAnimation("idle", Animation(0, frameCountX * frameCountY - 1, 1.0f));
    associated.AddComponent(animator);

    // Collider para trigger de início de queda
    associated.AddComponent(new Collider(associated));
}

void FallingBranch::Update(float dt) {
    animator->Update(dt);

    if (!falling) {
        // Verifica se personagem chegou embaixo para ativar a queda
        // Por exemplo, quando o player passar debaixo da branch
        // Esta parte pode ser customizada conforme lógica do jogo
        
        // Supondo que a branch cai imediatamente ao criar:
        falling = true;
    }

    if (falling) {
        // Cai para baixo
        associated.box.y += fallSpeed * dt;

        // Incrementa rotação
        currentRotation += rotationSpeed * dt;
        if (currentRotation >= 360.0f) currentRotation -= 360.0f;

        // Aplica transformação
        spriteRenderer->SetRotation(currentRotation);
    }
}

void FallingBranch::Render() {
    // A rotação já é aplicada pelo SpriteRenderer
}

bool FallingBranch::Is(const std::string& type) {
    return type == "FallingBranch";
}
