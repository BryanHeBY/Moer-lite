#pragma once

#include <ResourceLayer/JsonUtil.h>
#include <ResourceLayer/Factory.h>

class Filter {
public:
    Filter(const Json &json);

    virtual ~Filter() = default;

    virtual float evaluate(float x, float y) const = 0;

    int radius;
};
