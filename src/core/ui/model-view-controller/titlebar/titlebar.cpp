#include "core/ui/ui.h"
#include "titlebar.h"

namespace core::ui::mvc {
	void c_titlebar::collapse(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments) {
		ShowWindow(ui::window.wnd_handle, SW_MINIMIZE);
	}

	void c_titlebar::close(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments) {
		PostQuitMessage(0);
	}

	void c_titlebar::dragstart(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments) {
		GetCursorPos(&drag_offset);

		RECT rect{ };
		GetWindowRect(window.wnd_handle, &rect);
		drag_offset.x -= rect.left;
		drag_offset.y -= rect.top;
	}

	void c_titlebar::handledrag(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments) {
		if(event.GetTargetElement()->GetTagName() == "button")
			return;

		POINT cursor_pos{ };
		GetCursorPos(&cursor_pos);

		SetWindowPos(window.wnd_handle, nullptr, cursor_pos.x - drag_offset.x, cursor_pos.y - drag_offset.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	void c_titlebar::create() {
		constructor.BindFunc("get_architecture", [](Rml::Variant& architecture) {
#ifdef _WIN64
			architecture = "x64";
#else
			architecture = "x86";
#endif
		});
		constructor.BindEventCallback("close", &c_titlebar::close, this);
		constructor.BindEventCallback("collapse", &c_titlebar::collapse, this);
		constructor.BindEventCallback("title_dragstart", &c_titlebar::dragstart, this);
		constructor.BindEventCallback("title_handledrag", &c_titlebar::handledrag, this);
	}
}