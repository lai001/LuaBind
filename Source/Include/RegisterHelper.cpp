#include "RegisterHelper.h"

void RegisterHelper::addCFunction(lua_State *state, const char *name, lua_CFunction func, const int tableIndex)
{
    lua_pushstring(state, name);
    lua_pushcclosure(state, func, 0);
    lua_rawset(state, tableIndex);
}
