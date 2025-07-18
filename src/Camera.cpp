#include "Camera.h"
#include "InputManager.h"
#include "GameData.h"
#include "MathUtils.h"


Camera::Camera() : pos(0, 0), speed(0, 0), focus(nullptr) {
    
}

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
        if(GameData::state == 2){
            pos.x = focus->box.GetCenter().x - 400;
            pos.y = 175; // altura da camera
        }
        else{
            Vec2 focusPoint = focus->box.GetCenter();
            // Ajusta a posição da câmera para seguir o foco
            focusPoint.x -= 400;
            focusPoint.y -= 400;

            pos.x = Math::SmoothDamp(pos.x, focusPoint.x, velocity.x, .15f, dt);
            float newY = Math::SmoothDamp(pos.y, focusPoint.y, velocity.y, .25f, dt);

            // só atualiza a posição se não ultrapassar o limite inferior
            if (newY < minY)
                pos.y = newY;
            else
                pos.y = minY;
        }
    }

/*     else
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
    } */

}

Vec2 Camera::GetPosition() const
{
    return pos;
}

void Camera::Move(float dx, float dy)
{
    pos.x += dx;
    pos.y += dy;
}
