
#pragma once


#include <unordered_map>
#include <array>
#include <vector>

#include <glbinding/gl/types.h>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/vec3.hpp>
#pragma warning(pop)


class Cube
{
public:
    using Face = std::array<gl::GLushort, 3>;

protected:
    Cube();
    virtual ~Cube();

public:
    static std::array<glm::vec3, 8> vertices();
    static std::array<Face, 12> indices(); /// individual triangle indices (no strip, no fan)

};
