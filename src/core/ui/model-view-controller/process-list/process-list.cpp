#include <Windows.h>
#include <Psapi.h>
#include <tlhelp32.h>

#include "process-list.h"

namespace core::ui::mvc {
	bool c_process_list::process_t::can_show() {
		if(!name_filter.empty() && !(name.contains(name_filter) || name_filter.contains(std::to_string(pid))))
			return false;

		if(!filters.empty() && !(std::ranges::contains(filters, type) || std::ranges::contains(filters, architecture)))
			return false;

		return true;
	}

	bool c_process_list::process_t::selected() {
		return unique_name ? (selected_process_name == name) : (selected_process_name == std::to_string(pid));
	}

	void c_process_list::process_t::select(bool new_select) {
		if(new_select) {
			if(unique_name) selected_process_name = name;
			else selected_process_name = std::to_string(pid);
		} else if(selected()) selected_process_name.clear();
	}

	void c_process_list::processes_updating() {
		static const auto enum_windows{ [](HWND hwnd, LPARAM pid) -> BOOL {
			DWORD hwnd_pid{ };
			GetWindowThreadProcessId(hwnd, &hwnd_pid);

			return *(std::uint32_t*)pid != hwnd_pid;
		} };

		static const auto process_architecture{ [](HANDLE process) {
			SYSTEM_INFO system_info{ };
			GetNativeSystemInfo(&system_info);
			if(system_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) return "x86";
			else {
				BOOL result{ };
				IsWow64Process(process, &result);
				return result ? "x86" : "x64";
			}
		} };

		while(true) {
			std::unique_lock lock{ sync_lock };

			HANDLE snapshot{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };
			PROCESSENTRY32 entry{ { sizeof(PROCESSENTRY32) } };
			if(!Process32First(snapshot, &entry)) return;

			std::vector<process_t> all_processes{ };
			while(Process32Next(snapshot, &entry)) {
				HANDLE handle{ OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID) };
				if(!handle) continue;

				process_t process{
					.name{ utils::encoding::utf8_to_ascii(entry.szExeFile) },
					.type{ EnumWindows(enum_windows, (LPARAM)&entry.th32ProcessID) ? "background" : "application" },
					.architecture{ process_architecture(handle) },
					.pid{ entry.th32ProcessID },
					.unique_name{ std::ranges::find_if(all_processes, [&](const process_t& proc) { return proc.name == process.name; }) == all_processes.end() }
				};

				all_processes.push_back(process);
			}

			CloseHandle(snapshot);

			std::vector<process_t> filtered_process{ all_processes | std::views::filter(&process_t::can_show) | std::views::take(50) | std::ranges::to<std::vector>() };

			//@note: we wait until the main thread notifies about the possibility of changing data in order to synchronize their change only after calling Rml::Context::Update
			data_sync_gate.wait(lock);

			if(auto finded{ std::ranges::find_if(all_processes, &process_t::selected) }; finded != all_processes.end()) selected_pid = finded->pid;
			else selected_pid = -1;
			handle.DirtyVariable("selected_pid");

			processes = filtered_process;
			handle.DirtyVariable("processes");

			processes_count = all_processes.size();
			handle.DirtyVariable("processes_count");
		}
	}

	void c_process_list::create() {
		processes_updating_worker = std::thread{ processes_updating };

		constructor.Bind("name_filter", &name_filter);
		constructor.BindFunc("filters",
			[](Rml::Variant& variant) { variant = filters | std::views::join_with('&') | std::ranges::to<std::string>(); },
			[](const Rml::Variant& variant) { filters = variant.Get<std::string>() | std::views::split('&') | std::views::transform([](auto rng) { return std::string{ rng.begin(), rng.end() }; }) | std::ranges::to<std::vector>(); }
		);
		constructor.Bind("selected_pid", &selected_pid);

		if(auto process{ constructor.RegisterStruct<process_t>() }) {
			constructor.RegisterArray<std::vector<process_t>>();

			process.RegisterMember("name", &process_t::name);
			process.RegisterMember("pid", &process_t::pid);
			process.RegisterMember("architecture", &process_t::architecture);
			process.RegisterMember("selected", &process_t::selected, &process_t::select);
		} constructor.Bind("processes", &processes);
		constructor.Bind("processes_count", &processes_count);
	}

	void c_process_list::late_update() {
		//@note: we notify the process update flow that it is now possible to update the data
		data_sync_gate.notify_all();
	}
}