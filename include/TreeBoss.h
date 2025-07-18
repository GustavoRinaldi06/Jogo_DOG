#ifndef TREEBOSS_H
#define TREEBOSS_H

#include "Component.h"
#include "Rect_Vec2.h"
#include "Timer.h"
#include "Sound.h"
#include "Animator.h"

class TreeBoss : public Component
{
public:
    TreeBoss(GameObject &associated);

    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string &type) override;

    int GetDamage() const;

    void NotifyCollision(GameObject &other) override;
    std::string GetObjectName() const;

private:
    int damage = 0, originalDamage;
    std::string objectName = "TreeBoss";

    Timer AnimTime;

    bool active = false;
    Timer activeTimer;
    Sound Spawn;
    Animator *animator;
    Timer stateTimer;
};

#endif
