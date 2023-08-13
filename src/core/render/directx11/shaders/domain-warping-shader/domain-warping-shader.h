#pragma once
#include <null-render-backend-directx11.h>
#include "core/render/directx11/shaders/domain-warping-shader/compiled-object.h"

namespace core::render::directx11 {
	class c_domain_warping_shader : public backend::i_domain_wrapping_shader, public null::render::directx11::c_shader {
	public:
		float global_time{ };

	public:
		c_domain_warping_shader() : c_shader{ &domain_warping_shader_object, &null::render::directx11::passthrough_shader_object } {
			null::render::backend::object_event_dispatcher.attach_listener(null::render::backend::e_object_event_type::begin_render, this);
		}

		~c_domain_warping_shader() {
			null::render::backend::object_event_dispatcher.detach_listener(null::render::backend::e_object_event_type::begin_render, this);
		}

	public:
		void on_begin_render() override;

	public:
		void use() override;

	public:
		void set_constants(const constants_t& _constants) override;
	};
}
