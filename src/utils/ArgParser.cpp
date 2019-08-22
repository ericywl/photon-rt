#include "ArgParser.h"

using namespace std;

bool parseArgs(int argc, char *argv[], Arguments *args)
{
    int argNum = 1;
    while (argNum < argc)
    {
        if (!strcmp(argv[argNum], "-input"))
        {
            argNum++;
            assert(argNum < argc);
            args->inputName = argv[argNum];
        }
        else if (!strcmp(argv[argNum], "-output"))
        {
            argNum++;
            assert(argNum < argc);
            args->outputName = argv[argNum];
        }
        else if (!strcmp(argv[argNum], "-size"))
        {
            argNum++;
            assert(argNum < argc);
            args->width = atoi(argv[argNum]);

            argNum++;
            assert(argNum < argc);
            args->height = atoi(argv[argNum]);
        }
        else if (!strcmp(argv[argNum], "-normals"))
        {
            argNum++;
            assert(argNum < argc);
            args->normalsName = argv[argNum];
            args->showNormals = true;
        }
        else if (!strcmp(argv[argNum], "-gamma"))
        {
            argNum++;
            assert(argNum < argc);
            args->gamma = atof(argv[argNum]);
        }
        else if (!strcmp(argv[argNum], "-blinn"))
        {
            args->useBlinn = true;
        }
        else if (!strcmp(argv[argNum], "-secRays"))
        {
            argNum++;
            assert(argNum < argc);
            args->secondaryRays = atoi(argv[argNum]);
        }
        else if (!strcmp(argv[argNum], "-rBounces"))
        {
            argNum++;
            assert(argNum < argc);
            args->rayBounces = atoi(argv[argNum]);
        }
        else if (!strcmp(argv[argNum], "-pBounces"))
        {
            argNum++;
            assert(argNum < argc);
            args->photonBounces = atoi(argv[argNum]);
        }
        else if (!strcmp(argv[argNum], "-photons"))
        {
            argNum++;
            assert(argNum < argc);
            args->maxPhotons = atoi(argv[argNum]);
        }
        else if (!strcmp(argv[argNum], "-nearest"))
        {
            argNum++;
            assert(argNum < argc);
            args->nearestNeighbors = atoi(argv[argNum]);
        }
        else if (!strcmp(argv[argNum], "-2rays"))
        {
            argNum++;
            assert(argNum < argc);
            args->secondaryRays = atoi(argv[argNum]);
        }
        else if (!strcmp(argv[argNum], "-shadows"))
        {
            args->shadows = true;
        }
        else if (!strcmp(argv[argNum], "-aa"))
        {
            args->antiAlias = true;
        }
        else
        {
            cerr << "Invalid argument: " << argv[argNum] << endl;
            return false;
        }

        argNum++;
    }

    return true;
}
