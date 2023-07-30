#pragma once

#include "Denoise.h"
#include <FunctionLayer/Filter/Filter.h>

class MedianFilterDenoise : public Denoise {
public:
    MedianFilterDenoise() = default;

    virtual ~MedianFilterDenoise() = default;

    MedianFilterDenoise(const Json &json);

    virtual void denoise(Film &film) override;

protected:
    int radius;
};
