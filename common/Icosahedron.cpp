
#include <Icosahedron.h>

#include <iterator>
#include <algorithm>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#pragma warning(pop)


std::array<glm::vec3, 12> Icosahedron::vertices()
{
    static const float t = (1.f + glm::sqrt(5.f)) * 0.5f; // 2.118
    static const float i = glm::inversesqrt(t * t + 1.f);  // 0.427
    static const float a = t * i;                     // 0.904

    // icosahedron vertices (normalized) form three orthogonal golden rectangles
    // http://en.wikipedia.org/wiki/Icosahedron#Cartesian_coordinates

    return std::array<glm::vec3, 12>{{
        glm::vec3(-i, a, 0)
    ,   glm::vec3( i, a, 0)
    ,   glm::vec3(-i,-a, 0)
    ,   glm::vec3( i,-a, 0)
    ,   glm::vec3( 0,-i, a)
    ,   glm::vec3( 0, i, a)
    ,   glm::vec3( 0,-i,-a)
    ,   glm::vec3( 0, i,-a)
    ,   glm::vec3( a, 0,-i)
    ,   glm::vec3( a, 0, i)
    ,   glm::vec3(-a, 0,-i)
    ,   glm::vec3(-a, 0, i)
    }};
}

std::array<Icosahedron::Face, 20> Icosahedron::indices()
{
    return std::array<Face, 20>{{
        Face{{  0, 11,  5 }}
    ,   Face{{  0,  5,  1 }}
    ,   Face{{  0,  1,  7 }}
    ,   Face{{  0,  7, 10 }}
    ,   Face{{  0, 10, 11 }}

    ,   Face{{  1,  5,  9 }}
    ,   Face{{  5, 11,  4 }}
    ,   Face{{ 11, 10,  2 }}
    ,   Face{{ 10,  7,  6 }}
    ,   Face{{  7,  1,  8 }}

    ,   Face{{  3,  9,  4 }}
    ,   Face{{  3,  4,  2 }}
    ,   Face{{  3,  2,  6 }}
    ,   Face{{  3,  6,  8 }}
    ,   Face{{  3,  8,  9 }}

    ,   Face{{  4,  9,  5 }}
    ,   Face{{  2,  4, 11 }}
    ,   Face{{  6,  2, 10 }}
    ,   Face{{  8,  6,  7 }}
    ,   Face{{  9,  8,  1 }}
    }};
}

Icosahedron::Icosahedron()
{
}

Icosahedron::~Icosahedron()
{
}

void Icosahedron::refine(
    std::vector<glm::vec3> & vertices
,   std::vector<Face> & indices
,   const unsigned char levels)
{
    std::unordered_map<std::uint32_t, gl::GLushort> cache;

    for(int i = 0; i < levels; ++i)
    {
        const int size(static_cast<int>(indices.size()));

        for(int f = 0; f < size; ++f)
        {
            Face & face = indices[f];

            const gl::GLushort a(face[0]);
            const gl::GLushort b(face[1]);
            const gl::GLushort c(face[2]);

            const gl::GLushort ab(split(a, b, vertices, cache));
            const gl::GLushort bc(split(b, c, vertices, cache));
            const gl::GLushort ca(split(c, a, vertices, cache));

            face = { { ab, bc, ca } };

            indices.emplace_back(Face{ { a, ab, ca } });
            indices.emplace_back(Face{ { b, bc, ab } });
            indices.emplace_back(Face{ { c, ca, bc } });
        }
    }
}

gl::GLushort Icosahedron::split(
    const gl::GLushort a
,   const gl::GLushort b
,   std::vector<glm::vec3> & points
,   std::unordered_map<std::uint32_t, gl::GLushort> & cache)
{
    const bool aSmaller(a < b);

    const std::uint32_t smaller(aSmaller ? a : b);
    const std::uint32_t greater(aSmaller ? b : a);
    const std::uint32_t hash((smaller << 16) + greater);

    auto h(cache.find(hash));
    if(cache.end() != h)
        return h->second;

    points.push_back(glm::normalize((points[a] + points[b]) * .5f));

    const gl::GLushort i = static_cast<gl::GLushort>(points.size() - 1);

    cache[hash] = i;

    return i;
}
