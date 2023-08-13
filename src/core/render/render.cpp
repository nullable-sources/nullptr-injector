#include "directx11/shaders/domain-warping-shader/domain-warping-shader.h"

namespace core::render {
	void initialize() {
		backend::domain_warping_shader = std::make_unique<render::directx11::c_domain_warping_shader>();
	}
}