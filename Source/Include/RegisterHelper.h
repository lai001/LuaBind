#pragma once
extern "C"
{
#include <lua.h>
}

#include <string>
#include <spdlog/spdlog.h>

struct RegisterHelper
{
    template <typename TClassType>
    static TClassType *getClassInstance(lua_State *state)
    {
        lua_getfield(state, 1, "_USERDATA");
        TClassType *instance = reinterpret_cast<TClassType *>(lua_touserdata(state, lua_gettop(state)));
        lua_pop(state, 1);
        return instance;
    }

    static std::string getClassName(lua_State *state)
    {
        lua_getfield(state, 1, "_CLASSNAME");
        std::string className = lua_tostring(state, lua_gettop(state));
        lua_pop(state, 1);
        return className;
    }

    template <typename TClassType>
    static int DefaultClassInstanceDestructor(lua_State *state)
    {
        TClassType *instance = RegisterHelper::getClassInstance<TClassType>(state);
        std::string className = RegisterHelper::getClassName(state);
        spdlog::debug("Destructor {}, {}", className, fmt::ptr(instance));
        delete instance;
        return 0;
    }

    template <typename TClassType>
    static void createDefaultClassInstanceLuaTableWrapper(lua_State *state, const char* className, void *instance)
    {
        lua_newtable(state);
        lua_pushstring(state, "_USERDATA");
        lua_pushlightuserdata(state, instance);
        lua_rawset(state, 1);
        lua_pushstring(state, "_CLASSNAME");
        lua_pushstring(state, className);
        lua_rawset(state, 1);
        lua_pushstring(state, "__gc");
        lua_pushcclosure(state, RegisterHelper::DefaultClassInstanceDestructor<TClassType>, 0);
        lua_rawset(state, 1);
        lua_pushvalue(state, 1);
        lua_setmetatable(state, 1);
    }

    static void addCFunction(lua_State *state, const char *name, lua_CFunction func, const int tableIndex = 1);
};