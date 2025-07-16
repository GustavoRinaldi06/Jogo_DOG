#ifndef PARALLAX_H
#define PARALLAX_H

#include "Component.h"
#include "Rect_Vec2.h"

class Parallax : public Component
{
public:
    Parallax(GameObject &associated, float factor);
    Parallax(GameObject &associated, float horizontalFactor, float verticalFactor);

    void Update(float dt) override;
    void Render() override {}
    bool Is(const std::string &type) override;

private:
    float horizontalParallaxFactor;
    float verticalParallaxFactor;
};

#endif
