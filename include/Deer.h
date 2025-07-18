#ifndef DEER_H
#define DEER_H

#include "Component.h"
#include "Character.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Collider.h"
#include "Rect_Vec2.h"
#include "Timer.h"
#include "Sound.h"

class Deer : public Component
{
private:
    float chaseSpeed = 200.0f; // Velocidade de perseguição
    int damage = 30;
    Character *target; // Ponteiro para o jogador
    float gravity;
    Vec2 speed;
    bool isOnGround;
    Animator *animator;
    Sound chaseSound;
    bool facingRight;

public:
    Deer(GameObject &associated);
    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string &type) override;
    void NotifyCollision(GameObject &other) override;
    int GetDamage() const { return damage; }
    Sound Damage;
};

#endif // DEER_H