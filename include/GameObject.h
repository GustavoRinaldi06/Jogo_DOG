#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <memory>
#include "Component.h"
#include "Rect_Vec2.h"

class GameObject
{
public:
    GameObject(); // Inicializa isDead como falso
    ~GameObject();

    void Update(float dt);
    void Render();
    bool IsDead() const; // Retorna se está morto
    void RequestDelete(); // Atribui True para IsDead

    void AddComponent(Component *cpt); // adiciona componentes a objeto
    void RemoveComponent(Component *cpt); // remove componentes a objeto
    Component *GetComponent(const std::string &type); // retorna ponteiro de componente

    Rect box;

    //Trabahlo 5
    bool started;
    void Start();

    double angleDeg = 0; // usado para rotação

    //Trabalho 6
    void NotifyCollision(GameObject &other);

private:
    std::vector<Component *> components; // Um objeto é composto de componentes
    bool isDead;
};

#endif // GAMEOBJECT_H
