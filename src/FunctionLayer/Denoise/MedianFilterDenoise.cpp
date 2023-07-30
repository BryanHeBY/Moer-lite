#include "MedianFilterDenoise.h"

MedianFilterDenoise::MedianFilterDenoise(const Json &json) {
    radius = fetchOptional(json, "radius", 2);
}

void MedianFilterDenoise::denoise(Film &film) {
    int width = film.image->size[0], height = film.image->size[1];
    int filter_sz = 2 * radius + 1;
    float *input = film.image->data;
    float *output = new float[width * height * 3];
    float *sortBuf = new float[filter_sz * filter_sz];

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < 3; c++) {
                int cnt = 0;
                for (int dy = -radius; dy <= radius; dy++) {
                    for (int dx = -radius; dx <= radius; dx++) {
                        int nx = x + dx, ny = y + dy;
                        if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                        sortBuf[cnt++] = input[(ny * width + nx) * 3 + c];
                    }
                }
                std::sort(sortBuf, sortBuf + cnt);
                output[(y * width + x) * 3 + c] = sortBuf[cnt / 2];
            }
        }
    }

    film.image->data = output;

    delete[] input;
    delete[] sortBuf;
}

REGISTER_CLASS(MedianFilterDenoise, "medianFilterDenoise");