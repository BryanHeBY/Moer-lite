#pragma once

#include "NDF.h"

class BeckmannDistribution : public NDF {
public:
  BeckmannDistribution() noexcept = default;
  virtual ~BeckmannDistribution() noexcept = default;
  virtual float getD(const Vector3f &whLocal,
                     const Vector2f &alpha) const noexcept override {
    // TODO
    // 根据公式即可
    float cos_theta = whLocal[1];
    float tan2_theta = 1.f / (cos_theta * cos_theta) - 1.f;
    float a = alpha[0];
    return std::exp(-tan2_theta / (a * a)) /
           (PI * a * a * std::pow(cos_theta, 4));
  }
  // tips:
  // float getG1(...) {}
  float getG1(const Vector3f &w,
              const Vector2f &alpha) const noexcept {
    // TODO
    // 根据公式即可
    float cos_theta_v = w[1];
    float tan_theta_v = std::sqrt(1.f / (cos_theta_v * cos_theta_v) - 1.f);
    float a = 1 / (alpha[0] * tan_theta_v);
    if(a >= 1.6f) return 1.f;
    float a2 = a * a;
    return (3.535f * a + 2.181f * a2) / (1.f + 2.276f * a + 2.577f * a2);
  }
  virtual float getG(const Vector3f &woLocal, const Vector3f &wiLocal,
                     const Vector2f &alpha) const noexcept override {
    // TODO
    // 根据公式即可
    // tips: return getG1(wo) * getG1(wi);
    float G = getG1(woLocal, alpha) * getG1(wiLocal, alpha);
    return G;
  }
  virtual float pdf(const Vector3f &woLocal, const Vector3f &whLocal,
                    const Vector2f &alpha) const noexcept override {
    return getD(whLocal, alpha) * whLocal[1];
  }
  virtual Vector3f sampleWh(const Vector3f &woLocal, const Vector2f &alpha,
                            const Vector2f &sample) const noexcept override {
    float a = alpha[0];
    float tan_theta_2 = -std::log(1 - sample[0]) * a * a;
    float phi = sample[1] * 2 * PI;

    float cos_theta = std::sqrt(1.f / (1.f + tan_theta_2));
    float sin_theta = std::sqrt(1.f - cos_theta * cos_theta);
    return {sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta};
  }
};