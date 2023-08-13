#include "domain-warping-shader.h"

namespace core::render::directx11 {
	void c_domain_warping_shader::on_begin_render() {
		static utils::c_segment_time_measurement time_measurement{ };
		static std::once_flag once_flag{ };
		std::call_once(once_flag, [&] { time_measurement.begin(); });

		time_measurement.update();
		global_time += 0.1f * std::chrono::duration<float>{ time_measurement.representation() }.count();
	}

	void c_domain_warping_shader::use() {
		if (empty()) return;
		c_shader::use();

		null::render::directx11::passthrough_shader_object.set_constant({ null::render::backend::renderer->get_matrix() });
	}

	void c_domain_warping_shader::set_constants(const constants_t& _constants) {
		if (_constants.time == std::numeric_limits<float>::min()) {
			constants_t constants{ _constants };
			constants.time = global_time;
			domain_warping_shader_object.set_constant(constants);
		} else {
			domain_warping_shader_object.set_constant(_constants);
		}
	}
}