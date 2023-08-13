#pragma once
#include <null-render-backend-directx11.h>
#include "core/render/backend/shaders/domain-warping-shader.h"

namespace core::render::directx11 {
	namespace sources {
		static const std::vector<byte>& domain_warping() {
			#include "compiled/domain-warping.h"
			static const std::vector<byte> source{ shader_data, shader_data + sizeof(shader_data) };
			return source;
		}
	}

	class c_domain_warping_shader_object : public null::render::directx11::c_pixel_shader {
	public:
		null::render::directx11::c_constant_buffer<backend::i_domain_wrapping_shader::constants_t> constant_buffer{ };

	public:
		void set_constant(const backend::i_domain_wrapping_shader::constants_t& constant, int slot = 0) {
			constant_buffer.edit_constant(constant);
			set_constant_buffer(constant_buffer.buffer, slot);
		}

	public:
		void on_create() override {
			if (!empty()) return;
			create(sources::domain_warping());
			constant_buffer.create();
		}

		void on_destroy() override { destroy(); constant_buffer.destroy(); }
	} inline domain_warping_shader_object{ };
}