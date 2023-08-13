#pragma once
#include <null-render.h>

namespace core::render::backend {
	class i_domain_wrapping_shader : public virtual null::render::backend::i_shader {
	public:
		struct constants_t {
		public:
			color_t<int> first_color{ }, second_color{ };
			vec2_t<float> speed{ }, x_offset{ }, y_offset{ };
			vec2_t<int> detailing{ };
			float aspect{ }, time{ std::numeric_limits<float>::min() };
			int mix_detailing{ };
		};

	public:
		virtual void set_constants(const constants_t& constants) = 0;
	}; inline std::unique_ptr<i_domain_wrapping_shader> domain_warping_shader{ };
}