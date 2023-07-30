#pragma once

#include "Denoise.h"

class OpenImageDenoise : public Denoise {
public:
    OpenImageDenoise() = default;

    virtual ~OpenImageDenoise() = default;

    OpenImageDenoise(const Json &json) {}

    virtual void denoise(Film &film) override;
};
