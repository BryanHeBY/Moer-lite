#pragma once

#include "Denoise.h"
#include <FunctionLayer/Filter/Filter.h>

class FilterDenoise : public Denoise {
public:
    FilterDenoise() = default;

    virtual ~FilterDenoise() = default;

    FilterDenoise(const Json &json);

    virtual void denoise(Film &film) override;

protected:
    std::shared_ptr<Filter> filter;
};
