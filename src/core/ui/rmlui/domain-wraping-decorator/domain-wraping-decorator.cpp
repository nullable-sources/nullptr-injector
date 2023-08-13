#include "core/render/graphic/filters/domain-warping-filter/domain-warping-filter.h"
#include "domain-wraping-decorator.h"

namespace core::ui::rmlui {
	std::shared_ptr<Rml::Decorator> c_domain_wraping_decorator::instancer_t::InstanceDecorator(const std::string& name, const Rml::PropertyDictionary& properties, const Rml::DecoratorInstancerInterface& instancer_interface) {
		style_t style{
			.uvs{ (e_uvs)properties.GetProperty(uvs)->Get<int>() },
			.aspect{ (e_aspect)properties.GetProperty(aspect)->Get<int>() },
			.mix_detailing{ properties.GetProperty(mix_detailing)->Get<int>() },
			.detailing{ vec2_t<float>{ properties.GetProperty(detailing)->Get<Rml::Vector2f>() } },
			.speed{ properties.GetProperty(speed)->Get<Rml::Vector2f>() },
			.x_offset{ properties.GetProperty(x_offset)->Get<Rml::Vector2f>() },
			.y_offset{ properties.GetProperty(y_offset)->Get<Rml::Vector2f>() },
			.first_color{ properties.GetProperty(first_color)->Get<Rml::Colourb>() },
			.second_color{ properties.GetProperty(second_color)->Get<Rml::Colourb>() },
			.color{ properties.GetProperty(color)->Get<Rml::Colourb>() }
		};

		return std::make_shared<c_domain_wraping_decorator>(style);
	}

	Rml::DecoratorDataHandle c_domain_wraping_decorator::GenerateElementData(Rml::Element* element) const {
		const Rml::Box& box{ element->GetBox() };

		const Rml::Vector2f padding_size{ box.GetSize(Rml::Box::PADDING).Round() };
		const Rml::Vector2f padding_position{ Rml::Math::RoundFloat(box.GetEdge(Rml::Box::BORDER, Rml::Box::Edge::LEFT)), Rml::Math::RoundFloat(box.GetEdge(Rml::Box::BORDER, Rml::Box::Edge::TOP)) };
		const Rml::Vector2f border{ element->GetAbsoluteOffset(Rml::Box::BORDER) };

		const Rml::ComputedValues& computed{ element->GetComputedValues() };
		return (Rml::DecoratorDataHandle)new data_t{
			{ padding_position, padding_position + padding_size },
			{
				//@todo: ���������� ���� ������
				computed.border_top_width() != 0.f || computed.border_left_width() != 0.f ? 0.f : computed.border_top_left_radius(),
				computed.border_top_width() != 0.f || computed.border_right_width() != 0.f ? 0.f : computed.border_top_right_radius(),
				computed.border_bottom_width() != 0.f || computed.border_left_width() != 0.f ? 0.f : computed.border_bottom_left_radius(),
				computed.border_bottom_width() != 0.f || computed.border_right_width() != 0.f ? 0.f : computed.border_bottom_right_radius()
			},
			style
		};
	}

	void c_domain_wraping_decorator::RenderElement(Rml::Element* element, Rml::DecoratorDataHandle element_data) const {
		data_t* data{ (data_t*)element_data };

		const vec2_t<float> box_size{ data->box.size() };

		float aspect{ };
		if(data->style.aspect == e_aspect::viewport) aspect = null::render::shared::viewport.x / null::render::shared::viewport.y;
		else aspect = box_size.x / box_size.y;

		rect_t<float> uvs{ };
		switch(data->style.uvs) {
			case e_uvs::box: { uvs = { { 0.f }, { 1.f } }; } break;
			case e_uvs::viewport: { uvs = { { 0.f }, box_size / null::render::shared::viewport }; } break;
			case e_uvs::viewport_offseted: { uvs = (data->box + vec2_t<float>{ element->GetAbsoluteOffset(Rml::Box::BORDER) }) / null::render::shared::viewport; } break;
		}

		const Rml::ComputedValues& computed{ element->GetComputedValues() };
		null::rml::render_interface->draw_list.add_command(std::make_unique<null::rml::renderer::c_restore_command>());
		null::rml::render_interface->draw_list.add_convex_shape(
			null::render::path::make_rect(data->box + (vec2_t<float>)element->GetAbsoluteOffset(Rml::Box::BORDER), data->rounding),
			null::render::filter_brush_t{ }
			.set_color({ data->style.color, (int)(data->style.color.a * computed.opacity()) })
			.set_filter(
				render::domain_wraping_filter_t{ }
					.set_uvs(uvs)
					.set_mix_detailing(data->style.mix_detailing)
					.set_detailing(data->style.detailing)
					.set_aspect(aspect)
					.set_speed(data->style.speed)
					.set_x_offset(data->style.x_offset)
					.set_y_offset(data->style.y_offset)
					.set_first_color(data->style.first_color)
					.set_second_color(data->style.second_color)
			)
		);
	}
}