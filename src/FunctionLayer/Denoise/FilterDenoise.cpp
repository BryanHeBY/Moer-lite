#include "FilterDenoise.h"

FilterDenoise::FilterDenoise(const Json &json) {
    filter = Factory::construct_class<Filter>(json["filter"]);
}

void FilterDenoise::denoise(Film &film) {
    int width = film.image->size[0], height = film.image->size[1];
    int radius = filter->radius, filter_sz = 2 * radius + 1;
    float *input = film.image->data;
    float *output = new float[width * height * 3];
    float *weightBuf = new float[filter_sz * filter_sz];
    
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            weightBuf[(y + radius) * filter_sz + (x + radius)] = filter->evaluate(x, y);
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float weightSum = 0;
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || nx >= width * 3 || ny < 0 || ny >= height) continue;
                    float weight = weightBuf[(dy + radius) * filter_sz + (dx + radius)];
                    weightSum += weight;
                }
            }
            for (int c = 0; c < 3; c++) {
                float sum = 0;
                for (int dy = -radius; dy <= radius; dy++) {
                    for (int dx = -radius; dx <= radius; dx++) {
                        int nx = x + dx, ny = y + dy;
                        if (nx < 0 || nx >= width * 3 || ny < 0 || ny >= height) continue;
                        float weight = weightBuf[(dy + radius) * filter_sz + (dx + radius)];
                        sum += input[(ny * width + nx) * 3 + c] * weight;
                    }
                }
                output[(y * width + x) * 3 + c] = sum / weightSum;
            }
        }
    }

    film.image->data = output;

    delete[] input;
    delete[] weightBuf;
}

REGISTER_CLASS(FilterDenoise, "filterDenoise");