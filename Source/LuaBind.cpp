#include "LuaBind.hpp"
#include <memory>

int luabind::report(lua_State *state, int status, std::string *errorMsg) noexcept
{
    if (status != LUA_OK)
    {
        const char *msg = lua_tostring(state, -1);
        if (errorMsg)
        {
            *errorMsg = msg;
        }
        lua_pop(state, 1);
    }
    return status;
}

int luabind::runFile(lua_State *state, const std::string &scriptFile, std::string *errorMsg) noexcept
{
    return luabind::report(state, luaL_dofile(state, scriptFile.c_str()), errorMsg);
}

std::string luabind::dumpstack(lua_State *state) noexcept
{
    int top = lua_gettop(state);

    std::string msg = "\n";

    for (int i = 1; i <= top; i++)
    {
        std::string typeName = luaL_typename(state, i);
        switch (lua_type(state, i))
        {
        case LUA_TNONE:
            msg += typeName + "\n";
            break;

        case LUA_TNUMBER:
            msg += std::to_string(lua_tonumber(state, i)) + "\n";
            break;

        case LUA_TSTRING:
            msg += std::string(lua_tostring(state, i)) + "\n";
            break;

        case LUA_TBOOLEAN:
            msg += std::string((lua_toboolean(state, i) ? "true" : "false")) + "\n";
            break;

        case LUA_TNIL:
            msg += typeName + "\n";
            break;

        case LUA_TUSERDATA:
            msg += typeName + "\n";
            break;

        case LUA_TTABLE:
            msg += typeName + "\n";
            break;

        case LUA_TLIGHTUSERDATA:
            msg += typeName + "\n";
            break;

        case LUA_TFUNCTION:
            msg += typeName + "\n";
            break;
        }
    }

    return msg;
}

std::string luabind::getTypeName(const int typeCode) noexcept
{
    switch (typeCode)
    {
        case LUA_TNONE:
            return "NONE";
        case LUA_TNIL:
            return "NIL";
        case LUA_TBOOLEAN:
            return "BOOLEAN";
        case LUA_TLIGHTUSERDATA:
            return "LIGHTUSERDATA";
        case LUA_TNUMBER:
            return "NUMBER";
        case LUA_TSTRING:
            return "STRING";
        case LUA_TTABLE:
            return "TABLE";
        case LUA_TFUNCTION:
            return "FUNCTION";
        case LUA_TUSERDATA:
            return "USERDATA";
        case LUA_TTHREAD:
            return "THREAD";
        default:
            return "";
    }
}
