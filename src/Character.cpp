#include "Character.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "State.h" 
#include "Camera.h"
#include "Collider.h"
#include "Game.h"
#include "InputManager.h"

#include <iostream>

Character *Character::player = nullptr;

Character::Character(GameObject &associated, const std::string &spritePath)
    : Component(associated), linearSpeed(200), hp(100)
{
    if (player == nullptr){
        player = this;
    }

    auto renderer = new SpriteRenderer(associated, spritePath, 3, 4);
    renderer->SetCameraFollower(false);
    associated.AddComponent(renderer);

    associated.box.w = 50;  // ou a largura desejada
    associated.box.h = 100; // altura desejada

    // Novos sons
    //hitSound = Sound("recursos/audio/Hit1.wav");
    //deathSound = Sound("recursos/audio/Dead.wav");
    fallSound = Sound("recursos/audio/Caindo.wav");
    walkSound = Sound("recursos/audio/AndandoGrama.mp3");
    uivo = Sound("recursos/audio/LOBOteste.mp3");

    // Cria as animações
    auto animator = new Animator(associated);
    animator->AddAnimation("idle", Animation(6, 9, 0.5f));
    animator->AddAnimation("walking", Animation(0, 5, 0.2f));
    animator->AddAnimation("dead", Animation(10, 11, 0.3f));
    associated.AddComponent(animator);

    associated.AddComponent(new Collider(associated));
    dogTimer.Set(5.0);
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
    // Ao morrer -------------------------------------------------------------------------------
    if (associated.box.y > 1200)
    {
        // dispara animação e som apenas uma vez
        if (!deathAnimTriggered)
        {
            hp = 0;
            deathAnimTriggered = true;
            fallSound.Play(1);

            // seta animação "dead"
            Animator *animator = static_cast<Animator *>(associated.GetComponent("Animator"));
            if (animator)
                animator->SetAnimation("dead");

            // solta a câmera se é player
            if(this == Character::player)
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
            if (dogTimer.Get() >= 5.0) // Tempo para poder chamar o cachorro de volta
            {
                uivo.Play(1);
                
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

                // cria o cachorro
                GameObject *dogGO = new GameObject();
                dogGO->box.x = shooterCenter.x;
                dogGO->box.y = shooterCenter.y - 20;
                dogGO->AddComponent(new Bullet(*dogGO, angle, speed, damage, maxDistance, targetsPlayer, "recursos/img/Bullet.png")); // Alterar para o cachorro

                Game::GetInstance().GetCurrentState().AddObject(dogGO);

                dogTimer.Restart(); // reinicia tempo pra chamar o cachorro
            }
            taskQueue.pop();
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
            walkSound.Play(1);
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
    if (other.GetComponent("Bullet")) // ignora colisões com Bullet
    {
        return; 
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

    // Se colidir com parede agarrável (teste ainda, apenas n criar wall assim)
    if (collider && collider->tag == "climbWall")
    {
        // Ajusta posição
        associated.box.x = other.box.x - associated.box.w;
        speed.y -= 200; // Acho q vai bugar
        speed.x = 0;
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