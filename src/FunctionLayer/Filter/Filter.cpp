#include "Filter.h"

Filter::Filter(const Json &json) {
    radius = fetchOptional(json, "radius", 1);
}
