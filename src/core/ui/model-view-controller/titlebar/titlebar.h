#pragma once
#include "core/ui/model-view-controller/data-model.h"

namespace core::ui::mvc {
	class c_titlebar : public i_data_model {
	private:
		POINT drag_offset{ };

	public:
		c_titlebar() { event_dispatcher.attach_listener(e_event_type::create, this); }
		~c_titlebar() { event_dispatcher.detach_listener(e_event_type::create, this); }

	public:
		void collapse(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments);
		void close(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments);

		void dragstart(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments);
		void handledrag(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments);

	public:
		void create() override;
	} inline titlebar{ };
}