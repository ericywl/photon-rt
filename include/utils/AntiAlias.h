#ifndef ANTIALIAS_H
#define ANTIALIAS_H

#include <cassert>
#include "Image.h"

Image gaussianBlur(const Image &img, const float kernel[5]);
Image downSample(const Image &img, unsigned int scale);

#endif