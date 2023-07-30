#pragma once

#include "Filter.h"

class Box : public Filter {
public:
    Box(const Json &json);

    virtual ~Box() override = default;

    virtual float evaluate(float x, float y) const override;
};
