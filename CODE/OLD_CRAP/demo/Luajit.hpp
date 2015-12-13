#ifndef _Luajit_hpp_
#define _Luajit_hpp_

extern "C" {
#include <luajit/lua.h>
#include <luajit/lauxlib.h>
#include <luajit/lualib.h>
}

#define LUA_PARAM_TODOUBLE(res, L, n)\
luaL_argcheck((L), lua_isnumber((L), (n)), (n), "Number expected.");\
res = lua_tonumber((L), (n))

#define LUA_PARAM_TOINT(res, L, n)\
luaL_argcheck((L), lua_isnumber((L), (n)), (n), "Number expected.");\
res = (int)lua_tonumber((L), (n))

#endif
