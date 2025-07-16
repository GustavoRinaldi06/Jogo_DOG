#pragma once

#include "Component.h"
#include "Timer.h"
#include "Sound.h"
#include "Rect_Vec2.h"
#include <string>

class Animator;
class SpriteRenderer;
class Collider;

class Chainsaw : public Component {
private:
    int damage;
    int originalDamage;
    Sound Spawn;
    Sound Damage;
    bool active;
    Timer stateTimer;
    Timer moveTimer;
    Animator* animator;
    std::string currentState;
    
    SpriteRenderer* spriteRenderer;

    float startX;
    float moveRange;
    float moveSpeed;
    int direction; // 1 para direita, -1 para esquerda

    // Física de queda
    Vec2 speed;
    float gravity;
    bool isOnGround;
    bool applyGravity;
    bool isFalling;  

public:
    Chainsaw(
        GameObject& associated,
        int damage,
        const std::string& spritePath,
        const std::string& soundpath,
        const std::string& dmgpath,
        int frameCountX,
        int frameCountY,
        float moveRange = 550.0f,
        float moveSpeed = 200.0f
    );

    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string& type) override;
    void NotifyCollision(GameObject& other) override;
    
    int GetDamage() const;
};