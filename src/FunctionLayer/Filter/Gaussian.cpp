#include "Gaussian.h"

Gaussian::Gaussian(const Json &json) : Filter(json) {
    sigma = fetchOptional(json, "sigma", 1.5f);
}

float Gaussian::evaluate(float x, float y) const {
    return std::exp(-(x * x + y * y) / (2 * sigma * sigma));
}

REGISTER_CLASS(Gaussian, "gaussian")
