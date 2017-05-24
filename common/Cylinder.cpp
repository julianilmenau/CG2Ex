
#include <Cylinder.h>

#include <iterator>
#include <algorithm>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#pragma warning(pop)


std::vector<glm::vec3> Cylinder::vertices(unsigned char tessellationLevel)
{
    static const float a = 0.5f;
    static const float h = 0.5f;

    const auto numVertices = 4 * tessellationLevel;

    auto v = std::vector<glm::vec3>(2 + 2 * numVertices);

    v[0] = glm::vec3(0.0, h, 0.0);
    v[1] = glm::vec3(0.0, -h, 0.0);

    for (int i=0; i < numVertices; ++i)
    {
        auto angle = float(i) / float(numVertices) * 2.0f * glm::pi<float>();

        v[2+2*i+0] = glm::vec3(a, h, a) * glm::vec3(std::cos(angle), 1.0f, std::sin(angle));
        v[2+2*i+1] = glm::vec3(a, h, a) * glm::vec3(std::cos(angle), -1.0f, std::sin(angle));
    }

    return v;
}

std::vector<Cylinder::Face> Cylinder::indices(unsigned char tessellationLevel)
{
    const auto numVertices = 4 * tessellationLevel;

    auto idx = std::vector<Face>(4 * numVertices);

    const gl::GLushort topCenter = 0;
    const gl::GLushort bottomCenter = 1;

    for (int i=0; i < numVertices; ++i)
    {
        const gl::GLushort topL = static_cast<gl::GLushort>(2 + 2*i);
        const gl::GLushort bottomL = static_cast<gl::GLushort>(2 + 2*i + 1);
        const gl::GLushort topR = static_cast<gl::GLushort>(2 + 2 * ((i+1) % numVertices));
        const gl::GLushort bottomR = static_cast<gl::GLushort>(2 + 2 * ((i+1) % numVertices) + 1);

        idx[4*i+0] = Face{{ topR, topCenter, topL }};
        idx[4*i+1] = Face{{ bottomR, topR, topL }};
        idx[4*i+2] = Face{{ bottomR, topL, bottomL }};
        idx[4*i+3] = Face{{ bottomL, bottomCenter, bottomR }};
    }

    return idx;
}

Cylinder::Cylinder()
{
}

Cylinder::~Cylinder()
{
}
