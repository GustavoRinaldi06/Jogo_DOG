#ifndef CAMERA_H
#define CAMERA_H

#include "Rect_Vec2.h"
#include "GameObject.h"

class Camera
{
public:
    static Camera &GetInstance();

    void Follow(GameObject *newFocus);
    void Unfollow();

    void Update(float dt);
    Vec2 GetPosition() const; // <- aqui estava o erro

private:
    Camera(); // <- construtor declarado corretamente
    Camera(const Camera &) = delete;
    Camera &operator=(const Camera &) = delete;

    Vec2 pos;
    Vec2 speed;
    GameObject *focus;
};

#endif
