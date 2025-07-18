#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Rect_Vec2.h"
#include "Timer.h"

class DeerEating : public Component
{
public:
    DeerEating(GameObject &associated);
    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string &type) override;
    void NotifyCharacterInDangerArea(GameObject &other);
    void NotifyCollision(GameObject &other) override;

private:
    // Física de queda
    Vec2 speed;
    float gravity;
    bool isOnGround;
    bool applyGravity;
    bool isFalling;
    Timer dangerTimer;
    Timer discoverTimer;
    bool deerSpawned = false;
    std::string currentState = "";
};