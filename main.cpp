#include <chrono>
#include <random>

#include "BitmapImage.hpp"
#include "SceneParser.h"
#include "RayTracer.h"
#include "Image.h"
#include "ArgParser.h"
#include "AntiAlias.h"

using namespace std;

default_random_engine dre(chrono::steady_clock::now().time_since_epoch().count());
float randomFloat(float min, float max)
{
    uniform_real_distribution<float> res{min, max};
    return res(dre);
}

void renderAA(Arguments *args)
{
    SceneParser *scene = new SceneParser(args->inputName);
    RayTracer rtx = RayTracer(scene, args);

    // Sampling scale
    unsigned int scale = 3;
    // Gaussian blur kernel
    const float kernel[]{0.1201, 0.2339, 0.2931, 0.2339, 0.1201};

    int widthHigh = args->width * scale;
    int heightHigh = args->height * scale;
    Image imgHigh{widthHigh, heightHigh};
    scene->getCamera()->setAspect((float)args->width / args->height);

    float wStepHigh = 1.0f / widthHigh;
    float hStepHigh = 1.0f / heightHigh;

    for (unsigned int w = 0; w < widthHigh; w++)
    {
        for (unsigned int h = 0; h < heightHigh; h++)
        {
            // Jittered sampling
            float x = w + randomFloat(-0.5, 0.5);
            float y = h + randomFloat(-0.5, 0.5);
            // Calculate screen space coordinates
            Vector2f pixel{2.0f * (float(x) + 0.5f) * wStepHigh - 1, 2.0f * (float(y) + 0.5f) * hStepHigh - 1};
            // Generate ray through pixel and compute resulting color
            Vector3f color = rtx.computeColor(pixel);
            imgHigh.SetPixel(w, h, color);
        }
    }

    // Apply gaussian blur and downsampling
    Image temp = gaussianBlur(imgHigh, kernel);
    Image img = downSample(temp, scale);

    // Save images
    img.SaveImage(args->outputName);
}

void render(Arguments *args, bool normalsOnly = false)
{
    // First, parse the scene using SceneParser.
    SceneParser *scene = new SceneParser(args->inputName);
    RayTracer rtx = RayTracer(scene, args);

    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    Image img{args->width, args->height};
    Image normals{args->width, args->height};
    // Set camera aspect ratio
    scene->getCamera()->setAspect((float)args->width / args->height);

    // Step sizes for pixel coordinates
    float wStep = 1.0f / args->width;
    float hStep = 1.0f / args->height;

    for (unsigned int w = 0; w < args->width; w++)
    {
        for (unsigned int h = 0; h < args->height; h++)
        {
            // Calculate screen space coordinates
            Vector2f pixel{2.0f * (float(w) + 0.5f) * wStep - 1, 2.0f * (float(h) + 0.5f) * hStep - 1};
            // Generate ray through pixel and compute resulting color
            Vector3f normalViz;
            Vector3f color = rtx.computeColor(pixel, normalViz);
            img.SetPixel(w, h, color);

            // Set normal visualization pixel color
            if (args->showNormals)
            {
                // Convert [-1, 1] -> [0, 1]
                normalViz = (normalViz + Vector3f{1}) / 2.0f;
                normals.SetPixel(w, h, normalViz);
            }
        }
    }

    // Save images
    if (!normalsOnly)
        img.SaveImage(args->outputName);
    if (args->showNormals)
        normals.SaveImage(args->normalsName);
}

int main(int argc, char *argv[])
{
    // This loop loops over each of the input arguments.
    // argNum is initialized to 1 because the first
    // "argument" provided to the program is actually the
    // name of the executable (in our case, "a4").
    for (int argNum = 0; argNum < argc; argNum++)
    {
        cout << argv[argNum] << " ";
    }
    cout << endl;

    // Parse arguments
    Arguments *args = new Arguments;
    if (!parseArgs(argc, argv, args))
        return -1;

    if (args->antiAlias)
    {
        renderAA(args);
        if (args->showNormals)
        {
            args->maxBounces = 0;
            render(args, true);
        }
    }
    else
    {
        render(args);
    }

    return 0;
}
