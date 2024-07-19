extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h> 
}

#include <string>
#include <sstream>

void DumpStackAt(lua_State* L, int AtIndex, std::stringstream& ss, const std::string& Padding);

void DumpTableAt(lua_State* L, int AtIndex, std::stringstream& ss, const std::string& Padding, const std::string& ParentPadding)
{
	//printf("DumpTableAt begin: %s\n", ss.str().c_str());
	// 把table压入栈顶
	//printf("DumpTableAt top : %d, AtIndex : %d\n", lua_gettop(L), AtIndex);

	lua_pushvalue(L, AtIndex);
	//printf("DumpTableAt lua_pushvalue top : %d, AtIndex : %d\n", lua_gettop(L), AtIndex);

	ss << "{\n";
	lua_pushnil(L); // 首先压入一个nil值作为初始键
	while (lua_next(L, -2) != 0) { // 使用lua_next遍历table
		ss << Padding;
		
		// 获取当前键
		DumpStackAt(L, -2, ss, Padding);

		ss << " = ";

		// 获取当前值
		DumpStackAt(L, -1, ss, Padding);

		ss<<",\n";
		
		lua_pop(L, 1); // 弹出栈顶的值，保留键用于下一次迭代
	}
	ss << ParentPadding << "}";

	//printf("DumpTableAt before pop top : %d, AtIndex : %d\n", lua_gettop(L), AtIndex);
	
	lua_pop(L, 1);

	//printf("DumpTableAt after pop top : %d, AtIndex : %d\n", lua_gettop(L), AtIndex);
}

void DumpStackAt(lua_State* L, int AtIndex, std::stringstream& ss, const std::string& Padding)
{
	switch (lua_type(L, AtIndex)) {
	case LUA_TNIL:
		ss<<"nil";
		break;
	case LUA_TBOOLEAN:
		ss<<(lua_toboolean(L, AtIndex) ? "true" : "false");
		break;
	case LUA_TLIGHTUSERDATA:
		ss<<"LUA_TLIGHTUSERDATA";
		break;
	case LUA_TNUMBER:
		{
			double val = lua_tonumber(L, AtIndex);
			if(std::abs(static_cast<long long>(val) - val) < 1e-6)
				ss<<std::to_string((long long)(val));
			else
				ss<<std::to_string(val);
		}
		break;
	case LUA_TSTRING:
		ss<<'"'<<lua_tostring(L, AtIndex)<<'"';
		break;
	case LUA_TTABLE:
		DumpTableAt(L, AtIndex, ss, Padding + "  ", Padding);
		break;
	case LUA_TFUNCTION:
		ss<<("LUA_TFUNCTION");
		break;
	case LUA_TUSERDATA:
		ss<<("LUA_TUSERDATA");
		break;
	case LUA_TTHREAD:
		ss<<("LUA_TTHREAD");
		break;
	default:
		ss<<(std::to_string(reinterpret_cast<long long>(lua_topointer(L, AtIndex))));
		break;
	}
}

void TestDumpStack(lua_State* L)
{
	luaL_loadfile(L, "scriptt_for_dump.lua");
	lua_pcall(
		L,
		0,
		0,
		0
	);

	lua_getglobal(L, "data");
	lua_getglobal(L, "height");
	lua_getglobal(L, "nil_value");
	lua_getglobal(L, "person");
	lua_getglobal(L, "weight");
	int type = lua_type(L, -1);
	const int NumParams = lua_gettop(L);

	printf("before DumpStack top = %d\n", lua_gettop(L));
	//LuaStack.Append(FString::Printf(TEXT("DumpLuaStack LuaStack Params Count:%d\n"), NumParams));
	std::stringstream ss;
	for (int i = 1; i <= NumParams; i++)
	{
		DumpStackAt(L, i, ss, "");
		ss<<",\n";
	}
	printf("after DumpStack top = %d\n", lua_gettop(L));
	printf("DumpStack person type = %d NumParams = %d\n", type, NumParams);
	printf("%s\n", ss.str().c_str());
}

int main()
{
	lua_State* L;
	L = luaL_newstate();

	luaL_openlibs(L);

	TestDumpStack(L);
	
	lua_close(L);                               /* Clean up, free the Lua state var */
	return 0;
}