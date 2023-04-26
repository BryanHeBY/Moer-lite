#pragma once

#include "NDF.h"

class GGXDistribution : public NDF {
public:
  GGXDistribution() noexcept = default;
  virtual ~GGXDistribution() noexcept = default;
  virtual float getD(const Vector3f &whLocal,
                     const Vector2f &alpha) const noexcept override {
    // TODO
    // 根据公式即可
    float cos_theta = whLocal[1];
    float tan_2_theta = 1.f / (cos_theta * cos_theta) - 1.f;
    float a2 = alpha[0] * alpha[0];
    return a2 / (PI * std::pow(cos_theta, 4) * std::pow(a2 + tan_2_theta, 2));
  }
  // tips:
  // float getG1(...) {}
  float getG1(const Vector3f &w,
              const Vector3f &wh,
              const Vector2f &alpha) const noexcept {
    // TODO
    // 根据公式即可
    float cos_theta_v = dot(w, wh);
    float tan_2_theta_v = 1.f / (cos_theta_v * cos_theta_v) - 1.f;
    float a2 = alpha[0] * alpha[0];
    return 2.f / (1.f + std::sqrt(1.f + a2 * tan_2_theta_v));
  }
  virtual float getG(const Vector3f &woLocal, const Vector3f &wiLocal,
                     const Vector2f &alpha) const noexcept override {
    // TODO
    // 根据公式即可
    // tips: return getG1(wo) * getG1(wi);
    Vector3f wh = normalize(woLocal + wiLocal);
    float G = getG1(woLocal, wh, alpha) * getG1(wiLocal, wh, alpha);
    return G;
  }
  virtual float pdf(const Vector3f &woLocal, const Vector3f &whLocal,
                    const Vector2f &alpha) const noexcept override {
    return getD(whLocal, alpha) * whLocal[1];
  }
  virtual Vector3f sampleWh(const Vector3f &woLocal, const Vector2f &alpha,
                            const Vector2f &sample) const noexcept override {
    float a = alpha[0];
    float tan_theta_2 = a * a * sample[0] / (1.f - sample[0]);
    float phi = sample[1] * 2 * PI;

    float cos_theta = std::sqrt(1.f / (1.f + tan_theta_2));
    float sin_theta = std::sqrt(1.f - cos_theta * cos_theta);
    return {sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta};
  }
};