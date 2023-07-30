#include "Box.h"

Box::Box(const Json &json) : Filter(json) {}

float Box::evaluate(float x, float y) const {
    return 1.f;
}

REGISTER_CLASS(Box, "box")
