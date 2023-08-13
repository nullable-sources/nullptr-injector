#pragma once
#include <null-rmlui-renderer-directx11.h>

namespace core::ui {
	inline null::render::directx11::c_window window{ };
	inline Rml::Context* context{ };
	inline Rml::ElementDocument* document{ };

	void initialize();
}