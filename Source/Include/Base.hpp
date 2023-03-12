#pragma once

#include <functional>
extern "C"
{
#include <lua.h>
}
#include "Define.hpp"

namespace luabind
{
    static const char *UserDataKey = "_USERDATA";
    static const char *LuaGCFuncName = "__gc";

    template <typename TClassType>
    struct LUABIND_API Base
    {
        using AddFunctionTaskType = std::function<void(lua_State *, TClassType *)>;

        static std::string className;

        static std::string getLuaClassTemplateName() 
        {
            return Base<TClassType>::className + "_LuaClassTemplate";
        }
    };

    template <typename TClassType>
    std::string Base<TClassType>::className;
}