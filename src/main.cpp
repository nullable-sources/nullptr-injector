#include "core/render/render.h"
#include "core/ui/ui.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show) {
#if _DEBUG
	utils::console::attach();
#endif

	core::ui::window = null::render::directx11::c_window{ };
	core::ui::window.wnd_class.hIcon = (HICON)LoadImage(GetModuleHandleA(nullptr), L"window.ico", IMAGE_ICON, 256, 256, LR_DEFAULTCOLOR);
	core::ui::window.size = { 500, 300 };
	core::ui::window.clear_color = { 0, 0 };
	core::ui::window.wnd_class.style = 0x2;
	core::ui::window.styles = WS_POPUP;

	try {
		core::ui::window.create();

		core::render::initialize();
		core::ui::initialize();

		core::ui::window.main_loop();

		null::rml::shutdown();
		core::ui::window.destroy();
	} catch (const std::exception& exception) {
		std::cout << exception.what() << std::endl;
	}

    return 0;
}