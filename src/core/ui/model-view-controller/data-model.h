#pragma once
#include <null-rmlui.h>

namespace core::ui::mvc {
	inline Rml::DataModelHandle handle{ };
	inline Rml::DataModelConstructor constructor{ };

	enum class e_event_type { create, update, late_update };
	class c_event_dispatcher : public utils::c_event_dispatcher<e_event_type> {
	public:
		void create() { dispatch_event(e_event_type::create, { }); }
		void update() { dispatch_event(e_event_type::update, { }); }
		void late_update() { dispatch_event(e_event_type::late_update, { }); }
	} inline event_dispatcher{ };

	class i_data_model : public utils::i_event_listener<e_event_type> {
	private:
		void process_event(e_event_type id, const std::unordered_map<std::string, std::any>& parameters) override {
			switch(id) {
				case e_event_type::create: { create(); } break;
				case e_event_type::update: { update(); } break;
				case e_event_type::late_update: { late_update(); } break;
			}
		}

	public:
		virtual void create() { }
		virtual void update() { }
		virtual void late_update() { }
	};

	void initialize();
}