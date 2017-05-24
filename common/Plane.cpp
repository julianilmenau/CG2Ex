
#include <Plane.h>

#include <iterator>
#include <algorithm>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#pragma warning(pop)


std::vector<glm::vec3> Plane::vertices(unsigned int tessellationLevel)
{
    static const float a = 1.0f / float(tessellationLevel + 1);

    auto v = std::vector<glm::vec3>((tessellationLevel + 1) * (tessellationLevel + 1));

    for (int i=0; i <= tessellationLevel; ++i)
    {
        for (int j=0; j <= tessellationLevel; ++j)
        {
            v[i * (tessellationLevel+1) + j] = glm::vec3(i * a * 0.5, 0.0f, j * a * 0.5);
        }
    }

    return v;
}

std::vector<Plane::Face> Plane::indices(unsigned int tessellationLevel)
{
    auto idx = std::vector<Face>(2 * (tessellationLevel + 1) * (tessellationLevel + 1));

    for (int i=0; i < tessellationLevel; ++i)
    {
        for (int j=0; j < tessellationLevel; ++j)
        {
            const gl::GLint topL = i * (tessellationLevel+1) + j;
            const gl::GLint bottomL = (i+1) * (tessellationLevel+1) + j;
            const gl::GLint topR = i * (tessellationLevel+1) + (j+1);
            const gl::GLint bottomR = (i+1) * (tessellationLevel+1) + (j+1);

            idx[2 * (i * (tessellationLevel+1) + j) + 0] = Face{{ topR, topL, bottomL }};
            idx[2 * (i * (tessellationLevel+1) + j) + 1] = Face{{ bottomR, topR, bottomL }};
        }
    }

    return idx;
}

Plane::Plane()
{
}

Plane::~Plane()
{
}
