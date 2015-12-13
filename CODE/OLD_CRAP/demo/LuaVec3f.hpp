/* 
 * File:   LuaVec3f.hpp
 * Author: Stas
 *
 * Created on February 13, 2012, 12:36 AM
 */

#ifndef LUAVEC3F_HPP
#define	LUAVEC3F_HPP

#include "Vec3f.hpp"

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

using namespace luabind;

static void bindLuaVec3f(lua_State* luaState)
{
    luabind::module(luaState)
            [
            class_<Vec3f > ("Vec3f")
            .def_readwrite("x", &Vec3f::x)
            .def_readwrite("y", &Vec3f::y)
            .def_readwrite("z", &Vec3f::z)

            .def(constructor<>())
            .def(constructor<float, float, float > ())

            .def(const_self + other<Vec3f > ())
            .def(const_self - other<Vec3f > ())
            .def(const_self * float())

            .def("dot", &Vec3f::dot)
            .def("cross", &Vec3f::cross)
            .def("length", &Vec3f::length)
            .def("squaredLength", &Vec3f::squaredLength)

            .scope
            [
                def("blend", &Vec3f::blend),
                def("squaredDist", &Vec3f::squaredDist)
            ]
            ];
}


#endif	/* LUAVEC3F_HPP */

