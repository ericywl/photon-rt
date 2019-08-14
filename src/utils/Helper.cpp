#include "Helper.h"

using namespace std;

default_random_engine dre(chrono::steady_clock::now().time_since_epoch().count());

float randomFloat(float min, float max)
{
    uniform_real_distribution<float> res{min, max};
    return res(dre);
}

int randomInt(int min, int max)
{
    uniform_int_distribution<int> res{min, max};
    return res(dre);
}

Vector3f randomUnitVector()
{
    Vector3f res;
    float x, y, z;
    do
    {
        x = randomFloat(-1.0f, 1.0f);
        y = randomFloat(-1.0f, 1.0f);
        z = randomFloat(-1.0f, 1.0f);
        res = Vector3f{x, y, z};
    } while (res.absSquared() > 1.0f);
    
    return res.normalized();
}

Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming)
{
    // Compute reflected ray direction
    float NV = Vector3f::dot(normal, incoming);
    return (incoming - 2 * NV * normal).normalized();
}

bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming,
                          float rIndex1, float rIndex2,
                          Vector3f &transmitted)
{
    // Compute refracted ray direction
    // Avoid division-by-zero error
    if (rIndex2 == 0)
        return false;

    float rIndexRel = rIndex1 / rIndex2;
    Vector3f incomingN = incoming.normalized();

    float NV = Vector3f::dot(normal, incomingN);
    float root = 1 - rIndexRel * rIndexRel * (1 - NV * NV);
    // Imaginary root aka. total internal reflection
    if (root < 0)
        return false;

    float temp = rIndexRel * Vector3f::dot(normal, -incomingN);
    temp -= sqrt(root);
    transmitted = (temp * normal + rIndexRel * incoming).normalized();
    return true;
}

float computeReflectance(const Vector3f &normal, const Vector3f &incoming,
                         float refrIndex, float nextRefrIndex,
                         Vector3f &transmitted)
{
    // Compute reflectance using Schlick's approximation of Fresnel's equation
    float r0 = pow((nextRefrIndex - refrIndex) / (nextRefrIndex + refrIndex), 2);
    float c;
    if (refrIndex > nextRefrIndex)
        c = fabs(Vector3f::dot(transmitted, normal));
    else
        c = fabs(Vector3f::dot(incoming, normal));

    return r0 + pow(1 - c, 5) * (1 - r0);
}
