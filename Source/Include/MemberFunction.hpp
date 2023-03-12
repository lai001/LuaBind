#pragma once

#include "Define.hpp"
#include "RegisterHelper.h"

namespace luabind
{

    template <typename TReturnType, typename TClassType, typename TParameter1Type, typename TParameter2Type>
    struct TMemberFunction
    {
        static int call(lua_State *state)
        {
            TReturnType (*memberFunction)(TClassType *, TParameter1Type, TParameter2Type) = reinterpret_cast<TReturnType (*)(TClassType *, TParameter1Type, TParameter2Type)>(lua_touserdata(state, lua_upvalueindex(1)));
            TClassType *instance = RegisterHelper::getClassInstance<TClassType>(state);
            TParameter1Type v1 = luabind::Stack<TParameter1Type>::get(state, 2);
            TParameter2Type v2 = luabind::Stack<TParameter2Type>::get(state, 3);
            TReturnType result = memberFunction(instance, v1, v2);
            luabind::Stack<TReturnType>::set(state, result);
            return 1;
        }
    };

    template <typename TClassType, typename TParameter1Type, typename TParameter2Type>
    struct TMemberFunction<void, TClassType, TParameter1Type, TParameter2Type>
    {
        static int call(lua_State *state)
        {
            void (*memberFunction)(TClassType *, TParameter1Type, TParameter2Type) = reinterpret_cast<void (*)(TClassType *, TParameter1Type, TParameter2Type)>(lua_touserdata(state, lua_upvalueindex(1)));
            TClassType *instance = RegisterHelper::getClassInstance<TClassType>(state);
            TParameter1Type v1 = luabind::Stack<TParameter1Type>::get(state, 2);
            TParameter2Type v2 = luabind::Stack<TParameter2Type>::get(state, 3);
            memberFunction(instance, v1, v2);
            return 0;
        }
    };

}
