#pragma once
#include <string>
#include <cstdlib>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif // !_MSVC

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "Define.hpp"

namespace luabind
{
    int LUABIND_API report(lua_State *state, int status, std::string *errorMsg) noexcept;

    int LUABIND_API runFile(lua_State *state, const std::string &scriptFile, std::string *errorMsg) noexcept;

    std::string LUABIND_API dumpstack(lua_State *state) noexcept;

    std::string LUABIND_API getTypeName(const int typeCode) noexcept;

    template <typename T>
    std::string LUABIND_API type_name()
    {
        std::string tname = typeid(T).name();
#ifndef _MSC_VER
        int status;
        char* demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
        if (status == 0)
        {
            tname = demangled_name;
            std::free(demangled_name);
        }
#endif
        return tname;
    }
}
