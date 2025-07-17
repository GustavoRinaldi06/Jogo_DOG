#pragma once

#include "Component.h"
#include "Rect_Vec2.h"
#include <string>

class WaterLily : public Component {
public:
    WaterLily(GameObject &associated);
    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string &type) override;
    void NotifyCollision(GameObject &other) override;
    
private:
    Vec2 speed;
    float gravity;
    bool isOnGround;
    bool applyGravity;
    bool isFalling;  
};