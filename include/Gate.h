#pragma once
#include "Component.h"
#include "GameObject.h"

class Gate : public Component {
public:
    Gate(GameObject& associated);
    void Update(float dt) override;
    void Render() override;
    bool Is(const std::string& type) override;
    void NotifyBark(); // Chamado quando o Dog late perto
    
private:
    int barkCount; // Contador de latidos
};