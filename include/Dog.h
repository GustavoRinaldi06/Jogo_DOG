#ifndef DOG_H
#define DOG_H

#include "Component.h"
#include "Rect_Vec2.h"
#include "Timer.h"
#include "Sound.h"

class Dog : public Component
{
public:
    Dog(GameObject &associated, int damage, bool targetsPlayer, const std::string &spritePath, const std::string &soundpath);

    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string &type) override;

    int GetDamage() const;

    void NotifyCollision(GameObject &other) override;
    bool IsTargetingPlayer() const { return targetsPlayer; }
private:
    int damage;
    bool targetsPlayer;

    Timer RunTime;

    Sound action;

    bool collisionChecked = false;
    void CheckDestructibleCollisions();
};

#endif
