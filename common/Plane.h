
#pragma once


#include <unordered_map>
#include <array>
#include <vector>

#include <glbinding/gl/types.h>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/vec3.hpp>
#pragma warning(pop)


class Plane
{
public:
    using Face = std::array<gl::GLint, 3>;

protected:
    Plane();
    virtual ~Plane();

public:
    static std::vector<glm::vec3> vertices(unsigned int tessellationLevel);
    static std::vector<Face> indices(unsigned int tessellationLevel); /// individual triangle indices (no strip, no fan)

};
