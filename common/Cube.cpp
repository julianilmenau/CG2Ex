
#include <Cube.h>

#include <iterator>
#include <algorithm>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#pragma warning(pop)


std::array<glm::vec3, 8> Cube::vertices()
{
    static const float a = 0.5f;

    return std::array<glm::vec3, 8>{{
        glm::vec3( a,  a,  a) // 0
    ,   glm::vec3( a,  a, -a) // 1
    ,   glm::vec3( a, -a,  a) // 2
    ,   glm::vec3( a, -a, -a) // 3
    ,   glm::vec3(-a,  a,  a) // 4
    ,   glm::vec3(-a,  a, -a) // 5
    ,   glm::vec3(-a, -a,  a) // 6
    ,   glm::vec3(-a, -a, -a) // 7
    }};
}

std::array<Cube::Face, 12> Cube::indices()
{
    return std::array<Face, 12>{{
        Face{{ 3, 1, 2 }}
    ,   Face{{ 2, 1, 0 }}

    ,   Face{{ 2, 6, 0 }}
    ,   Face{{ 0, 6, 4 }}

    ,   Face{{ 6, 4, 7 }}
    ,   Face{{ 7, 4, 5 }}

    ,   Face{{ 5, 7, 1 }}
    ,   Face{{ 1, 7, 3 }}

    ,   Face{{ 3, 2, 7 }}
    ,   Face{{ 7, 2, 6 }}

    ,   Face{{ 1, 5, 0 }}
    ,   Face{{ 0, 5, 4 }}
    }};
}

Cube::Cube()
{
}

Cube::~Cube()
{
}
