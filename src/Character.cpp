#include "Character.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "State.h"
#include "Camera.h"
#include "Collider.h"
#include "Game.h"
#include "Dog.h"
#include "InputManager.h"
#include "GameData.h"
#include "Chainsaw.h"
#include "DamageObj.h"
#include "FallingBranch.h"
#include "Deer.h"

#include <algorithm>
#include <iostream>

Character *Character::player = nullptr;

Character::Character(GameObject &associated, const std::string &spritePath)
    : Component(associated), linearSpeed(250), hp(GameData::playerHP), gravity(1000.0f)
{
    if (player == nullptr)
    {
        player = this;
    }

    auto renderer = new SpriteRenderer(associated, spritePath, 8, 8);
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);

    associated.box.w = 50;  // ou a largura desejada
    associated.box.h = 100; // altura desejada
    renderer->SetScale(0.2f, 0.2f);

    // Sons do caçador
    hitSound = Sound("recursos/audio/Hunter/Dano.wav");    // Levou dano
    deathSound = Sound("recursos/audio/Hunter/Death.wav"); // Morreu
    // jumpSound = Sound("recursos/audio/Hunter/Pulo.wav");     // Pula

    fallSound = Sound("recursos/audio/Hunter/Caindo.wav");      // Cai do mapa
    drownSound = Sound("recursos/audio/Hunter/Afogamento.wav"); // Cai do mapa 2, se afoga

    walkSound = Sound("recursos/audio/Hunter/AndandoGrama.mp3");   // Andando na grama
    hitGroundSound = Sound("recursos/audio/Hunter/CaiuGrama.wav"); // Cai no chão grama

    walkSoundMud = Sound("recursos/audio/Hunter/AndandoLama.wav");   // Andando na lama
    hitGroundSoundMud = Sound("recursos/audio/Hunter/CaiuLama.wav"); // Cai no chão lama

    // Cria as animações
    auto animator = new Animator(associated);
    animator->AddAnimation("walking", Animation(0, 4, 0.2f));
    animator->AddAnimation("run", Animation(8, 12, 0.15f));
    animator->AddAnimation("crouch", Animation(16, 20, 0.3f));
    animator->AddAnimation("damage", Animation(24, 27, 0.3f));
    animator->AddAnimation("dead", Animation(32, 36, 0.3f));
    animator->AddAnimation("jump", Animation(40, 47, 0.2f));
    animator->AddAnimation("idle", Animation(56, 58, 0.3f));
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));
    dogTimer.Set(4.0);
}

Character::~Character()
{
    if (player == this)
    {
        player = nullptr;
    }
}

void Character::Start()
{
}

void Character::Update(float dt)
{
    wasInDangerArea = isInDangerArea;
    isInDangerArea = false;

    Animator *animator = static_cast<Animator *>(associated.GetComponent("Animator"));
    GameData::playerHP = hp; // Atualiza sempre o gamedata

    // Ao morrer -------------------------------------------------------------------------------
    if ((associated.box.y > 1500 && GameData::state == 1) || (associated.box.y > 970 && GameData::state == 2))
    {
        if (!deathAnimTriggered)
        {
            hp = 0;
            deathAnimTriggered = true;
            if (GameData::state == 1)
            {
                fallSound.Play(1);
            }
            else
            {
                drownSound.Play(1);
            }

            // solta a câmera se é player
            if (this == Character::player)
                Camera::GetInstance().Unfollow();

            deathTimer.Restart();
        }

        // avança timer de morte
        deathTimer.Update(dt);

        // só deleta após s
        if (deathTimer.Get() > 4.0f)
            associated.RequestDelete();

        return; // não executa mais lógica de movimento
    }

    if (hp <= 0)
    {
        // dispara animação e som apenas uma vez
        if (!deathAnimTriggered)
        {
            hp = 0;
            deathAnimTriggered = true;
            deathSound.Play(1);

            // seta animação "dead"
            if (animator)
                animator->SetAnimation("dead");

            // solta a câmera se é player
            if (this == Character::player)
                Camera::GetInstance().Unfollow();

            deathTimer.Restart();
        }

        // avança timer de morte
        deathTimer.Update(dt);

        // só deleta após 6s
        if (deathTimer.Get() > 1.5f)
            associated.RequestDelete();

        return; // não executa mais lógica de movimento
    }

    // Enquanto vivo --------------------------------------------------------------------------
    damageCooldown.Update(dt);

    // Pega input de pulo ---------------------------------------------------------------------
    InputManager &input = InputManager::GetInstance();

    if (input.KeyPress(SDLK_SPACE) && isOnGround)
    {
        jumpSound.Play(1);
        speed.y = -850.0f; // ajuste a força de pulo conforme desejado
        isOnGround = false;
    }

    // Aplica a gravidade --------------------------------------------------------------------
    speed.y += gravity * dt;
    associated.box.y += speed.y * dt;

    // Realiza os movimentos e ações ---------------------------------------------------------

    dogTimer.Update(dt);
    if (!taskQueue.empty())
    {
        Command current = taskQueue.front();

        // MOVE ------------------------------------------------------------------------
        if (current.type == CommandType::MOVE)
        {
            Vec2 dir = (current.pos - associated.box.GetCenter()).Normalize();
            speed.x = dir.x * linearSpeed;

            associated.box.x += speed.x * dt;

            if ((associated.box.GetCenter() - current.pos).Magnitude() < 10.0f)
            {
                taskQueue.pop();
            }
        }

        // SHOOT DOG -----------------------------------------------------------------------
        else if (current.type == CommandType::SHOOT)
        {
            if (dogTimer.Get() >= 4.0) // Tempo para poder chamar o cachorro de volta
            {
                // Calcular a direção do projetil
                Vec2 shooterCenter = associated.box.GetCenter();
                Vec2 target(current.pos.x, current.pos.y);
                Vec2 delta = target - shooterCenter;
                if (delta.Magnitude() < 0.01f)
                {
                    delta = Vec2(1, 0); // se o mouse estiver em cima do personagem
                }
                Vec2 direction = delta.Normalize();

                // Características do cachorro
                float angle = atan2(direction.y, direction.x);
                float speed = 200.0f;
                int damage = 10;
                float maxDistance = 500.0f;
                bool targetsPlayer = false;

                // cria o DOG
                GameObject *dogGO = new GameObject();
                dogGO->box.x = shooterCenter.x;
                dogGO->box.y = shooterCenter.y - 150;
                dogGO->AddComponent(new Bullet(*dogGO, angle, speed, damage, maxDistance, targetsPlayer, "recursos/img/sprites/DogShoot.png", "recursos/audio/DOG/Tiro.wav", "recursos/audio/DOG/End.wav")); // Alterar para o cachorro

                Game::GetInstance().GetCurrentState().AddObject(dogGO);

                dogTimer.Restart(); // reinicia tempo pra chamar o cachorro
            }
            taskQueue.pop();
        }
    }

    // UIVO DOG ------------------------------------------------------------------------
    if (input.KeyPress('e'))
    {
        if (dogTimer.Get() >= 4.0) // Tempo para poder chamar o cachorro de volta
        {
            Vec2 shooterCenter = associated.box.GetCenter();

            GameObject *dogGO1 = new GameObject();
            if (facingDir == -1)
            { // Andando pra esquerda, chama para esquerda
                dogGO1->box.x = shooterCenter.x - 210;
            }
            else
            { // Andando pra direita, chama para direita
                dogGO1->box.x = shooterCenter.x + 70;
            }
            dogGO1->box.y = shooterCenter.y - 120;
            dogGO1->AddComponent(new Dog(*dogGO1, 20, false, "recursos/img/sprites/DogHowling.png", "recursos/audio/DOG/Latido.wav")); // Alterar para o cachorro

            Game::GetInstance().GetCurrentState().AddObject(dogGO1);

            dogTimer.Restart(); // reinicia tempo pra chamar o cachorro
        }
    }

    // Atualiza animação de acordo com a movimentação
    if (animator)
    {
        if (!isOnGround)
        {
            animator->SetAnimation("jump");
        }
        else if (fabs(speed.x) > 1.0f)
        {
            animator->SetAnimation("run");
        }
        else
        {
            animator->SetAnimation("idle");
        }
    }

    // Decide a direção que vai ficar
    if (speed.x < -0.1f)
        facingDir = -1;
    else if (speed.x > 0.1f)
        facingDir = 1;

    // Flip horizontal de acordo a direção ---------------------------------------------------------------
    SpriteRenderer *renderer = static_cast<SpriteRenderer *>(associated.GetComponent("SpriteRenderer"));
    if (renderer && animator)
    {
        int frame = animator->GetCurrentFrame();
        if (facingDir == -1) // Andando pra esquerda
            renderer->SetFrame(frame, SDL_FLIP_HORIZONTAL);
        else
            renderer->SetFrame(frame, SDL_FLIP_NONE);
    }

    // Fazer as passadas do caçador e seu som ao andar ---------------------------------------------------
    if (fabs(speed.x) > 1.0f && isOnGround == true) // está andando
    {
        walkSoundTimer.Update(dt);

        if (walkSoundTimer.Get() >= 0.3f) // intervalo entre passos
        {
            if (GameData::state == 1)
            {
                walkSound.Play(1);
            }
            else
            {
                walkSoundMud.Play(1);
            }

            walkSoundTimer.Restart();
        }
    }
    else
    {
        walkSoundTimer.Restart(); // parar de andar
    }

    speed.x = 0;
    wasInDangerArea = isInDangerArea;
}

void Character::Render() {}

bool Character::Is(const std::string &type)
{
    return type == "Character";
}

void Character::Issue(Command task)
{
    while (!taskQueue.empty())
        taskQueue.pop();
    taskQueue.push(task);
}

void Character::NotifyCollision(GameObject &other)
{
    Collider *collider = static_cast<Collider *>(other.GetComponent("Collider"));

    if (collider->tag == "danger_area")
    {
        isInDangerArea = true;
    }

    // Dano chainsaw
    Chainsaw *chain = (Chainsaw *)other.GetComponent("Chainsaw");
    if (chain && damageCooldown.Get() > 1.0f)
    {
        if (hp > 0)
        {
            hp -= chain->GetDamage();
            hitSound.Play(1);
            damageCooldown.Restart();
            return;
        }
        return;
    }

    // Dano Deer
    Deer *deer = (Deer *)other.GetComponent("Deer");
    if (deer && damageCooldown.Get() > 1.0f)
    {
        if (hp > 0)
        {
            hp -= deer->GetDamage();
            hitSound.Play(1);
            damageCooldown.Restart();
            return;
        }
        return;
    }

    // Dano falling branch
    FallingBranch *branch = (FallingBranch *)other.GetComponent("FallingBranch");
    if (branch && damageCooldown.Get() > 1.0f)
    {
        if (hp > 0)
        {
            hp -= 20; // Dano fixo de 10
            hitSound.Play(1);
            damageCooldown.Restart();
            return;
        }
        return;
    }

    // Ignorar colisões com objetos específicos
    std::vector<std::string> ignoredComponents = {"Bullet", "Dog", "Chainsaw", "FallingBranch"};
    for (const std::string &type : ignoredComponents)
    {
        if (other.GetComponent(type))
        {
            return;
        }
    }

    // Ignorar colisões com objetos  do tipo "DamageObj"
    std::vector<std::string> objIgnoraColisao = {"Thorn"};
    std::vector<std::string> objDano = {"Thorn", "Hand"};
    DamageObj *damageObj = (DamageObj *)other.GetComponent("DamageObj");
    if (damageObj)
    {
        std::string objName = damageObj->GetObjectName();

        if (std::find(objDano.begin(), objDano.end(), objName) != objDano.end() &&
            damageCooldown.Get() > 1.0f && hp > 0)
        {
            hp -= damageObj->GetDamage();
            hitSound.Play(1);
            damageCooldown.Restart();
        }

        // Ignorar colisão com objetos da lista ignorada
        if (std::find(objIgnoraColisao.begin(), objIgnoraColisao.end(), objName) != objIgnoraColisao.end())
        {
            return;
        }
    }

    // Colisão com WaterLily
    if (other.GetComponent("WaterLily") && collider->tag == "waterlily")
    {
        Rect &box = associated.box;
        Rect &otherBox = other.box;

        float overlapTop = (box.y + box.h) - otherBox.y;
        float overlapBottom = (otherBox.y + otherBox.h) - box.y;

        if (overlapTop < overlapBottom)
        {
            box.y = otherBox.y - box.h;
            speed.y = 0;

            if (!isOnGround)
            {
                if (GameData::state == 1)
                {
                    hitGroundSound.Play(1);
                }
                else
                {
                    hitGroundSoundMud.Play(1);
                }
                while (!taskQueue.empty())
                    taskQueue.pop();
            }
            isOnGround = true;
        }
        return;
    }

    if (collider)
    {
        Rect &box = associated.box; // personagem
        Rect &otherBox = other.box; // outro objeto

        // Verifica colisões com objetos em bloco (sobreposições)
        float overlapLeft = (box.x + box.w) - otherBox.x;
        float overlapRight = (otherBox.x + otherBox.w) - box.x;
        float overlapTop = (box.y + box.h) - otherBox.y;
        float overlapBottom = (otherBox.y + otherBox.h) - box.y;

        // busca a menor colisão
        float minHorizontal = std::min(overlapLeft, overlapRight);
        float minVertical = std::min(overlapTop, overlapBottom);

        if (minHorizontal < minVertical)
        {
            // Colisão horizontal
            if (overlapLeft < overlapRight)
            {
                box.x = otherBox.x - box.w; // bateu da esquerda
            }
            else
            {
                box.x = otherBox.x + otherBox.w; // bateu da direita
            }
            speed.x = 0;
        }
        else
        {
            // Colisão vertical
            if (overlapTop < overlapBottom)
            {
                box.y = otherBox.y - box.h; // bateu por cima (no chão)
                speed.y = 0;
                if (collider->tag == "ground")
                {
                    if (!isOnGround)
                    {
                        if (GameData::state == 1)
                        {
                            hitGroundSound.Play(1); // Som de colisão com o chão
                        }
                        else
                        {
                            hitGroundSoundMud.Play(1); // Som de colisão com a lama no chão
                        }

                        while (!taskQueue.empty()) // Limpa a lista de tasks feitas no ar
                        {
                            taskQueue.pop();
                        }
                    }
                    isOnGround = true;
                }
            }
            else
            {
                box.y = otherBox.y + otherBox.h; // bateu por baixo (teto)
                speed.y = 0;
            }
        }
    }
}

int Character::GetHP() const
{
    return hp;
}

int Character::GetCool() const
{
    return static_cast<int>(dogTimer.Get());
}

Vec2 Character::GetPosition() const
{
    return associated.box.GetCenter();
}
