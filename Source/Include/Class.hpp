#pragma once

#include <string>
#include <vector>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <spdlog/spdlog.h>

#include "Define.hpp"
#include "LuaBind.hpp"
#include "LuaStack.hpp"
#include "MemberFunction.hpp"
#include "ClassConstructor.hpp"

namespace luabind
{
    template <typename TClassType>
    class LUABIND_API TClass
    {
    public:
        explicit TClass(lua_State *state, const std::string &className)
            : className(className), state(state)
        {
            Base<TClassType>::className = className;
        }

        TClass(const TClass<TClassType> &other)
        {
            this->className = other.className;
            this->state = other.state;
            this->addFunctionTasks = other.addFunctionTasks;
        }

        ~TClass()
        {
        }

        void addFunction(const std::string &functionName, lua_CFunction luaCall)
        {
            typename Base<TClassType>::AddFunctionTaskType task = [=](lua_State *state, TClassType *classInstance)
            {
                lua_pushstring(state, functionName.c_str());
                lua_pushcclosure(state, luaCall, 0);
                assert(lua_istable(state, 1));
                lua_rawset(state, 1);
            };
            addFunctionTasks.push_back(task);
        }

        template <typename TReturnType, typename... TParameters>
        void addFunction(const std::string &functionName, TReturnType (TClassType::*function)(TParameters...) const)
        {
            using MemberFunctionType = TReturnType (TClassType::*)(TParameters...) const;
            typename Base<TClassType>::AddFunctionTaskType task = [=](lua_State *state, TClassType *classInstance)
            {
                lua_pushstring(state, functionName.c_str());
                lua_pushlightuserdata(state, (void *&)function);
                lua_pushcclosure(state, OnLuaCall<TReturnType, TParameters...>, 1);
                assert(lua_istable(state, 1));
                lua_rawset(state, 1);
            };
            addFunctionTasks.push_back(task);
        }

        template <typename TReturnType, typename... TParameters>
        void addFunction(const std::string &functionName, TReturnType (TClassType::*function)(TParameters...))
        {
            using MemberFunctionType = TReturnType (TClassType::*)(TParameters...) const;
            this->addFunction(functionName, (MemberFunctionType)function);
        }

        template <typename TReturnType, typename... TParameters>
        static int OnLuaCall(lua_State *state)
        {
            return TMemberFunction<TReturnType, TClassType, TParameters...>::call(state);
        }

        void submit()
        {
            void *userData = lua_newuserdata(state, sizeof(TClass<TClassType>));
            new (userData) TClass<TClassType>(*this);
            lua_pushcclosure(state, TClass<TClassType>::CreateClassInstanceTableWrapper, 1);
            lua_setglobal(state, className.c_str());
            lua_pushlightuserdata(state, userData);
            lua_setglobal(state, Base<TClassType>::getLuaClassTemplateName().c_str());
        }

        static int CreateClassInstanceTableWrapper(lua_State *state)
        {
            return TClassConstructor<TClassType>::Create(state);
        }

        std::string className;
        lua_State *state = nullptr;
        std::vector<typename Base<TClassType>::AddFunctionTaskType> addFunctionTasks;
    };

}
