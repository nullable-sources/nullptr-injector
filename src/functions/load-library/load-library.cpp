#include "load-library.h"

namespace functions::load_library {
	void inject(int pid, std::string_view dll_path) {
		HANDLE handle{ OpenProcess(PROCESS_ALL_ACCESS, false, pid) };
		void* dll_path_address_in_process{ VirtualAllocEx(handle, 0, strlen(dll_path.data()), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE) };
		if(!dll_path_address_in_process) return;

		if(!WriteProcessMemory(handle, dll_path_address_in_process, dll_path.data(), strlen(dll_path.data()), nullptr)) return;

		HMODULE kernel_base{ GetModuleHandleA("kernel32.dll") };
		if(!kernel_base) return;

		void* load_library_address{ GetProcAddress(kernel_base, "LoadLibraryA") };
		if(!load_library_address) return;

		HANDLE remote_thread{ CreateRemoteThread(handle, nullptr, 0, (LPTHREAD_START_ROUTINE)load_library_address, dll_path_address_in_process, 0, nullptr) };
		if(!remote_thread) return;

		WaitForSingleObject(remote_thread, INFINITE);
		VirtualFreeEx(handle, dll_path_address_in_process, 0, MEM_RELEASE);
		CloseHandle(handle);
	}
}