#include "MathUtils.h"
#include <cmath>

namespace Math {
  float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float dt) {
      smoothTime = std::fmax(0.0001f, smoothTime);
      float omega = 2.0f / smoothTime;
      float x = omega * dt;
      float exp = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);
      float change = current - target;
      float originalTo = target;

      target = current - change;

      float temp = (currentVelocity + omega * change) * dt;
      currentVelocity = (currentVelocity - omega * temp) * exp;
      float output = target + (change + temp) * exp;

      if (((originalTo - current) > 0.0F) == (output > originalTo)) {
          output = originalTo;
          currentVelocity = (output - originalTo) / dt;
      }

      return output;
  }
}
