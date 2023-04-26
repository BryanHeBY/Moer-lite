#pragma once
#include "BSDF.h"
#include "Warp.h"

class OrenNayarBSDF : public BSDF {
public:
  OrenNayarBSDF(const Vector3f &_normal, const Vector3f &_tangent,
                const Vector3f &_bitangent, Spectrum _albedo, float sigma)
      : BSDF(_normal, _tangent, _bitangent), albedo(_albedo) {}

  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const override {
    // TODO
    // 1. 转换坐标系到局部坐标
    // 2. 计算 A, B, \alpha, \beta（可以直接求\sin\alpha,\tan\beta）,
    // \cos(\phi_i-\phi_o)
    // 3. return Oren-Nayar brdf

    Vector3f woLocal = toLocal(wo), wiLocal = toLocal(wi);
    float cos_theta_i = wiLocal[1];

    float sigma_2 = sigma * sigma;
    float A = 1.f - 0.5f * sigma_2 / (sigma_2 + 0.33f);
    float B = 0.45f * sigma_2 / (sigma_2 + 0.09f);

    float cos_alpha = std::min(woLocal[1], woLocal[1]);
    float sin_alpha = std::sqrt(1.f - cos_alpha * cos_alpha);
    float cos_beta = std::max(woLocal[1], woLocal[1]);
    float tan_beta = std::sqrt(1.f - cos_beta * cos_beta) / cos_beta;

    float phi_i = std::atan2(wiLocal[2], wiLocal[0]);
    float phi_o = std::atan2(woLocal[2], woLocal[0]);
    float cos_phi = std::cos(phi_i - phi_o);

    return albedo * INV_PI * \
            (A + B * std::max(.0f, cos_phi) * sin_alpha * tan_beta) \
            * cos_theta_i;
  }

  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const override {
    Vector3f wi = squareToCosineHemisphere(sample);
    float pdf = squareToCosineHemispherePdf(wi);
    return {albedo, toWorld(wi), pdf, BSDFType::Diffuse};
  }

private:
  Spectrum albedo;
  float sigma;
};