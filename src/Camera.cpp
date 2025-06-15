#include "../include/Camera.h"
#include "../include/InputManager.h"

Camera::Camera() : pos(0, 0), speed(0, 0), focus(nullptr) {}

Camera &Camera::GetInstance()
{
    static Camera instance;
    return instance;
}

void Camera::Follow(GameObject *newFocus)
{
    focus = newFocus;
}

void Camera::Unfollow()
{
    focus = nullptr;
}

void Camera::Update(float dt)
{
    if (focus != nullptr)
    {
        pos = focus->box.GetCenter() - Vec2(400, 550); // Posição da camera em relação ao foco
    }
    else
    {
        InputManager &input = InputManager::GetInstance();
        speed = Vec2();

        if (input.IsKeyDown(LEFT_ARROW_KEY) || input.IsKeyDown('a'))
            speed.x = -300;
        if (input.IsKeyDown(RIGHT_ARROW_KEY) || input.IsKeyDown('d'))
            speed.x = 300;
        if (input.IsKeyDown(UP_ARROW_KEY) || input.IsKeyDown('w'))
            speed.y = -300;
        if (input.IsKeyDown(DOWN_ARROW_KEY) || input.IsKeyDown('s'))
            speed.y = 300;

        pos = pos + speed * dt;
    }
}

Vec2 Camera::GetPosition() const
{
    return pos;
}
