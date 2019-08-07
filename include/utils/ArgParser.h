#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <string.h>

struct Arguments
{
    char *inputName, *outputName, *normalsName;
    int width, height;
    // Optional arguments
    bool useBlinn = false;
    bool showNormals = false;
    bool shadows = false;
    bool antiAlias = false;
    float gamma = 1.0f;
    unsigned int maxBounces = 0;
};

bool parseArgs(int argc, char *argv[], Arguments *args);

#endif