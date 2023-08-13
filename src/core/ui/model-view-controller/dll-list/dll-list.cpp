#include "functions/load-library/load-library.h"
#include "core/ui/ui.h"

#include "dll-list.h"

//@credits: https://stackoverflow.com/a/14562060
bool get_image_file_headers(std::string_view file_name, IMAGE_NT_HEADERS& headers) {
	HANDLE file_handle{ CreateFileA(file_name.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0) };
	if(file_handle == INVALID_HANDLE_VALUE)
		return false;

	HANDLE image_handle{ CreateFileMappingA(file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr) };
	if(image_handle == 0) {
		CloseHandle(file_handle);
		return false;
	}

	void* image_ptr{ MapViewOfFile(image_handle, FILE_MAP_READ, 0, 0, 0) };
	if(image_ptr == nullptr) {
		CloseHandle(image_handle);
		CloseHandle(file_handle);
		return false;
	}

	if(PIMAGE_NT_HEADERS headers_ptr{ ImageNtHeader(image_ptr) }; headers_ptr != nullptr) {
		headers = *headers_ptr;
	} else {
		UnmapViewOfFile(image_ptr);
		CloseHandle(image_handle);
		CloseHandle(file_handle);
		return false;
	}

	UnmapViewOfFile(image_ptr);
	CloseHandle(image_handle);
	CloseHandle(file_handle);

	return true;
}

namespace core::ui::mvc {
	c_dll_list::dll_t::dll_t(std::string_view _path) : path{ _path }, name{ std::filesystem::path{ _path }.filename().string() } {
		IMAGE_NT_HEADERS headers{ };
		if(!get_image_file_headers(path, headers))
			utils::logger(utils::e_log_type::error, "get_image_file_headers return false.");

		switch(headers.FileHeader.Machine) {
			case IMAGE_FILE_MACHINE_I386: { architecture = "x86"; } break;
			case IMAGE_FILE_MACHINE_AMD64: { architecture = "x64"; } break;
			default: { architecture = "unk"; } break;
		}
	}

	bool c_dll_list::dll_t::selected() {
		return selected_dll_path == path;
	}

	void c_dll_list::dll_t::select(bool new_select) {
		if(new_select) selected_dll_path = path;
		else if(selected()) selected_dll_path.clear();
		handle.DirtyVariable("selected_dll");
	}

	void c_dll_list::file_selecting() {
		while(true) {
			std::unique_lock lock{ sync_lock };
			resume_gate.wait(lock);

			char buffer[260]{ '\0' };
			OPENFILENAMEA opf{
				.lStructSize{ sizeof(OPENFILENAMEA) },
				.hwndOwner{ NULL },
				.lpstrFilter{ ".dll\0*.dll" },
				.nFilterIndex{ 1 },
				.lpstrFile{ buffer },
				.nMaxFile{ sizeof(buffer) },
				.lpstrFileTitle{ NULL },
				.nMaxFileTitle{ 0 },
				.lpstrInitialDir{ NULL },
				.Flags{ OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST }
			};

			dll_t new_dll{ };
			if(GetOpenFileNameA(&opf))
				new_dll = dll_t{ opf.lpstrFile };

			data_sync_gate.wait(lock);

			if(!new_dll.path.empty() && std::ranges::find_if(dlls, [&](const dll_t& dll) { return dll.path == new_dll.path; }) == dlls.end()) {
				dlls.push_back(new_dll);
				handle.DirtyVariable("dlls");
			}

			adding_locked = false;
			handle.DirtyVariable("adding_locked");
		}
	}

	void c_dll_list::add_new(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments) {
		if(adding_locked) return;

		resume_gate.notify_all();
		adding_locked = true;
		handle.DirtyVariable("adding_locked");
	}

	void c_dll_list::remove_selected(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments) {
		if(selected_dll_path.empty()) return;

		auto range{ std::ranges::remove_if(dlls, [&](const dll_t& dll) { return dll.path == selected_dll_path; }) };
		dlls.erase(range.begin(), range.end());
		handle.DirtyVariable("dlls");

		selected_dll_path.clear();
		handle.DirtyVariable("selected_dll");
	}

	void c_dll_list::inject_selected(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments) {
		if(selected_dll_path.empty()) return;
		if(arguments.empty()) return;

		int pid{ arguments[0].Get<int>() };
		if(pid == -1) return;

		functions::load_library::inject(pid, selected_dll_path);
	}

	int c_dll_list::drag_and_drop(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
		if(msg == WM_DROPFILES) {
			HDROP hdrop{ (HDROP)wparam };
			size_t file_count{ DragQueryFileA(hdrop, -1, nullptr, 0) };

			for(int i : std::views::iota(0u, file_count)) {
				std::string path{ };
				path.resize(MAX_PATH);

				DragQueryFileA(hdrop, i, path.data(), MAX_PATH);

				if(path.empty() || !std::filesystem::path{ path }.extension().string().contains(".dll"))
					continue;

				if(std::ranges::find_if(dlls, [&](const dll_t& dll) { return dll.path == path; }) == dlls.end()) {
					dlls.push_back(dll_t{ path });
					handle.DirtyVariable("dlls");
				}
			}

			DragFinish(hdrop);
		}
		return -1;
	}

	void c_dll_list::create() {
		window.callbacks.at<utils::win::e_window_callbacks::on_wnd_proc>().add(drag_and_drop);

		file_selecting_worker = std::thread{ file_selecting };

		constructor.Bind("selected_dll", &selected_dll_path);
		constructor.Bind("adding_locked", &adding_locked);

		constructor.BindEventCallback("add_new", &c_dll_list::add_new, this);
		constructor.BindEventCallback("remove_selected", &c_dll_list::remove_selected, this);
		constructor.BindEventCallback("inject_selected", &c_dll_list::inject_selected, this);

		if(auto dll{ constructor.RegisterStruct<dll_t>() }) {
			constructor.RegisterArray<std::vector<dll_t>>();

			dll.RegisterMember("name", &dll_t::name);
			dll.RegisterMember("path", &dll_t::path);
			dll.RegisterMember("architecture", &dll_t::architecture);

			dll.RegisterMember("selected", &dll_t::selected, &dll_t::select);
		} constructor.Bind("dlls", &dlls);
	}

	void c_dll_list::late_update() {
		data_sync_gate.notify_all();
	}
}