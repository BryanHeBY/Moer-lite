#pragma once

#include <ResourceLayer/JsonUtil.h>
#include <ResourceLayer/Factory.h>
#include <FunctionLayer/Film/Film.h>

class Denoise {
public:
    Denoise() = default;

    virtual ~Denoise() = default;

    Denoise(const Json &json) {}

    virtual void denoise(Film &film) = 0;
};

class NoneDenoise : public Denoise {
public:
    NoneDenoise() = default;

    ~NoneDenoise() override = default;

    NoneDenoise(const Json &json) : Denoise(json) {}

    void denoise(Film &film) override {}
};
