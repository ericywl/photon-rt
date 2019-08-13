#ifndef HELPER_H
#define HELPER_H

#include <chrono>
#include <random>
#include <cassert>
#include "Vector3f.h"

float randomFloat(float min, float max);

int randomInt(int min, int max);

Vector3f randomUnitVector();

Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming);

bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming,
                          float rIndex1, float rIndex2,
                          Vector3f &transmitted);

float computeReflectance(const Vector3f &normal, const Vector3f &incoming,
                         float refrIndex, float nextRefrIndex,
                         Vector3f &transmitted);

#endif