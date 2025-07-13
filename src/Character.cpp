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

#include <iostream>

Character *Character::player = nullptr;

Character::Character(GameObject &associated, const std::string &spritePath)
    : Component(associated), linearSpeed(200), hp(GameData::playerHP), gravity(1000.0f)
{
    if (player == nullptr){
        player = this;
    }

    auto renderer = new SpriteRenderer(associated, spritePath, 5, 1);
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);

    associated.box.w = 50;  // ou a largura desejada
    associated.box.h = 100; // altura desejada
    renderer->SetScale(0.2f, 0.2f);

    // Sons do caçador
    hitSound = Sound("recursos/audio/Hunter/Dano.wav");    // Levou dano
    deathSound = Sound("recursos/audio/Hunter/Death.wav");     // Morreu
    jumpSound = Sound("recursos/audio/Hunter/boing.mp3");     // Pula

    fallSound = Sound("recursos/audio/Hunter/Caindo.wav");    // Cai do mapa
    drownSound = Sound("recursos/audio/Hunter/Afogamento.wav"); // Cai do mapa 2, se afoga

    walkSound = Sound("recursos/audio/Hunter/AndandoGrama.mp3");      // Andando na grama                               
    hitGroundSound = Sound("recursos/audio/Hunter/CaiuGrama.wav");    // Cai no chão grama

    walkSoundMud = Sound("recursos/audio/Hunter/AndandoLama.wav");  // Andando na lama
    hitGroundSoundMud = Sound("recursos/audio/Hunter/CaiuLama.wav");  // Cai no chão lama

    // Cria as animações
    auto animator = new Animator(associated);
    animator->AddAnimation("idle", Animation(6, 9, 0.5f));
    animator->AddAnimation("walking", Animation(0, 5, 0.2f));
    animator->AddAnimation("dead", Animation(10, 11, 0.3f));
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));
    dogTimer.Set(4.0);
}

Character::~Character()
{
    if (player == this)
        player = nullptr;
}

void Character::Start()
{}


void Character::Update(float dt)
{
    GameData::playerHP = hp; // Atualiza sempre o gamedata

    // Ao morrer -------------------------------------------------------------------------------
    if (associated.box.y > 1200)
    {
        if (!deathAnimTriggered){
            hp = 0;
            deathAnimTriggered = true;
            if(GameData::state == 1){
                fallSound.Play(1);
            }else{
                drownSound.Play(1);
            }
                

            // solta a câmera se é player
            if(this == Character::player)
                Camera::GetInstance().Unfollow();

            deathTimer.Restart();
        }

        // avança timer de morte
        deathTimer.Update(dt);

        // só deleta após 5s
        if (deathTimer.Get() > 5.0f)
            associated.RequestDelete();

        return; // não executa mais lógica de movimento
    }

    if (hp <= 0){
        // dispara animação e som apenas uma vez
        if (!deathAnimTriggered)
        {
            deathAnimTriggered = true;
            deathSound.Play(1);

            // seta animação "dead"
            Animator *animator = static_cast<Animator *>(associated.GetComponent("Animator"));
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
        if (deathTimer.Get() > 6.0f)
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
        speed.y = -600.0f; // ajuste a força de pulo conforme desejado
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

            if ((associated.box.GetCenter() - current.pos).Magnitude() < 10.0f){
                taskQueue.pop();
            }

        }

        // SHOOT -----------------------------------------------------------------------
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
                dogGO->box.y = shooterCenter.y - 20;
                dogGO->AddComponent(new Bullet(*dogGO, angle, speed, damage, maxDistance, targetsPlayer, "recursos/img/Bullet.png", "recursos/audio/DOG/LOBOteste.mp3", "recursos/audio/DOG/explode.mp3")); // Alterar para o cachorro

                Game::GetInstance().GetCurrentState().AddObject(dogGO);

                dogTimer.Restart(); // reinicia tempo pra chamar o cachorro
            }
            taskQueue.pop();
        }
    }

    // DOG ------------------------------------------------------------------------
    if (input.KeyPress('e'))
    {
        if (dogTimer.Get() >= 4.0) // Tempo para poder chamar o cachorro de volta
        {
            Vec2 shooterCenter = associated.box.GetCenter();

            GameObject *dogGO1 = new GameObject();
            if (facingDir == -1){ // Andando pra esquerda, chama para esquerda
                dogGO1->box.x = shooterCenter.x - 110;
            }
            else{ // Andando pra direita, chama para direita
                dogGO1->box.x = shooterCenter.x + 70;
            }
            dogGO1->box.y = shooterCenter.y - 30;
            dogGO1->AddComponent(new Dog(*dogGO1, 20, false, "recursos/img/Enemy.png", "recursos/audio/DOG/LOBOteste.mp3")); // Alterar para o cachorro

            Game::GetInstance().GetCurrentState().AddObject(dogGO1);

            dogTimer.Restart(); // reinicia tempo pra chamar o cachorro
        }
    }

    // Atualiza animação de acordo com a movimentação
    Animator *animator = static_cast<Animator *>(associated.GetComponent("Animator"));
    if (animator)
    {
        animator->Update(dt);
        if (fabs(speed.x) > 1.0f)
            animator->SetAnimation("walking");
        else
            animator->SetAnimation("idle");
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
            if (GameData::state == 1){
                walkSound.Play(1);
            }else{
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
    Chainsaw *chain = (Chainsaw *)other.GetComponent("Chainsaw");
    if (chain && damageCooldown.Get() > 1.0f)
    {
        if(hp > 0){
            hp -= chain->GetDamage();
            hitSound.Play(1);
            damageCooldown.Restart();
            return;
        }
        return;
    }

    std::vector<std::string> ignoredComponents = {"Bullet", "Dog", "Chainsaw"};
    for (const std::string& type : ignoredComponents) {
        if (other.GetComponent(type)) {
            return;
        }
    }

    Collider *collider = (Collider *)other.GetComponent("Collider");
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
                    if(!isOnGround){
                        if (GameData::state == 1)
                        {
                            hitGroundSound.Play(1); // Som de colisão com o chão
                        }else{
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