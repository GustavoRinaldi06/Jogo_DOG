#include "../include/Barrier.h"
#include "../include/InputManager.h"
#include "Collider.h"
#include "Bullet.h"
#include "Character.h"
#include "../include/Camera.h"

#include <iostream>

Barrier::Barrier(GameObject &associated, int hp, const std::string &spritePath, const std::string &destruct, const std::string &hitpath) 
: Component(associated), hitpoints(hp), destructSound(destruct), hitSound(hitpath)
{
    // Criando um SpriteRenderer 
    SpriteRenderer *sprite = new SpriteRenderer(associated, spritePath , 3, 2);
    sprite->SetCameraFollower(false);
    associated.AddComponent(sprite); // Adicionando o componente ao GameObject

    // Cria o Animator
    Animator *animator = new Animator(associated);

    // Adiciona animações de vida
    animator->AddAnimation("stage1", Animation{0, 0, 0});
    animator->AddAnimation("st1dmg", Animation{1, 1, 0});

    animator->AddAnimation("stage2", Animation{2, 2, 0});
    animator->AddAnimation("st2dmg", Animation{3, 3, 0});

    animator->AddAnimation("stage3", Animation{4, 4, 0});
    animator->AddAnimation("stg3dmg", Animation{5, 5, 0});

    // Animação inicial
    associated.AddComponent(animator); // Adiciona o Animator ao GameObject

    hit = false; // Não está levando dano

    Collider *collider = new Collider(associated);
    collider->tag = "wall";
    associated.AddComponent(collider);

    stage2 = hitpoints * (2 / 3);
    stage3 = hitpoints * (1 / 3);
}

void Barrier::Damage(int damage)
{
    hitpoints -= damage;

    Animator *animator = (Animator *)associated.GetComponent("Animator");

    if (hitpoints <= 0)
    {
        if (animator)
        {
            destructSound.Play(1);
            // REmove o colisor
            Collider *col = dynamic_cast<Collider *>(associated.GetComponent("Collider"));
            if (col)
            {
                associated.RemoveComponent(col);
                delete col;
            }
        }
    }
    else
    {
        // Só faz hit se ainda está vivo
        hit = true;
        hitTimer.Restart();

        if (animator)
        {
            if(hitpoints > stage2){
                animator->SetAnimation("st1dmg");
            }else if(hitpoints > stage3){
                animator->SetAnimation("st2dmg");
            }
            else{
                animator->SetAnimation("stg3dmg");
            }
        }
    }
    hitSound.Play(1);
}

void Barrier::Update(float dt)
{
    // Atualiza o tempo da animação
    Animator *animator = (Animator *)associated.GetComponent("Animator");
    if (animator)
    {
        animator->Update(dt);
    }

    // Atualiza timers
    hitTimer.Update(dt);
    deathTimer.Update(dt);

    if (hitpoints <= 0) // está morto
    {
        // Se já morto deleta o objeto
        associated.RequestDelete();
        
    }
    else
    {
        // Se levou hit e já passou 0.5s
        if (hit && hitTimer.Get() > 0.5f)
        {
            if (animator)
            {
                if (hitpoints > stage2)
                {
                    animator->SetAnimation("stage1");
                }
                else if (hitpoints > stage3)
                {
                    animator->SetAnimation("stage2");
                }
                else
                {
                    animator->SetAnimation("stage3");
                }
            hit = false;
            }
        }
    }
}

void Barrier::Render()
{
    // Nada a ser feito
}

bool Barrier::Is(const std::string &type)
{
    return type == "Barrier";
}

void Barrier::Start()
{
    Animator *animator = (Animator *)associated.GetComponent("Animator");
    if (animator)
    {
        animator->SetAnimation("stage1");
    }
}

void Barrier::NotifyCollision(GameObject &other)
{
    Bullet *bullet = (Bullet *)other.GetComponent("Bullet");
    if (bullet)
    {
        Damage(bullet->GetDamage());
    }
}
