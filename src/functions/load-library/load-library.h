#pragma once
#pragma comment(lib, "dbghelp.lib")
#include <Windows.h>
#include <Psapi.h>
#include <Dbghelp.h>
#include <string_view>

namespace functions::load_library {
	void inject(int pid, std::string_view dll_path);
}
