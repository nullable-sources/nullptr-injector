#include "animations.h"

namespace core::ui::mvc {
	void c_animations::running_line(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments) {
		Rml::Element* label{ event.GetCurrentElement()->GetFirstChild() };

		float target_position{ 0.f };
		if(event.GetId() == Rml::EventId::Mouseover) {
			float label_size{ label->GetClientWidth() };
			float conainer_size{ event.GetCurrentElement()->GetClientWidth() };

			if(label_size <= conainer_size) return;
			target_position = conainer_size - label_size;
		}

		label->Animate("transform", Rml::Transform::MakeProperty({ Rml::Transforms::TranslateX{ target_position } }), arguments[0].Get<float>(), Rml::Tween{ Rml::Tween::Quadratic, Rml::Tween::In }, 1, true, arguments[1].Get<float>());
	}

	void c_animations::create() {
		constructor.BindEventCallback("running_line", &c_animations::running_line, this);
	}
}