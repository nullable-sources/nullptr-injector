#include "domain-warping-filter.h"

namespace core::render {
	void c_domain_wraping_filter::handle() {
		backend::domain_warping_shader->set_constants(constants);
		backend::domain_warping_shader->use();

		child_command->handle();

		null::render::backend::passthrough_color_shader->use();
	}
}