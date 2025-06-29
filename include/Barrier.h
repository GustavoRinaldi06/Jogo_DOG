#ifndef BARRIER_H
#define BARRIER_H

#include "Component.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Sound.h"
#include "Timer.h"

class Barrier : public Component
{
public:
    Barrier(GameObject &associated, int hitpoints, const std::string &spritePath, const std::string &destruct, const std::string &hit);

    void Damage(int dmg); // leva dano

    // Herdados de Component
    void Update(float dt) override;
    void Render() override;

    // Verifica se o componente é do tipo "Barrier"
    bool Is(const std::string &type) override;

    void Start() override;

    void NotifyCollision(GameObject &other) override;
private:
    int hitpoints; // Vida
    bool hit;
    Sound destructSound;
    Sound hitSound;

    Timer hitTimer;
    Timer deathTimer;

    int stage2, stage3;
};

#endif // BARRIER_H
