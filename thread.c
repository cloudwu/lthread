/*
	This source code is under the MIT License.

	You can build it in linux,

	gcc -O2 -o thread.so --shared thread.c

	Or in mingw,

	gcc -O2 -o thread.dll --shared thread.c -llua52 -L$(YOUR_LUA52_LIBPATH) -I$(YOUR_LUA52_INCLUDE)
 */

#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>

#define DEFAULT_LINECOUNT 1024

static int
wrapper(lua_State *L) {
	int p = 0;
	lua_State *tL = lua_tothread(L, lua_upvalueindex(1));
	switch(lua_resume(tL, L, 0)) {
	case LUA_YIELD:
		lua_settop(tL, 0);
		return 0;
	case LUA_OK:
		p = lua_gettop(tL);
		lua_pushboolean(L, 1);
		lua_xmove(tL, L, p);
		return p+1;
	default:
		lua_pushboolean(L, 0);
		luaL_traceback(L, tL, lua_tostring(tL, 1), 1);
		lua_settop(tL, 0);
		return 2;
	}
}

static void
hook(lua_State *L, lua_Debug *ar) {
	lua_yield(L,0);
}

static int
lthread(lua_State *L) {
	lua_State * nL;
	int count = luaL_optinteger(L, 2, DEFAULT_LINECOUNT);
	luaL_checktype(L, 1, LUA_TFUNCTION);
	nL = lua_newthread(L);
	lua_sethook(nL, hook, LUA_MASKCOUNT, count);

	lua_pushvalue(L, 1);
	lua_xmove(L, nL, 1);

	lua_pushcclosure(L, wrapper, 1);

	return 1;
}

int
luaopen_thread(lua_State *L) {
	luaL_checkversion(L);
	lua_pushcfunction(L, lthread);
	return 1;
}

