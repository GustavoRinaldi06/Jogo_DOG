#ifndef DOG_H
#define DOG_H

#include "Component.h"
#include "Rect_Vec2.h"
#include "Timer.h"
#include "Sound.h"

class Dog : public Component
{
public:
    Dog(GameObject &associated, float angle, float speed, int damage, float maxDistance, bool targetsPlayer, const std::string &soundpath, const std::string &explodepath);

    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string &type) override;

    int GetDamage() const;

    void NotifyCollision(GameObject &other) override;
    bool IsTargetingPlayer() const { return targetsPlayer; }

    void TriggerExplosion();

private:
    Vec2 velocity;
    float distanceLeft;
    int damage;
    bool targetsPlayer;

    Timer SpawnTime;

    bool exploded = false;
    Timer explodeTimer;
    Sound Run, Explode;
};

#endif
