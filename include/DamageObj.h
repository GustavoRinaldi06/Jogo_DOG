#ifndef DAMAGEOBJ_H
#define DAMAGEOBJ_H

#include "Component.h"
#include "Rect_Vec2.h"
#include "Timer.h"
#include "Sound.h"

class DamageObj : public Component
{
public:
    DamageObj(GameObject &associated, int damage, const std::string &spritePath, const std::string &spawnpath, const std::string &dmgpath);

    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string &type) override;

    int GetDamage() const;

    void NotifyCollision(GameObject &other) override;
private:
    int damage, originalDamage;

    Timer AnimTime;

    bool active = false;
    Timer activeTimer;
    Sound Spawn, Dmg;
};

#endif
