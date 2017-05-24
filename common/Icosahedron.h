
#pragma once


#include <unordered_map>
#include <array>
#include <vector>

#include <glbinding/gl/types.h>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/vec3.hpp>
#pragma warning(pop)


/**
*  @brief
*    Icosahedron geometry that can be refined dynamically
*
*  Originally taken from gloperate:
*   * https://github.com/cginternals/gloperate/blob/master/source/gloperate/include/gloperate/primitives/Icosahedron.h
*   * https://github.com/cginternals/gloperate/blob/master/source/gloperate/source/primitives/Icosahedron.cpp
*/
class Icosahedron
{
public:
    using Face = std::array<gl::GLushort, 3>;

protected:
    Icosahedron();
    virtual ~Icosahedron();

public:
    static std::array<glm::vec3, 12> vertices();
    static std::array<Face, 20> indices(); /// individual triangle indices (no strip, no fan)

    /**
    *  @brief
    *    Iterative triangle refinement: split each triangle into 4 new ones and 
    *    create points and indices appropriately
    */
    static void refine(
        std::vector<glm::vec3> & vertices
    ,   std::vector<Face> & indices
    ,   unsigned char levels);

private:
    /**
    *  @brief
    *    Splits a triangle edge by adding an appropriate new point (normalized
    *    on sphere) to the points (if not already cached) and returns the index
    *    to this point.
    */
    static gl::GLushort split(
        gl::GLushort a
    ,   gl::GLushort b
    ,   std::vector<glm::vec3> & points
    ,   std::unordered_map<glm::uint, gl::GLushort> & cache);

};
