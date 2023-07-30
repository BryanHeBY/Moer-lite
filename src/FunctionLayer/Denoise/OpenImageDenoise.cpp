#include "OpenImageDenoise.h"
#include <OpenImageDenoise/oidn.hpp>

void OpenImageDenoise::denoise(Film &film) {
    oidn::DeviceRef device = oidn::newDevice();
    device.commit();
    oidn::FilterRef filter = device.newFilter("RT");
    int width = film.size[0], height = film.size[1];
    oidn::BufferRef colorBuffer = device.newBuffer(sizeof(float[width * height * 3]));
    oidn::BufferRef outputBuffer = device.newBuffer(sizeof(float[width * height * 3]));
    memcpy(colorBuffer.getData(), film.image->data, sizeof(float[width * height * 3]));
    filter.setImage("color", colorBuffer, oidn::Format::Float3, width, height);
    filter.setImage("output", outputBuffer, oidn::Format::Float3, width, height);
    filter.set("hdr", true);
    filter.commit();
    filter.execute();
    memcpy(film.image->data, outputBuffer.getData(), sizeof(float[width * height * 3]));
}

REGISTER_CLASS(OpenImageDenoise, "oidn");
