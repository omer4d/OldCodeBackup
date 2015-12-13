/* 
 * File:   LuaVec2f.hpp
 * Author: Stas
 *
 * Created on February 13, 2012, 12:04 AM
 */

#ifndef LUAVEC2F_HPP
#define	LUAVEC2F_HPP

#include "Vec2f.hpp"

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

using namespace luabind;

static void bindLuaVec2f(lua_State* luaState)
{
    luabind::module(luaState)
            [
            class_<Vec2f > ("Vec2f")
            .def_readwrite("x", &Vec2f::x)
            .def_readwrite("y", &Vec2f::y)

            .def(constructor<>())
            .def(constructor<float, float > ())

            .def(const_self + other<Vec2f > ())
            .def(const_self - other<Vec2f > ())
            .def(const_self * float())

            .def("dot", &Vec2f::dot)
            .def("cross", &Vec2f::cross)
            .def("length", &Vec2f::length)
            .def("squaredLength", &Vec2f::squaredLength)
            .def("approxLength", &Vec2f::approxLength)
            .def("normal", &Vec2f::normal)
            .def("normalize", &Vec2f::normalize)
            .def("unit", &Vec2f::unit)
            .def("unitNormal", &Vec2f::unitNormal)
            .def("rotated", &Vec2f::rotated)
            .def("fromBase", &Vec2f::fromBase)

            .scope
            [
                def("cosAng", &Vec2f::cosAng),
                def("ang", &Vec2f::ang),
                def("blend", &Vec2f::blend),
                def("squaredDist", &Vec2f::squaredDist)
            ]
            ];
}

#endif	/* LUAVEC2F_HPP */

