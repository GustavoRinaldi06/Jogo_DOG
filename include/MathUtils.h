#pragma once

namespace Math {
  float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float dt);
}
