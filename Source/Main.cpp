#include <assert.h>
#include <string>
#include <iostream>
#include <memory>
#include <functional>
#include <type_traits>
#include <cstdint>

#include <spdlog/spdlog.h>

#include "LuaBind.hpp"
#include "AllClassRegister.h"

int main(int argc, char** argv)
{
	spdlog::set_level(spdlog::level::trace);

	const std::string testFile = "./lua/Entry.lua";
	lua_State* state = luaL_newstate();
	luaL_openlibs(state);

	{
		registerAllClass(state);
	}

	//{
	//	luabind::TClass<A> classA = luabind::TClass<A>(state, "A");
	//	classA.addFunction("mf_const", &A::mf_const);
	//	classA.addFunction("mf1", &A::mf1);
	//	classA.addFunction("mf2", &A::mf2);
	//	classA.submit();

	//	A* a = new A();
	//	a->data = 1000;
	//	luabind::Stack<A*>::set(state, "t3", a);
	//	A* retrieve = luabind::Stack<A*>::get(state, "t3");
	//	spdlog::debug("retrieve->data: {}", retrieve->data);
	//}

	std::string errorMsg;
	int status = 0;
	if (luabind::runFile(state, testFile, &errorMsg) != LUA_OK)
	{
		spdlog::error(errorMsg);
		status = 1;
	}

	lua_close(state);
	return status;
}
