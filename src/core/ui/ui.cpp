#pragma comment(lib, "dwmapi.lib")
#include <dwmapi.h>

#include "rmlui/domain-wraping-decorator/domain-wraping-decorator.h"
#include "model-view-controller/data-model.h"
#include "ui.h"

namespace core::ui {
	void initialize() {
		window.callbacks.at<utils::win::e_window_callbacks::on_wnd_proc>().add([&](HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) { return null::rml::backend::wnd_proc(ui::context, hwnd, msg, w_param, l_param); });
		window.callbacks.at<utils::win::e_window_callbacks::on_wnd_proc>().add([](HWND, UINT msg, WPARAM, LPARAM) { if (msg == WM_SIZE) Rml::ReleaseTextures(); return -1; });
		window.callbacks.at<utils::win::e_window_callbacks::on_main_loop>().add([&]() {
			if(IsIconic(window.wnd_handle)) return;

			null::render::begin_frame(window); {
				mvc::event_dispatcher.update();
				context->Update();
				mvc::event_dispatcher.late_update();

				context->Render();
			} null::render::end_frame();

			null::render::backend::renderer->begin_render();
			null::rml::render_interface->render();
			null::render::backend::renderer->end_render();
			});

		DragAcceptFiles(window.wnd_handle, true);

		//@note: removing window background
		HRGN region{ CreateRectRgn(0, 0, -1, -1) };
		DWM_BLURBEHIND blur_behind{
			.dwFlags{ DWM_BB_ENABLE | DWM_BB_BLURREGION },
			.fEnable{ TRUE },
			.hRgnBlur{ region }
		};
		DwmEnableBlurBehindWindow(ui::window.wnd_handle, &blur_behind);
		DeleteObject(region);

		null::rml::render_interface = std::make_unique<null::rml::directx11::c_render>();
		null::rml::set_default_interfaces(ui::window);
		null::rml::initialize();

		null::rml::extensions::register_all();
		rmlui::c_domain_wraping_decorator::register_instancer();

		null::rml::load_system_font();
		Rml::LoadFontFace("[resource:rml] icons.ttf");
		Rml::LoadFontFace("[resource:rml] roboto-regular.ttf");

		if(!(context = Rml::CreateContext("main", window.size)))
			utils::logger(utils::e_log_type::error, "Rml::CreateContext return nullptr");

		mvc::initialize();

		if(document = context->LoadDocument("[resource:rml] main.rml")) document->Show();
		else utils::logger(utils::e_log_type::error, "Cant load main document");
	}
}