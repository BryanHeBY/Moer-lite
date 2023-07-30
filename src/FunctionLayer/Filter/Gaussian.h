#pragma once

#include "Filter.h"

class Gaussian : public Filter {
public:
    Gaussian(const Json &json);

    virtual ~Gaussian() override = default;

    virtual float evaluate(float x, float y) const override;

    float sigma;
};
