#pragma once
#include "core/ui/model-view-controller/data-model.h"

namespace core::ui::mvc {
	class c_animations : public i_data_model {
	public:
		c_animations() { event_dispatcher.attach_listener(e_event_type::create, this); }
		~c_animations() { event_dispatcher.detach_listener(e_event_type::create, this); }

	public:
		void running_line(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments);

	public:
		void create() override;
	} inline animations{ };
}
