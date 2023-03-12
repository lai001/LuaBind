#pragma once
#include <string>
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <spdlog/spdlog.h>
#include "Define.hpp"
#include "ClassConstructor.hpp"

namespace luabind
{

    enum class ELuaValueType
    {
        None,
        Nil,
        Boolean,
        Lightuserdata,
        Number,
        String,
        Table,
        Function,
        Userdata,
        Thread,
    };

    static ELuaValueType getType(lua_State *state, int index)
    {
        std::unordered_map<int, ELuaValueType> typeMap;
        typeMap[LUA_TNONE] = ELuaValueType::None;
        typeMap[LUA_TNIL] = ELuaValueType::Nil;
        typeMap[LUA_TBOOLEAN] = ELuaValueType::Boolean;
        typeMap[LUA_TLIGHTUSERDATA] = ELuaValueType::Lightuserdata;
        typeMap[LUA_TNUMBER] = ELuaValueType::Number;
        typeMap[LUA_TSTRING] = ELuaValueType::String;
        typeMap[LUA_TTABLE] = ELuaValueType::Table;
        typeMap[LUA_TFUNCTION] = ELuaValueType::Function;
        typeMap[LUA_TUSERDATA] = ELuaValueType::Userdata;
        typeMap[LUA_TTHREAD] = ELuaValueType::Thread;
        return typeMap[lua_type(state, index)];
    }

    template <typename TValueType>
    struct LUABIND_API Stack
    {
        static TValueType get(lua_State *state, int index);
        static void set(lua_State *state, TValueType value);
    };

    template <>
    struct LUABIND_API Stack<int>
    {
        static int get(lua_State *state, int index)
        {
            return static_cast<int>(lua_tonumber(state, index));
        }

        static void set(lua_State *state, int value)
        {
            lua_pushinteger(state, value);
        }
    };

    template <>
    struct LUABIND_API Stack<float>
    {
        static float get(lua_State *state, int index)
        {
            return static_cast<float>(lua_tonumber(state, index));
        }

        static void set(lua_State *state, float value)
        {
            lua_pushnumber(state, value);
        }
    };

    template <>
    struct LUABIND_API Stack<double>
    {
        static double get(lua_State *state, int index)
        {
            return static_cast<double>(lua_tonumber(state, index));
        }

        static void set(lua_State *state, double value)
        {
            lua_pushnumber(state, value);
        }
    };

    template <>
    struct LUABIND_API Stack<std::string>
    {
        static std::string get(lua_State *state, int index)
        {
            return std::string(lua_tostring(state, index));
        }

        static void set(lua_State *state, std::string value)
        {
            lua_pushstring(state, value.c_str());
        }
    };

    template <typename TClassType>
    struct LUABIND_API Stack<TClassType *>
    {
        static TClassType *get(lua_State *state, int index)
        {
            TClassType *classInstance = static_cast<TClassType *>(lua_touserdata(state, index));
            assert(classInstance);
            return classInstance;
        }

        static TClassType *get(lua_State *state, const std::string &paramName)
        {
            lua_getglobal(state, paramName.c_str());
            assert(lua_istable(state, lua_gettop(state)));
            lua_getfield(state, lua_gettop(state), luabind::UserDataKey);
            assert(lua_islightuserdata(state, lua_gettop(state)));
            TClassType *classInstance = reinterpret_cast<TClassType *>(lua_touserdata(state, lua_gettop(state)));
            assert(classInstance);
            lua_pop(state, 2);
            return classInstance;
        }

        static void set(lua_State *state, const std::string &paramName, TClassType *classInstance)
        {
            lua_getglobal(state, Base<TClassType>::getLuaClassTemplateName().c_str());
            assert(lua_islightuserdata(state, 1));
            TClass<TClassType> *lClass = reinterpret_cast<TClass<TClassType> *>(lua_touserdata(state, 1));
            assert(lClass);

            lua_pop(state, 1);
            lua_newtable(state);
            lua_pushstring(state, luabind::UserDataKey);
            lua_pushlightuserdata(state, classInstance);
            assert(lua_istable(state, 1));
            lua_rawset(state, 1);

            for (const typename Base<TClassType>::AddFunctionTaskType &task : lClass->addFunctionTasks)
            {
                assert(task);
                task(state, classInstance);
            }
            assert(lua_istable(state, 1));
            lua_setglobal(state, paramName.c_str());
        }
    };
}
