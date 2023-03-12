#pragma once

#ifdef _WIN32
#ifdef LUABIND_BUILD_DLL_EXPORT
#define LUABIND_API __declspec(dllexport)
#elif (defined LUABIND_DLL)
#define LUABIND_API __declspec(dllimport)
#else
#define LUABIND_API
#endif 
#else
#define LUABIND_API
#endif