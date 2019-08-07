#include "AntiAlias.h"

Image gaussianBlur(const Image &img, const float kernel[5])
{
    int width = img.Width(),
          height = img.Height();

    Image temp{img.Width(), img.Height()};
    Image res{img.Width(), img.Height()};
    // Horizontal gaussian
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            Vector3f pixels[]{
                ((j - 2) > 0) ? img.GetPixel(i, j - 2) : img.GetPixel(i, 0),
                ((j - 1) > 0) ? img.GetPixel(i, j - 1) : img.GetPixel(i, 0),
                img.GetPixel(i, j),
                ((j + 1) < height - 1) ? img.GetPixel(i, j + 1) : img.GetPixel(i, height - 1),
                ((j + 2) < height - 1) ? img.GetPixel(i, j + 2) : img.GetPixel(i, height - 1)};

            Vector3f color = Vector3f::ZERO;
            for (unsigned int x = 0; x < 5; x++)
            {
                color += pixels[x] * kernel[x];
            }

            temp.SetPixel(i, j, color);
        }
    }

    // Vertical gaussian
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            Vector3f pixels[]{
                ((i - 2) > 0) ? temp.GetPixel(i - 2, j) : temp.GetPixel(0, j),
                ((i - 1) > 0) ? temp.GetPixel(i - 1, j) : temp.GetPixel(0, j),
                temp.GetPixel(i, j),
                ((i + 1) < width - 1) ? temp.GetPixel(i + 1, j) : temp.GetPixel(width - 1, j),
                ((i + 2) < width - 1) ? temp.GetPixel(i + 2, j) : temp.GetPixel(width - 1, j)};

            Vector3f color = Vector3f::ZERO;
            for (unsigned int x = 0; x < 5; x++)
            {
                color += pixels[x] * kernel[x];
            }

            res.SetPixel(i, j, color);
        }
    }

    return res;
}

Image downSample(const Image &img, unsigned int scale)
{
    // Down sampling
    assert(scale > 0);

    int width = img.Width() / scale,
        height = img.Height() / scale;

    Image res{width, height};
    for (unsigned int w = 0; w < width; w++)
    {
        for (unsigned int h = 0; h < height; h++)
        {
            Vector3f color = Vector3f::ZERO;
            for (unsigned int sw = 0; sw < scale; sw++)
            {
                for (unsigned int sh = 0; sh < scale; sh++)
                {
                    color += img.GetPixel(scale * w + sw, scale * h + sh);
                }
            }

            color = color / (scale * scale);
            res.SetPixel(w, h, color);
        }
    }

    return res;
}
