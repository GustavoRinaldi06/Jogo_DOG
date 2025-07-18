#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Animator.h"
#include "SpriteRenderer.h"
#include "Collider.h"
#include <cmath>

class FallingBranch : public Component
{
public:
    FallingBranch(GameObject &associated,
                  float fallSpeed,
                  float rotationSpeed);

    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string &type) override;

private:
    float fallSpeed;              // pixels per second ao descer
    float rotationSpeed;          // graus por segundo
    float currentRotation = 0.0f; // rotação atual
    bool falling;                 // indica se começou a cair

    SpriteRenderer *spriteRenderer;
    Animator *animator;
};