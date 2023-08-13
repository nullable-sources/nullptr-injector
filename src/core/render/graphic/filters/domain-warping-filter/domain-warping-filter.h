#pragma once
#include <null-render.h>
#include "core/render/backend/shaders/domain-warping-shader.h"

namespace core::render {
	class c_domain_wraping_filter : public null::render::i_filter {
	public:
		backend::i_domain_wrapping_shader::constants_t constants{ };

	public:
		c_domain_wraping_filter(std::unique_ptr<null::render::i_command>&& _child_command, const backend::i_domain_wrapping_shader::constants_t& _constants)
			: i_filter{ std::move(_child_command) }, constants{ _constants } { }

	public:
		virtual void handle() override;
	};

	struct domain_wraping_filter_t : public null::render::i_filter_instancer {
	public:
		rect_t<float> uvs{ };

		backend::i_domain_wrapping_shader::constants_t constants{ };

	public:
		template <typename self_t> auto&& set_uvs(this self_t&& self, const rect_t<float>& uvs) { self.uvs = uvs; return self; }
		template <typename self_t> auto&& set_mix_detailing(this self_t&& self, int mix_detailing) { self.constants.mix_detailing = mix_detailing; return self; }
		template <typename self_t> auto&& set_detailing(this self_t&& self, const vec2_t<int>& detailing) { self.constants.detailing = detailing; return self; }
		template <typename self_t> auto&& set_aspect(this self_t&& self, float aspect) { self.constants.aspect = aspect; return self; }
		template <typename self_t> auto&& set_speed(this self_t&& self, const vec2_t<float>& speed) { self.constants.speed = speed; return self; }
		template <typename self_t> auto&& set_x_offset(this self_t&& self, const vec2_t<float>& x_offset) { self.constants.x_offset = x_offset; return self; }
		template <typename self_t> auto&& set_y_offset(this self_t&& self, const vec2_t<float>& y_offset) { self.constants.y_offset = y_offset; return self; }
		template <typename self_t> auto&& set_first_color(this self_t&& self, const color_t<int>& first_color) { self.constants.first_color = first_color; return self; }
		template <typename self_t> auto&& set_second_color(this self_t&& self, const color_t<int>& second_color) { self.constants.second_color = second_color; return self; }
		template <typename self_t> auto&& set_time(this self_t&& self, float time) { self.constants.time = time; return self; }

	public:
		std::unique_ptr<null::render::i_filter> instance_filter(std::unique_ptr<null::render::c_geometry_command>&& child_command) const {
			child_command->set_uvs(uvs);
			return std::make_unique<c_domain_wraping_filter>(std::move(child_command), constants);
		}
	};
}