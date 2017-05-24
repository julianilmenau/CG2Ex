
#pragma once


#include <unordered_map>
#include <array>
#include <vector>

#include <glbinding/gl/types.h>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/vec3.hpp>
#pragma warning(pop)


class Cylinder
{
public:
    using Face = std::array<gl::GLushort, 3>;

protected:
    Cylinder();
    virtual ~Cylinder();

public:
    static std::vector<glm::vec3> vertices(unsigned char tessellationLevel);
    static std::vector<Face> indices(unsigned char tessellationLevel); /// individual triangle indices (no strip, no fan)

};
