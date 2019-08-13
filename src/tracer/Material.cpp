#include "Material.h"

Vector3f Material::diffuseShade(const Ray &ray,
                                const Hit &hit,
                                const Vector3f &normal,
                                const ShadeArgs &sArgs,
                                const bool disregardL)
{
    // Diffuse shading
    // Thanks to http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/
    Vector3f difCol = diffuseColor;
    if (noise.valid())
    {
        // Use noise if valid
        difCol = noise.getColor(ray.pointAtParameter(hit.getT()));
    }
    else if (t.valid() && hit.hasTex)
    {
        // Use textures if there is a valid one
        difCol = t(hit.texCoord[0], hit.texCoord[1]);

        if (c.valid())
        {
            difCol = difCol + c(hit.texCoord[0], hit.texCoord[1]);
            // Clamp diffuse color to range [0, 1]
            for (unsigned int i = 0; i < 3; i++)
            {
                difCol[i] = (difCol[i] > 1.0f) ? 1.0f : difCol[i];
            }
        }
    }

    if (disregardL)
        return difCol;

    float LN = max(Vector3f::dot(sArgs.dirToLight, normal), 0.0f);
    return LN * difCol * sArgs.lightCol;
}

Vector3f Material::specularShade(const Ray &ray,
                                 const Vector3f &normal,
                                 const ShadeArgs &sArgs,
                                 const bool disregardL)
{
    // Specular shading
    // Thanks to http://learnwebgl.brown37.net/09_lights/lights_specular.html
    if (disregardL)
    {
        return specularColor;
    }

    Vector3f rayDir = ray.getDirection().normalized();
    if (sArgs.blinn)
    {
        Vector3f halfwayDir = (-rayDir + sArgs.dirToLight).normalized();
        // Shininess is increased by exponent to compensate for the smaller angle between
        // halfway vector and normal (as compared to ray direction and reflection direction)
        // The exponent can be further tuned
        float exponent = 3.5f;
        float HNs = pow(max(Vector3f::dot(normal, halfwayDir), 0.0f), exponent * shininess);
        return HNs * specularColor * sArgs.lightCol;
    }
    else
    {
        // Calculate reflected light
        float LN = Vector3f::dot(normal, sArgs.dirToLight);
        Vector3f reflected = (2 * LN * normal - sArgs.dirToLight).normalized();
        // Calculate specular shading intensity
        float VRs = pow(max(Vector3f::dot(-rayDir, reflected), 0.0f), shininess);
        return VRs * specularColor * sArgs.lightCol;
    }
}

Vector3f Material::shade(const Ray &ray,
                         const Hit &hit,
                         const ShadeArgs &sArgs,
                         Vector3f &normalViz,
                         const bool noSpecShade)
{
    Vector3f normal;
    if (nm.valid() && hit.hasTex)
    {
        // Apply TBN transformation to 2D normal
        Matrix3f TBN{hit.getTangent().normalized(),
                     hit.getBitangent().normalized(),
                     hit.getNormal().normalized()};

        normal = nm(hit.texCoord[0], hit.texCoord[1]);
        normal = (normal * 2.0f - Vector3f{1}).normalized();
        normal = (TBN * normal).normalized();
    }
    else
    {
        normal = hit.getNormal().normalized();
    }

    normalViz = normal;

    Vector3f retCol = diffuseShade(ray, hit, normal, sArgs, disregardLighting);
    if (!noSpecShade)
    {
        // For backward compatibility with ray casting assignment,
        // include specular shading only if specified because we
        // will be using ray tracing for the specular component
        retCol += specularShade(ray, normal, sArgs, disregardLighting);
    }

    // Gamma correction
    for (unsigned int i = 0; i < 3; i++)
    {
        retCol[i] = pow(retCol[i], 1 / sArgs.gamma);
    }

    return retCol;
}

Vector3f Material::shade(const Ray &ray,
                         const Hit &hit,
                         const ShadeArgs &sArgs,
                         const bool noSpecShade)
{
    Vector3f temp;
    return shade(ray, hit, sArgs, temp, noSpecShade);
}
