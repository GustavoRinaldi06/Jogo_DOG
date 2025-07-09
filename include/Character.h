#ifndef CHARACTER_H
#define CHARACTER_H

#include "Component.h"
#include "Rect_Vec2.h"
#include "Timer.h"
#include "Sound.h"
#include "Bullet.h"
#include <queue>
#include <memory>

class Character : public Component
{
public:
    enum class CommandType
    {
        MOVE,
        SHOOT
    };

    struct Command
    {
        CommandType type;
        Vec2 pos;

        Command(CommandType type, float x, float y) : type(type), pos(x, y) {}
    };

    Character(GameObject &associated, const std::string &spritePath);
    ~Character();

    void Start() override;
    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string &type) override;
    void Issue(Command task);

    static Character *player; // ponteiro global (caso seja o player)

    void NotifyCollision(GameObject &other) override;
    GameObject *GetGameObject() { return &associated; }

    int GetHP() const;
    int GetCool() const;

private:
    std::queue<Command> taskQueue;

    Vec2 speed;
    float gravity = 1000.0f; // Gravidade
    bool isOnGround = false;

    float linearSpeed;
    int hp;
    Timer deathTimer;

    Sound hitSound, fallSound, walkSound, deathSound, jumpSound, hitGroundSound;
    Timer damageCooldown;

    bool deathAnimTriggered{false};
    Timer walkSoundTimer; // Timer para controlar o som de andar

    //Cachorro
    Timer dogTimer; //cooldown
    Sound uivo;

    int facingDir = 1; // Direção
};

#endif
