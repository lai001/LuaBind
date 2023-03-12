#pragma once
#include <memory>
#include <assert.h>
#include "Base.hpp"
#include "Define.hpp"

namespace luabind
{

    template <typename TClassType>
    class TClass;

    template <typename TClassType>
    struct LUABIND_API TClassConstructor
    {
        static int Create(lua_State *state)
        {
            assert(lua_isuserdata(state, lua_upvalueindex(1)));
            TClass<TClassType> *lClass = reinterpret_cast<TClass<TClassType> *>(lua_touserdata(state, lua_upvalueindex(1)));
            lua_newtable(state);
            lua_pushstring(state, luabind::UserDataKey);
            void *userData = lua_newuserdata(state, sizeof(TClassType));
            assert(lua_istable(state, 1));
            lua_rawset(state, 1);
            TClassType *classInstance = new (userData) TClassType();
            for (const typename Base<TClassType>::AddFunctionTaskType &task : lClass->addFunctionTasks)
            {
                assert(task);
                task(state, classInstance);
            }
            lua_pushstring(state, luabind::LuaGCFuncName);
            lua_pushcclosure(state, TClassConstructor<TClassType>::Deconstructor, 0);
            lua_rawset(state, 1);
            lua_pushvalue(state, 1);
            lua_setmetatable(state, 1);
            return 1;
        }

        static int Deconstructor(lua_State *state)
        {
            lua_getfield(state, 1, luabind::UserDataKey);
            TClassType *classInstance = reinterpret_cast<TClassType *>(lua_touserdata(state, lua_gettop(state)));
            lua_pop(state, 1);
            classInstance->~TClassType();
            return 0;
        }
    };

    template <typename TClassType>
    struct LUABIND_API TClassConstructor<std::shared_ptr<TClassType>>
    {
        static int Create(lua_State *state)
        {
            assert(lua_isuserdata(state, lua_upvalueindex(1)));
            TClass<TClassType> *lClass = reinterpret_cast<TClass<TClassType> *>(lua_touserdata(state, lua_upvalueindex(1)));
            lua_newtable(state);
            lua_pushstring(state, luabind::UserDataKey);
            void *userData = lua_newuserdata(state, sizeof(std::shared_ptr<TClassType>));
            assert(lua_istable(state, 1));
            lua_rawset(state, 1);
            std::shared_ptr<TClassType> *classInstance = new (userData) std::shared_ptr<TClassType>();
            for (const typename Base<TClassType>::AddFunctionTaskType &task : lClass->addFunctionTasks)
            {
                assert(task);
                task(state, classInstance.get());
            }
            return 1;
        }
    };

}
