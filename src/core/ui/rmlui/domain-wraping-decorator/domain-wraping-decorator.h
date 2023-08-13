#pragma once
#include <null-rmlui.h>

namespace core::ui::rmlui {
	class c_domain_wraping_decorator : public Rml::Decorator {
	public:
		enum class e_uvs {
			box,
			viewport,
			viewport_offseted,
		};

		enum class e_aspect {
			box,
			viewport
		};

	public:
		struct instancer_t : public Rml::DecoratorInstancer {
		public:
			Rml::PropertyId
				color{ RegisterProperty("color", "white").AddParser("color").GetId() },
				uvs{ RegisterProperty("uvs", "from-box").AddParser("keyword", "from-box, from-viewport, from-viewport-offseted").GetId() },
				aspect{ RegisterProperty("aspect", "from-box").AddParser("keyword", "from-box, from-viewport").GetId() },
				mix_detailing{ RegisterProperty("mix_detailing", "1").AddParser("number").GetId() },
				detailing{ RegisterProperty("detailing", "2/4").AddParser("ratio").GetId() },
				speed{ RegisterProperty("speed", "1/1").AddParser("ratio").GetId() },
				x_offset{ RegisterProperty("x_offset", "1/1").AddParser("ratio").GetId() },
				y_offset{ RegisterProperty("y_offset", "1/1").AddParser("ratio").GetId() },
				first_color{ RegisterProperty("first_color", "white").AddParser("color").GetId() },
				second_color{ RegisterProperty("second_color", "white").AddParser("color").GetId() };
		public:
			instancer_t() {
				RegisterShorthand("domain-wraping", "color, mix_detailing, detailing, aspect, speed, x_offset, y_offset, first_color, second_color", Rml::ShorthandType::FallThrough);
			}

		public:
			std::shared_ptr<Rml::Decorator> InstanceDecorator(const std::string& name, const Rml::PropertyDictionary& properties, const Rml::DecoratorInstancerInterface& instancer_interface) override;
		}; static inline std::unique_ptr<instancer_t> instancer{ };

		static void register_instancer() { instancer = std::make_unique<instancer_t>(); Rml::Factory::RegisterDecoratorInstancer("domain-wraping", instancer.get()); }

	public:
		struct style_t {
		public:
			e_uvs uvs{ };
			e_aspect aspect{ };
			int mix_detailing{ };
			vec2_t<int> detailing{ };
			vec2_t<float> speed{ }, x_offset{ }, y_offset{ };
			color_t<int> first_color{ }, second_color{ }, color{ };
		} style{ };

		struct data_t {
		public:
			rect_t<float> box{ };
			null::render::path::rounding_t rounding{ };

			style_t style{ };
		};

	public:
		c_domain_wraping_decorator(const style_t& _style) : style{ _style } { }

	public:
		Rml::DecoratorDataHandle GenerateElementData(Rml::Element* element) const override;
		void ReleaseElementData(Rml::DecoratorDataHandle element_data) const override { delete (data_t*)element_data; }

		void RenderElement(Rml::Element* element, Rml::DecoratorDataHandle element_data) const override;
	};
}